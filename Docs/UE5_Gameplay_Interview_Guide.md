# UE5 Gameplay 项目面试手册

> 适用项目：`Demo`，Unreal Engine 5.6。内容依据当前 C++ 工程整理，不把尚未实现的功能包装成已完成成果。

## 使用方法

第一遍只背“项目介绍”和每题的“面试回答”，目标是能在 60 秒内说清一个问题。第二遍打开对应源码，把回答中的类名、调用顺序和设计理由与代码对应起来。第三遍只看“继续追问”，脱稿回答。最后让别人从模拟题中随机抽题，每题限制两分钟；超过两分钟仍说不清，说明还没有形成稳定的知识结构。

## 一分钟项目介绍

这是一个基于 UE5.6、以 C++ 为主的第三人称 Gameplay Demo。我用 GAS 实现了属性、技能、GameplayEffect、伤害结算和受击状态；玩家的 ASC 与 AttributeSet 放在 PlayerState 上，敌人的 ASC 放在 Character 上，以适应两者不同的生命周期。输入层使用 Enhanced Input，并用 GameplayTag 把 InputAction 和 GameplayAbility 解耦。战斗部分包含 Montage 驱动的近战连招、GameplayEvent 命中窗口、球形 Sweep、SetByCaller 伤害参数、ExecutionCalculation 防御与暴击计算，以及自定义 EffectContext 同步暴击结果。

项目还包含组件化背包、DataTable 静态物品数据、消耗品 GameplayEffect、WidgetController 驱动的属性 UI、Behavior Tree 敌人 AI、锁定目标、NPC 对话和掉落物。当前工程已经使用属性复制、RepNotify、GAS 复制模式和 Multicast 等网络机制，但背包、交互和部分战斗判定仍以单机原型为主；如果扩展到联机，我会优先改成服务端权威并为背包使用 Fast Array Serializer。

## 三分钟项目展开顺序

先说明目标：用一个可运行项目串起 UE Gameplay 的主要系统，而不是分别制作孤立功能。随后讲架构：PlayerState 保存跨 Pawn 生命周期的玩家能力和属性，Character 负责表现、移动与交互，ActorComponent 承载背包和经验等可组合功能，WidgetController 把 UI 与 Gameplay 数据隔开，DataAsset 和 DataTable 保存静态配置。

再讲一条完整战斗链路：Enhanced Input 产生 InputAction，输入配置把它映射为 GameplayTag，ASC 根据 AbilitySpec 的动态标签激活 Ability；近战 Ability 播放 Montage 并等待 GameplayEvent，在命中窗口执行 Sweep，创建 GameplayEffectSpec，通过 SetByCaller 写入物理伤害；ExecCalc 捕获攻防、暴击和抗性，输出 IncomingDamage；AttributeSet 消费元属性、扣血、触发受击或死亡，并把伤害数字交给控制器显示。

最后主动讲限制：目前启动 Ability 和默认属性初始化可能因多次调用 `InitAbilityActorInfo` 而重复执行；锁定功能依赖 Tick，但基类构造函数关闭了 Tick；背包没有网络复制和事务式添加；近战命中还需要明确服务端执行策略。主动指出这些问题并给出修改顺序，比声称系统已经完全支持联机更可信。

## 项目架构速记

| 职责 | 当前类 | 面试时的解释 |
| --- | --- | --- |
| 玩家长期状态 | `AOPlayerState` | 保存 ASC、AttributeSet、等级和经验组件，Pawn 更换后仍可保留 |
| 玩家表现与输入 | `APlayerCharacter` | 相机、移动、交互、锁定、UI 开关和 ASC Avatar 初始化 |
| 通用战斗角色 | `ACharacterBase` | 武器、默认属性、初始技能、受击和死亡表现 |
| 敌人运行状态 | `AEnemyCharacter` | 自有 ASC、AttributeSet、血条、AI 与掉落 |
| 伤害公式 | `UExecCalc_Damage` | 集中捕获属性并计算伤害、抗性、防御和暴击 |
| 背包 | `UInventoryComponent` | 固定槽位、堆叠、使用物品和变化通知 |
| UI 中介 | `UPlayerWidgetController` 派生类 | 订阅 Gameplay 数据，再广播给 UMG |
| 输入映射 | `UPlayerInputConfig` | 用 DataAsset 保存 InputAction 与 GameplayTag 的对应关系 |

