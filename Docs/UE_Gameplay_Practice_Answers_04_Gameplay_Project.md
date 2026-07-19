# 刷题答案第四卷：Gameplay 与项目题

## GAS、AI、动画与 UI

### 186. ASC 的 OwnerActor 与 AvatarActor

OwnerActor 持有能力系统数据并决定长期所有权，AvatarActor 是当前在世界中执行能力、提供移动与表现的实体。玩家可用 PlayerState 作为 Owner、Character 作为 Avatar，重生后只更换 Avatar；普通敌人两者可以都是自身 Character。

### 187. 玩家 ASC 放 PlayerState

PlayerState 通常跨 Pawn 死亡重生存在，并且是可复制的玩家长期状态容器，适合保留 Ability、Attribute 和持续 Effect。代价是初始化必须正确建立 Owner 链与 Avatar，并在新 Pawn 上重新绑定 ActorInfo 和本地表现。

### 188. AbilityActorInfo 初始化时机

服务端在 Pawn `PossessedBy`、PlayerState 与 Controller 均可用后初始化；客户端通常在 `OnRep_PlayerState` 中初始化，必要时再处理 Controller 相关 UI。流程必须幂等，委托绑定、默认属性和 Ability 授予分别设保护，其中 GiveAbility 只由 Authority 执行。

### 189. GE、Modifier、MMC 与 ExecCalc

GameplayEffect 是效果容器和持续时间策略；简单固定或属性驱动运算用 Modifier；需要计算一个 Modifier Magnitude 且读取捕获属性时用 MMC；一次结算需要多个源/目标属性、条件、伤害类型和多个输出时用 ExecutionCalculation。

### 190. GE 生命周期

Instant GE 立即执行 Modifier，不进入 ActiveGameplayEffects；Duration GE 在指定时长内存在并可周期执行；Infinite GE 持续到显式移除。Duration/Infinite 可产生 Tag、Cue、持续 Modifier 和堆叠，并受复制模式影响。

### 191. Attribute RepNotify 与 Meta Attribute

Attribute 由服务端权威修改并复制，RepNotify 配合 `GAMEPLAYATTRIBUTE_REPNOTIFY` 触发 ASC 属性变化和预测校正。Meta Attribute 如 IncomingDamage 只承载一次结算中间值，PostGameplayEffectExecute 消费后清零，不代表需要长期复制的状态。

### 192. GameplayTag 分层

输入 Tag 表达按键意图，Ability Tag 表达能力身份与阻挡/取消关系，State Tag 表达 Attacking、Stunned 等状态，Damage Tag 表达元素或结算类型。分层命名让系统通过语义匹配通信，避免拿同一个 Tag 同时承担输入、状态和伤害职责。

### 193. AbilityTask

AbilityTask 把 Montage 完成、GameplayEvent、输入、TargetData 等异步事件包装进 Ability 生命周期，并通过委托恢复逻辑。Ability 结束时任务会清理，但自建 Timer、外部 Delegate 和自定义资源仍需在取消/结束路径中明确释放。

### 194. 预测与重复结算

Local Predicted Ability 用 PredictionKey 把客户端预测行为与服务端确认关联；可预测的本地 Effect 在服务端副本到达后被协调，而不可预测的最终伤害只由服务端产生。命中请求还需 AbilitySpecHandle、预测键、目标数据和服务端验证，确保同一攻击窗口只结算一次。

### 195. GAS 复制模式

Full 向相关客户端复制完整 Active GameplayEffect；Mixed 通常让 Owner 收到完整 Effect，其他人主要收到 Tag/Cue，适合玩家；Minimal 主要复制 Tag/Cue，适合大量 AI。Attribute 仍按自身属性复制规则同步，模式不是“敌人属性不复制”。

### 196. Behavior Tree 节点职责

Task 执行一个具体动作并返回运行状态；Service 在分支激活期间周期更新感知或 Blackboard；Decorator 判断分支条件并可观察值变化中止分支。复杂行为应把决策留在树中，把可复用动作封装为 Task。

### 197. Service 更新什么

适合更新随时间变化且多个节点会读取的决策数据，如最近目标、距离、视线、攻击范围和环境评分。不会变化的数据不应周期计算，高成本感知应降低频率、事件驱动或分层更新。