## 第一组：GAS 与战斗

### Q1：为什么玩家的 ASC 放在 PlayerState，而不是 Character？

**面试回答：** PlayerState 的生命周期通常长于 Pawn。玩家死亡重生或切换 Pawn 时，Character 可能销毁，但 PlayerState 仍存在，因此长期属性、持续 GameplayEffect 和已授予 Ability 不必重建。当前项目使用 PlayerState 作为 ASC 的 OwnerActor，Character 作为 AvatarActor，分别表达“谁拥有能力数据”和“谁在场景中执行能力”。敌人通常不会换 Pawn，所以敌人的 ASC 直接放在 EnemyCharacter 上，结构更简单。

**结合项目：** `AOPlayerState` 创建复制的 ASC 和 AttributeSet；`APlayerCharacter::InitAbilityActorInfo` 调用 `InitAbilityActorInfo(OPlayerState, this)`。

**继续追问：** OwnerActor 和 AvatarActor 分别有什么职责？重生后需要重新做哪些初始化？Mixed 模式为什么要求 OwnerActor 的 Owner 链能够找到 PlayerController？

### Q2：服务端和客户端分别什么时候初始化 AbilityActorInfo？

**面试回答：** 服务端在 `PossessedBy` 后已经拥有 Controller 和 PlayerState，可以初始化 ASC。客户端不能依赖 `PossessedBy`，通常在 `OnRep_PlayerState` 中初始化。初始化应设计成幂等流程：允许多次进入，但委托绑定、默认属性应用和 Ability 授予只能发生一次。`OnRep_Controller` 可以承担与本地控制器相关的初始化，但不应无条件重复授予能力。

**结合项目：** 当前 PlayerCharacter 从 `PossessedBy`、`OnRep_PlayerState` 和 `OnRep_Controller` 三处调用同一函数，后续应增加初始化标志，并保证 `GiveAbility` 只在 Authority 上执行。

**继续追问：** 为什么默认属性重复应用会有问题？如何判断 ASC 是否已经绑定过委托？

### Q3：Full、Mixed、Minimal 三种 GAS 复制模式有什么区别？

**面试回答：** Full 会向相关客户端复制完整 GameplayEffect 信息，适合玩家较少或需要完整信息的场景。Mixed 通常让拥有者收到完整 Effect，其他客户端主要接收 GameplayTag 和 GameplayCue，适合玩家角色。Minimal 主要复制 Tag 和 Cue，不向客户端复制完整 Active GameplayEffect，适合大量 AI。模式选择影响网络带宽和客户端可见信息，不影响服务端权威计算。

**结合项目：** PlayerState 的 ASC 使用 Mixed，EnemyCharacter 的 ASC 使用 Minimal，符合玩家与普通 AI 的信息需求差异。

**继续追问：** Attribute 是否仍然复制？GameplayCue 在 Minimal 下如何表现？

### Q4：AttributeSet 为什么使用 ReplicatedUsing 和 GAMEPLAYATTRIBUTE_REPNOTIFY？

**面试回答：** 服务端修改属性后，属性值通过普通属性复制到客户端；`ReplicatedUsing` 提供变化回调，`GAMEPLAYATTRIBUTE_REPNOTIFY` 让 ASC 的属性变化委托获得正确的新旧值。`DOREPLIFETIME_CONDITION_NOTIFY` 使用 `REPNOTIFY_Always`，即使预测值与服务端最终值相同，也能完成 GAS 的预测校正和通知流程。

**结合项目：** Health、MaxHealth、Attack、Defense、暴击、伤害加成和抗性都注册了复制与 OnRep。

**继续追问：** 为什么不能只在 OnRep 中直接更新血条？监听 ASC 属性变化委托有什么好处？

### Q5：为什么伤害先写入 IncomingDamage，而不是直接修改 Health？

**面试回答：** IncomingDamage 是元属性，只作为一次结算过程的中间值，不代表需要长期复制的角色状态。ExecCalc 只负责公式，把最终结果输出到 IncomingDamage；AttributeSet 在 `PostGameplayEffectExecute` 中统一消费它，处理扣血、死亡、受击反应和伤害数字。这样把“数值计算”和“结算副作用”分开，也便于以后插入护盾、免死和吸血等规则。

**继续追问：** 元属性为什么通常不复制？为什么消费后要立即清零？

### Q6：ExecutionCalculation 和 Modifier、MMC 应该如何选择？

**面试回答：** 简单的固定加减或可直接由属性表达的公式使用 Modifier。MMC 适合计算一个 Modifier 的 Magnitude，通常产生单个数值并可访问捕获属性。ExecutionCalculation 适合一次结算中读取多个源和目标属性、处理多伤害类型、暴击、防御并输出一个或多个 Modifier。当前伤害公式同时依赖攻击者、目标、防御曲线、抗性和 SetByCaller，因此 ExecCalc 更合适。

**继续追问：** Snapshot 捕获与非 Snapshot 捕获有什么区别？为什么 ExecCalc 本身不做持久状态修改？

### Q7：SetByCaller 在伤害链路中解决了什么问题？

**面试回答：** GameplayEffectClass 描述通用伤害规则，但每次攻击的基础伤害可能不同。Ability 创建 Spec 时用伤害类型 Tag 写入本次攻击的动态 Magnitude，ExecCalc 再按 Tag 读取。这样同一个伤害 GE 可以服务不同技能和不同元素，不需要为每个数值创建一个 GE 类。

**结合项目：** 近战 Ability 使用 `Damage.Physical` 写入曲线值，ExecCalc 根据“伤害类型到抗性”的 Tag 映射逐项结算。

**继续追问：** SetByCaller 缺失时应该报错还是使用零？Tag 和 FName 作为键有什么差异？

### Q8：为什么暴击结果放进自定义 GameplayEffectContext？

**面试回答：** 暴击是本次命中的结果，不是角色长期属性，也不适合靠全局变量传递。EffectContext 跟随 GameplayEffectSpec 在伤害链路中传播，并可通过 NetSerialize 把暴击标记同步给客户端。AttributeSet 消费伤害时读取同一个 Context，就能决定飘字样式、GameplayCue 或命中特效。

**继续追问：** 自定义 Context 需要重写哪些函数？为什么必须让 `AbilitySystemGlobals` 返回自定义 Context？

### Q9：输入如何通过 GameplayTag 激活 Ability？

**面试回答：** DataAsset 保存 InputAction 到 InputTag 的映射。自定义 EnhancedInputComponent 绑定动作时把 Tag 作为额外参数传给 Character；Character 再把 Pressed、Held、Released 转发给 ASC。授予 Ability 时，StartupInputTag 被写入 AbilitySpec 的动态 SourceTag；ASC 遍历可激活 Spec，找到精确匹配的 Tag 后记录输入并尝试激活。这样换键和换技能不需要在 Character 中硬编码 AbilityClass。

**继续追问：** 每次输入线性遍历 AbilitySpec 是否有性能问题？输入 Tag、Ability Tag 和状态 Tag 为什么要分层命名？

### Q10：近战连招为什么使用 AbilityTask 和 GameplayEvent？

**面试回答：** Montage 决定表现时间线，AnimNotify 在准确帧发送 GameplayEvent，AbilityTask 异步等待命中和连招窗口。玩家输入在窗口前到达时先缓存，窗口事件到达后再跳转下一个 Montage Section。Ability 的取消、打断、Montage 完成和混出都统一结束 Ability，避免状态残留。

**结合项目：** `UPlayerMeleeAttack` 使用 `WaitGameplayEvent`、`PlayMontageAndWait`、输入缓存和 `Attack1/Attack2` Section。