### 198. EQS 与 NavMesh 楼层限制

EQS 生成候选点并用距离、可见性、可达性等 Test 过滤/评分，NavMesh 提供可行走区域与路径成本。限制同层可比较高度差、使用导航投影与 PathExist/PathCost Test，或按 Nav Area、楼层 Volume/Tag 分区，不能只按二维距离选点。

### 199. MoveTo 失败排查

依次检查 Controller/Pawn、NavMesh 是否覆盖、目标是否投影到导航面、Agent 半径高度与 NavData 是否匹配、路径是否存在、AcceptanceRadius、碰撞与动态障碍、Behavior Tree 中止原因。开启 `show Navigation`、Visual Logger 和 AI Debug，读取 MoveRequest 的失败状态而不是反复调参数。

### 200. 动画系统职责

AnimBP/AnimInstance 根据 Gameplay 状态计算姿态；State Machine 管理持续 locomotion 状态；BlendSpace 按速度方向混合样本；Montage 管理有明确时序、可中断和分段的攻击、技能与受击。Gameplay 状态是源，动画不应成为权威规则存储。

### 201. Montage 连招结构

Section 表示每段攻击并允许跳转；Slot 把 Montage 混入 AnimGraph 指定层；Notify/NotifyState 标记命中帧和连招窗口。输入先缓存，窗口事件到达后决定是否跳下一 Section；完成、混出、取消和打断都要统一清理 Ability 状态。

### 202. Root Motion 与 In-Place

Root Motion 从动画根骨提取位移，动作贴合好，适合翻滚、处决和固定攻击，但网络与碰撞协调更复杂。In-Place 由 CharacterMovement 驱动，响应和网络预测更稳定，适合常规移动；项目可按动作混合使用。

### 203. Motion Warping

Motion Warping 在动画时间窗口内调整 Root Motion，使角色对齐动态目标位置/朝向，同时保留动画节奏。它适合处决、翻越和近战贴合，但目标点必须由 Gameplay 规则验证，联机时不能让客户端任意指定落点。

### 204. Enhanced Input 四部分

Input Mapping Context 把物理输入映射到 Action 并支持优先级叠加；InputAction 表达 Move、Jump 等逻辑动作；Trigger 判断 Started/Triggered/Completed 等触发条件；Modifier 在触发前做死区、缩放、反转和轴变换。

### 205. Widget 与 Gameplay 解耦

Widget 生命周期短且面向显示，不应知道如何查找所有 ASC、PlayerState 和组件。使用 WidgetController/ViewModel 订阅 Gameplay 委托、广播初值和变化值，Widget 只渲染；页面销毁时解绑，避免重复绑定和悬空回调。

## 项目与场景题

### 206. 一分钟项目介绍

可按以下版本回答：“这是一个 UE5.6 第三人称 Gameplay Demo，我负责 C++ 架构和主要功能。项目用 GAS 实现属性、技能、伤害和状态，用 Enhanced Input 与 GameplayTag 解耦输入和能力，并实现了 Montage 连招、AI 行为树、背包与事件驱动 UI。当前网络部分完成属性复制、ASC 复制模式和部分 RPC 原型，下一步是把命中和背包改为完整服务端权威，并用 Insights 给出性能指标。”

### 207. 输入到伤害调用链

Enhanced Input 产生 Action，InputConfig 映射为 InputTag，自定义 InputComponent 把 Press/Hold/Release 转给 ASC；ASC 找到动态 Tag 匹配的 AbilitySpec 并激活 Ability；Ability 播放 Montage，命中 Notify 发送 GameplayEvent，AbilityTask 收到后做服务端 Sweep，创建 GE Spec 并写 SetByCaller；ExecCalc 捕获攻防、暴击和抗性，输出 IncomingDamage；AttributeSet 扣血并触发受击/死亡和 UI 委托。

### 208. 组件、继承、接口与 Subsystem

“是一个”且共享实现骨架时使用继承；依附 Actor、可组合且有状态的能力用 Component；只需要跨无关类型调用某项能力时用 Interface；服务覆盖整个 Engine/GameInstance/World/LocalPlayer 生命周期时才用对应 Subsystem。回答时必须说明数据所有权和生命周期，而不是只谈复用。