**继续追问：** 为什么 Timer 只能作为命中事件的兜底？Ability 被取消时还需要清理什么？

### Q11：近战命中判定应该在客户端还是服务端执行？

**面试回答：** 最终伤害必须由服务端确认。客户端可以先播放 Montage、预测输入和显示临时反馈，但不能自行决定命中和伤害。服务端应验证 Ability 是否激活、攻击窗口、距离、方向和目标有效性，然后创建并应用 GE。对高延迟动作游戏可以保存短时间位置历史做有限回溯，但不能无条件相信客户端 HitResult。

**结合项目：** 当前 Sweep 和 GE 应用已经形成完整单机链路，联机化时需要明确 Ability 的网络执行策略，并把命中确认固定在 Authority。

**继续追问：** Local Predicted Ability 如何避免重复伤害？TargetData 如何从客户端传到服务端？

### Q12：死亡为什么不能只在客户端播放动画？

**面试回答：** 死亡由服务端权威 Health 结算触发。服务端负责停止 AI、发经验、生成掉落和改变可交互状态；客户端只负责 Montage、布娃娃、碰撞和视觉表现。Multicast 可以通知当前相关客户端播放一次性表现，但持久状态仍应由复制变量表达，否则晚加入或重新相关的客户端无法恢复正确状态。

**继续追问：** Reliable Multicast 有什么带宽风险？为什么 `bIsDead` 最好做成 RepNotify 状态？

## 第二组：Gameplay Framework 与对象系统

### Q13：GameMode、GameState、PlayerState 和 PlayerController 如何分工？

**面试回答：** GameMode 只存在于服务端，负责规则、出生和胜负判定。GameState 在服务端存在并复制到客户端，保存全局比赛状态。PlayerState 表示一个玩家的可复制长期状态，适合等级、队伍和跨 Pawn 数据。PlayerController 表示玩家的控制与连接，服务端和所属客户端各有一份，适合输入、HUD、本地 UI 与 Client RPC。Character 是场景中的可替换 Avatar，负责移动、碰撞和表现。

**继续追问：** 为什么 UI 不应直接放在 GameMode？其他客户端能否拿到你的 PlayerController？

### Q14：CreateDefaultSubobject、NewObject 和 SpawnActor 有什么区别？

**面试回答：** `CreateDefaultSubobject` 主要在 UObject/Actor 构造阶段创建默认子对象，并参与 CDO 和蓝图默认值体系。`NewObject` 创建普通 UObject，需要合适的 Outer 和可达引用才能避免 GC。`SpawnActor` 通过 UWorld 创建 Actor，进入 Actor 的生成、组件初始化和 BeginPlay 生命周期，还能设置碰撞和 Owner、Instigator 等参数。

**结合项目：** ASC、AttributeSet、相机、武器和背包组件属于默认子对象；Widget 是运行时 `CreateWidget`；掉落物使用 `SpawnActor`。

### Q15：Actor 的主要生命周期顺序是什么？

**面试回答：** 面试中先给稳定主线：构造函数创建默认组件，实例生成后执行组件注册和初始化，`PostInitializeComponents` 后进入 `BeginPlay`，运行期可能 Tick，结束时先收到 `EndPlay`，之后才等待销毁和 GC。编辑器放置、运行时 Spawn、网络复制到客户端的细节顺序不同，因此不要把构造函数当作访问 World、PlayerController 或运行时数据的地方。

**继续追问：** `Destroy()` 是否立即释放内存？`EndPlay` 的不同 Reason 有什么意义？

### Q16：UPROPERTY 和 TObjectPtr 在当前项目里分别解决什么问题？

**面试回答：** `UPROPERTY` 把字段纳入反射系统，使 GC、序列化、复制、编辑器和蓝图能够按 Specifier 处理它。`TObjectPtr` 是 UE5 用于 UObject 成员引用的包装类型，在编辑器和引擎内部提供对象句柄能力；它不会自动替代 `UPROPERTY` 的反射与 GC 可达性声明。临时局部变量通常可以继续使用裸指针，成员引用则根据所有权和生命周期选择 `TObjectPtr`、`TWeakObjectPtr` 或 `TSoftObjectPtr`。