### 209. 难定位问题回答框架

使用“现象、稳定复现、证据、假设、实验、根因、修复、回归”顺序。以重复 GAS 初始化为例：属性或技能重复出现；记录三处初始化入口和 Authority；用日志标记调用时序；确认 PossessedBy/OnRep 重复执行一次性逻辑；拆分初始化并加幂等标志；最后在 Listen Server、客户端重连和重生场景回归。

### 210. 量化优化

没有真实数据时应坦白“当前尚未完成正式基准”，再说明测量计划。固定地图、敌人数、镜头和构建，录制 Insights 前后 Trace，比较 Game/GPU 帧时间、P95/P99、Tick 数、Draw Call、内存峰值；只在同一条件下得出结论。

### 211. 单机改服务端权威

先把生命、背包、技能冷却、掉落和任务进度的写权限移到服务端；客户端输入变成请求，服务端验证并修改；持久状态用属性复制/Fast Array，瞬时表现用 Cue 或 RPC；本地移动和能力可预测，但服务端校正。逐系统迁移并测试延迟、丢包、重连和并发竞争。

### 212. 1000 个敌人

先按距离和可见性划分更新等级。CPU 侧减少 Actor/Component/Tick、降低 AI 与感知频率、批量空间查询，必要时用 Mass/数据导向模拟；GPU 侧做 LOD/HLOD、实例化、动画预算和遮挡；网络侧使用相关性、Dormancy、Replication Graph 和状态量化。用 Insights/ProfileGPU/Network Insights 找真正瓶颈。

### 213. 大量物品与联机背包

静态物品定义由 AssetManager/DataAsset 或 DataTable 按稳定 ID 管理，大资源使用 Soft 引用；运行时只保存 ItemID、数量和实例属性。服务端权威执行 Add/Use/Move，槽位用 Fast Array 增量复制给 Owner，添加返回实际数量并支持跨多栈、容量预检和事务语义。

### 214. 商店防篡改与超卖

客户端提交商品 ID、数量和唯一请求 ID；服务端读取权威价格与库存，在数据库/服务端事务中验证余额、扣款、减库存和发货，并记录幂等结果。重复请求返回原结果；不能从客户端接收最终价格或“购买成功”状态。

### 215. 全局事件总线问题

事件总线降低直接依赖，但会隐藏调用关系、产生字符串/Tag 协议错误、重复订阅、顺序不确定、悬空回调和难追踪的级联事件。应按作用域拆分通道、使用强类型事件、保存 Handle、明确解绑和线程规则，并为事件加来源与 Trace。

### 216. 多平台 UI

布局使用 Anchor、SizeBox/ScaleBox 和安全区，不按单一像素硬编码；输入层支持鼠标、键盘、手柄和触摸，焦点导航与提示图标动态变化；考虑 DPI Scale、纵横比、文字长度、平台安全区域和性能。用代表性分辨率与输入设备做自动/人工回归。

### 217. 异步加载循环依赖

先把资产依赖图从回调控制流中分离，用统一加载请求表记录 `Unloaded/Loading/Loaded/Failed`，相同请求复用同一 Future/Handle。检测当前依赖栈中的循环，拆出共享资源或改软引用；回调只声明依赖结果，不在层层回调中无条件重新请求父资产。

### 218. 射击散布与验证

客户端根据后坐力状态和确定性种子立即显示准星/弹道；服务端验证射速、弹药、姿态和种子，按相同算法生成锥体散布并做权威命中。命中扫描可结合有限历史回溯，客户端 HitResult 只作提示，伤害和资源消耗由服务端决定。

### 219. 随机地图

用服务端种子驱动确定性生成，先生成房间/节点图并保证起终点连通，再放置几何和装饰；每次放置维护约束，失败时回溯或重试。生成后用 BFS/并查集验证可达、边界、出生安全和导航构建，并保存种子用于复现。

### 220. 周期性卡顿定位

先记录稳定复现的周期、帧时间和玩家操作，再同时采集 Insights、加载和内存事件。周期性问题优先检查 GC、定时保存、资产同步加载、日志/遥测批量 flush、AI Service 和网络批处理；从卡顿帧最长事件反向定位，禁用单一候选验证，修复后比较 P95/P99 与最长帧。