### Q17：TWeakObjectPtr 和 TSoftObjectPtr 如何选择？

**面试回答：** Weak 指向已经加载的 UObject，不阻止对象被 GC，对象销毁后可以检测失效，适合锁定目标和临时观察者。Soft 保存资源路径，可以在资源未加载时存在，适合可异步加载的配置资产、图标和大资源。Soft 解决加载依赖，Weak 解决运行时非拥有引用，两者不是同一种弱引用。

### Q18：为什么使用 UE Interface，而不是到处 Cast？

**面试回答：** Interface 定义调用方真正依赖的能力，例如可交互、可战斗、可高亮，而不是依赖某个具体 Character 类。它允许蓝图和不同 Actor 提供各自实现，也减少 PlayerCharacter 对 NPC、敌人和拾取物类型的耦合。需要注意接口不保存普通实例状态，执行 BlueprintNativeEvent 时应通过 `Execute_` 入口调用。

**结合项目：** 交互 Sweep 检查 `InteractableInterface`，锁定系统只依赖 `EnemyInterface`，伤害系统通过 `CombatInterface` 获取等级与触发死亡。

## 第三组：背包、UI、输入与 AI

### Q19：为什么背包做成 ActorComponent？

**面试回答：** 背包具有独立状态和行为，可组合到玩家或其他拥有者，不属于 Character 的移动与表现职责。组件便于复用、单独测试和通过委托通知 UI。是否放在 Character 取决于生命周期：如果死亡换 Pawn 后仍应保留，应把组件放到 PlayerState，或者由 PlayerState 保存数据、Pawn 上的组件只做代理。

**继续追问：** 组件复制需要哪些设置？PlayerState 上的背包如何让本地 UI 访问？

### Q20：DataTable 和 PrimaryDataAsset 应该如何选择？

**面试回答：** DataTable 适合结构一致、需要批量编辑或从表格导入的轻量静态数据，例如名称、最大堆叠和数值。PrimaryDataAsset 更适合每项资源有独立资产、字段复杂、需要 AssetManager 分组扫描和异步加载的场景。当前物品数量少且结构统一，DataTable 是合理原型；物品规模扩大、图标和 Mesh 需要按需加载时，会把大资源改为 Soft 引用，或迁移到 PrimaryDataAsset。

### Q21：当前 AddItem 算法有什么边界问题？

**面试回答：** 当前实现会先修改已有堆叠，再寻找一个空槽；如果剩余数量超过单槽 MaxStack，它可能把超量数量放进一个空槽。若已有堆叠被部分填充后发现没有空槽，函数返回 false，但之前的修改不会回滚，调用者无法知道实际加入了多少。生产实现应先计算容量，或者返回 AddedAmount，并循环拆分多个新堆叠，保证操作语义明确。

**继续追问：** 如何让 AddItem 成为事务操作？背包满时拾取物能否直接销毁？

### Q22：联机背包如何同步？

**面试回答：** 客户端只发送“尝试拾取或使用”的请求，服务端验证距离、物品状态、权限和冷却，然后修改背包。槽位数组适合使用 `FFastArraySerializer` 做增量复制，每个 Entry 带 ReplicationID 和 ReplicationKey；UI 响应 Fast Array 的 Add、Change、Remove 回调。静态物品定义不必重复复制，只同步稳定 ItemID、数量和必要实例数据。

### Q23：WidgetController 解决了什么耦合？

**面试回答：** Widget 不直接了解 PlayerState、ASC 和 AttributeSet 的获取细节。WidgetController 持有这些依赖，先广播初始值，再订阅属性委托、经验组件和 Effect Tag，最后用面向 UI 的委托广播。这样 Widget 只负责显示和交互，Gameplay 层也不需要持有具体 Widget。

**继续追问：** 为什么初始化值和变化值要分两步？页面关闭后如何解除委托？

### Q24：AddLambda 捕获 this 有什么生命周期风险？

**面试回答：** 如果委托的拥有者比监听对象活得更久，普通 Lambda 中保存的裸 `this` 可能在监听对象销毁后被调用。可使用返回的 DelegateHandle 在销毁时 Remove，或使用支持 UObject 生命周期跟踪的 `AddUObject`、`AddWeakLambda`。动态多播委托则使用 `AddDynamic` 并在适当时机解绑，尤其要防止重复初始化造成重复绑定。

**结合项目：** OverlayWidgetController 和 EnemyCharacter 都有 Lambda 订阅点，是面试时可以主动说明的改进项。

### Q25：Enhanced Input 相比旧输入系统提供了什么？

**面试回答：** Input Mapping Context 可以按角色、载具、UI 状态动态叠加和设定优先级；InputAction 表达逻辑动作，Trigger 决定 Started、Triggered、Completed 等状态，Modifier 负责死区、缩放和轴变换。它把物理按键与 Gameplay 动作解耦，更适合重绑定、多设备和情境输入。项目再用 GameplayTag 做第二层映射，把动作与具体 Ability 解耦。

### Q26：Behavior Tree 为什么只在服务端运行？

**面试回答：** AI 决策会改变移动、技能和世界状态，应由服务端权威执行，客户端只接收 Pawn 移动、动画和 Gameplay 状态的复制结果。让每个客户端运行同一棵树既浪费性能，也可能因为感知和时序差异产生不同决策。当前 EnemyCharacter 在服务端 `PossessedBy` 后初始化 Blackboard 并运行 Behavior Tree，符合这个原则。

### Q27：受击 GameplayTag 如何驱动 AI？

**面试回答：** AttributeSet 在非致命伤害后按 Tag 激活受击 Ability；ASC 的 Tag 计数变化委托通知 EnemyCharacter，Character 将移动速度设为零，并更新 Blackboard 的 HitReacting。Behavior Tree 可以通过 Decorator 或分支避免受击期间继续攻击。Tag 是跨 GAS、Character 和 AI 的状态协议，减少了系统之间的直接调用。

### Q28：锁定目标算法使用了哪些数学知识？

**面试回答：** 先用球形 Overlap 做空间候选集，再把相机到目标的方向归一化，与相机 Forward 做点积。点积越接近 1，目标越靠近视线中心；阈值 0.6 用来排除侧后方目标。当前只按角度评分，后续可加入距离、遮挡和屏幕中心偏移，并用加权分数选择目标。

**项目风险：** `ACharacterBase` 关闭了 Actor Tick，而 PlayerCharacter 的锁定旋转写在 Tick 中；若派生类没有重新启用 Tick，这段逻辑不会运行。面试时应说明会改为“锁定时启用 Tick，解除时关闭”，既修复功能又避免常驻开销。

## 第四组：网络、性能与 C++

### Q29：Authority、Ownership 和 Relevancy 有什么区别？

**面试回答：** Authority 表示哪个实例对 Actor 状态有最终决定权，常规客户端服务器模型中是服务端。Ownership 是 Owner 链关系，决定 Client RPC 目标、条件复制和客户端是否有资格调用某些 Server RPC。Relevancy 决定某个连接当前是否需要接收 Actor 的复制数据。三者分别回答“谁决定”“属于哪个连接”“谁需要看到”，不能混用。

### Q30：Server、Client 和 NetMulticast RPC 应该如何使用？

**面试回答：** Server RPC 从拥有该 Actor 的客户端请求服务端操作，服务端必须重新验证。Client RPC 从服务端发给拥有者，适合私有 UI 或反馈。NetMulticast 由服务端调用并发送给当前相关客户端，适合短暂表现，不适合承载持久状态。Reliable 只保证有序可靠传输，不代表可以高频使用；移动和连续特效通常需要属性复制、GameplayCue 或 Unreliable RPC。

### Q31：如何把当前交互和拾取改成服务端权威？

**面试回答：** 客户端输入只调用 ServerTryInteract，传候选 Actor 或必要的视线信息。服务端重新检查 Actor 有效性、接口、距离、视角、遮挡和拾取物是否仍存在，然后执行 AddItem；只有服务端确认加入成功后才销毁拾取物。背包变化通过复制返回客户端，UI 不以客户端本地 AddItem 的结果为准。

### Q32：什么时候使用 Tick、Timer、Delegate 和 GameplayEvent？

**面试回答：** 每帧都依赖连续时间的逻辑才使用 Tick，例如锁定状态下平滑旋转，并且只在需要时启用。低频周期任务使用 Timer。数据发生变化后通知消费者使用 Delegate。跨 GAS 或动画时间线传递语义事件使用 GameplayEvent。选择依据不是语法方便，而是更新频率、所有权、取消方式和生命周期。

### Q33：当前项目有哪些明确的性能优化？

**面试回答：** CharacterBase 和 InventoryComponent 默认关闭 Tick；背包和 UI 通过委托事件刷新；AI 决策放在 Behavior Tree；静态配置从 DataTable/DataAsset 读取；敌人 ASC 使用 Minimal 复制模式。下一步会用 Insights 验证，而不是凭感觉优化，重点检查 Widget 重建、AbilitySpec 输入遍历、大量 AI 的 BT Service 频率、硬资源引用和锁定查询频率。

### Q34：TArray 为什么通常比链表更适合 Gameplay 数据？

**面试回答：** TArray 连续存储，遍历时缓存局部性好，按索引访问是常数时间，UE 的序列化和反射也广泛支持。中间删除会移动元素并使指针或迭代器失效，因此需要根据语义选择 RemoveAtSwap、预留容量或稳定句柄。背包固定槽位使用 TArray 很自然，因为槽位顺序和索引本身就是 UI 语义。

### Q35：为什么 UObject 不应该由 TSharedPtr 管理？

**面试回答：** UObject 的生存由 UE GC 的可达性系统管理，TSharedPtr 是独立的引用计数系统，两者互相不知道对方的所有权。SharedPtr 引用计数大于零不能阻止 GC 回收 UObject。UObject 成员引用使用反射可见的强引用、Weak、Soft 或特定场景下的 `TStrongObjectPtr`；TSharedPtr 主要用于非 UObject 的纯 C++ 类型和 Slate 数据模型。

### Q36：const 引用、移动语义和 UE 容器在项目里如何落地？

**面试回答：** 只读且不需要复制的较大结构通过 `const T&` 传递，例如 GameplayTagContainer 和 Spec。`MoveTemp` 只有在对象之后不再使用、且类型支持移动时才有收益，它本身只是把表达式转换为可移动形式。容器扩容策略不是语言标准保证，不能死背为固定两倍；应使用 `Reserve` 表达已知容量，减少重分配和元素移动。

## 当前项目最可能被追问的六个薄弱点

### 初始化幂等性

`InitAbilityActorInfo` 有多个入口，当前还会应用默认属性和授予 StartupAbilities。回答时要承认需要把“绑定 ActorInfo”“应用一次性默认属性”“服务端授予 Ability”“本地 HUD 初始化”拆开，并分别加条件保护。

### 背包一致性

当前 AddItem 不是完整事务，不能正确处理任意数量跨多个空槽的情况，也没有复制。改造顺序是先定义返回语义，再修复多栈算法，之后加入服务端验证和 Fast Array。

### 战斗网络权威

当前项目用了属性复制和 GAS 复制模式，但不能据此声称近战已经完整支持联机。需要补充 Authority 命中、预测 Ability、TargetData 验证和重复命中防护。

### 重复委托绑定

`AbilityActorInfoSet` 和多个 WidgetController/Enemy 委托绑定需要防止重复进入，并保存 DelegateHandle 或使用弱绑定，在对象结束生命周期时解除。

### 锁定 Tick

基类关闭 Tick，而锁定旋转依赖 PlayerCharacter::Tick。应在锁定开始时 `SetActorTickEnabled(true)`，解除锁定时关闭，并确认派生类允许 Tick。

### 硬资源引用与加载

背包 DataTable 中的 Texture、Mesh 当前是硬指针，规模扩大后会形成加载链。可改为 Soft Object Ptr，并由 AssetManager 或 StreamableManager 在 UI 或拾取物需要时异步加载。

## 两周复习安排

| 天数 | 主题 | 当天必须产出 |
| --- | --- | --- |
| 1 | 项目介绍与架构 | 脱稿讲完一分钟和三分钟版本 |
| 2 | UObject、GC、引用类型 | 画出对象可达性与三种创建方式 |
| 3 | Actor 生命周期、Framework | 说清五个 Framework 类的职责 |
| 4 | ASC 初始化与复制模式 | 手画服务器、拥有客户端初始化时序 |
| 5 | AttributeSet 与 GE | 讲清属性复制和 IncomingDamage |
| 6 | ExecCalc 与伤害链路 | 从输入一路讲到扣血和飘字 |
| 7 | AbilityTask、Montage、连招 | 说明取消、打断和命中窗口 |
| 8 | RPC、Ownership、Authority | 把拾取改造流程口述一遍 |
| 9 | 背包与数据驱动 | 写出正确的跨槽堆叠伪代码 |
| 10 | UI、委托、生命周期 | 找出所有绑定点并说明解绑策略 |
| 11 | AI 与 GameplayTag | 讲清受击状态如何影响行为树 |
| 12 | 性能与资产加载 | 用 Insights 术语描述测量步骤 |
| 13 | C++、容器、智能指针 | 完成本手册 Q34-Q36 的追问 |
| 14 | 模拟面试 | 45 分钟录音，复盘含糊和超时回答 |

## 模拟面试题

1. 从按下鼠标到敌人扣血，把你项目里的完整调用链讲一遍。
2. 为什么玩家 ASC 在 PlayerState，敌人 ASC 却在 Character？
3. 现在让角色死亡后重生，哪些数据会保留，哪些对象要重新初始化？
4. 你的近战系统在 100 ms 延迟下可能出现什么问题？
5. 如果客户端伪造一个高额 SetByCaller 伤害，服务端如何防止作弊？
6. 为什么伤害使用 ExecCalc，而回血药可以直接使用 GameplayEffect Modifier？
7. 背包一次拾取 250 个、最大堆叠 99，当前算法会怎样？你如何修改？
8. 20 格背包如何同步给所属客户端？为什么不每次复制整个数组？
9. WidgetController 被销毁后，ASC 的 Lambda 委托可能发生什么？
10. 一百个敌人都运行 Behavior Tree 时，你会先测量什么？
11. 锁定系统为什么选择点积？如何加入遮挡与距离权重？
12. `Destroy()`、`EndPlay()`、GC 和 `TWeakObjectPtr` 之间是什么关系？
13. GameMode 和 GameState 各保存什么？为什么客户端拿不到 GameMode？
14. Mixed 与 Minimal 复制模式分别减少了什么数据？
15. 如何证明你的优化有效，而不是只说“关闭 Tick、使用对象池”？

## 面试表达边界

可以说：“我已经实现了 GAS 属性复制、Mixed/Minimal 模式、RepNotify、自定义 EffectContext 和死亡 Multicast，并针对完整联机列出了下一步改造。”不要说：“项目已经完整支持多人联机。”

可以说：“背包使用组件、DataTable 和事件驱动 UI，静态数据与运行时槽位已经分离。”不要说：“背包可直接支撑 MMO 规模。”

可以说：“近战通过 Montage Event、AbilityTask、Sweep 和 ExecCalc 形成了完整链路。”不要说：“命中判定已经解决延迟补偿与反作弊。”

可以说：“我能主动找出重复初始化、委托生命周期、Tick 和背包事务问题，并给出改造顺序。”这会把项目中的不足转化为工程判断力，而不是等面试官发现后被动解释。
