# 刷题答案第三卷：UE 核心

## UObject、反射与生命周期

### 146. 反射与 UHT

UE 的反射元数据支撑 GC、序列化、属性编辑、蓝图、网络复制和运行时类型查询。UHT 在 C++ 编译前扫描受支持的宏和声明，生成注册、参数封送与元数据代码；它不是一个完整 C++ 编译器，宏后的声明必须符合 UHT 规则。

### 147. UCLASS、UPROPERTY、UFUNCTION

UCLASS 注册 UObject 类型和类元数据；UPROPERTY 注册字段，使引擎能按 Specifier 处理 GC 引用、编辑器、序列化、复制和蓝图；UFUNCTION 注册函数，支持反射调用、蓝图事件、RPC 和委托绑定。宏本身只是 UHT 标记，实际能力来自生成代码和运行时类型系统。

### 148. CDO

每个 UClass 有一个 Class Default Object，保存该类的默认属性并作为实例初始化模板。构造函数会在创建 CDO 时运行，也会在实例构造路径中运行，因此构造函数应创建默认子对象和设置默认值，不能假设 World、Controller 或运行期数据已就绪。

### 149. Outer

Outer 建立对象的命名路径、包归属和上下文层级，并影响某些销毁和序列化行为。它不等于通用的“Outer 强持有 Inner”，不能用 Outer 代替反射可见的强引用；需要存活保证时仍应使用 UPROPERTY/TObjectPtr、Root 或其他受支持引用机制。

### 150. UE GC

GC 从 Root Set 和引擎显式引用入口出发，沿反射可见的 UObject 引用、`AddReferencedObjects` 等路径做可达性标记，再清理不可达对象。普通未注册裸指针不会成为 GC 边，因此对象回收后会悬空。

### 151. 保持 UObject 存活

常规做法是让一个存活 UObject 通过 UPROPERTY/TObjectPtr 强引用它。临时跨作用域可使用 TStrongObjectPtr；非 UObject 所有者可实现 FGCObject 报告引用；极少数全局对象可 AddToRoot，并必须在合适时 RemoveFromRoot。不要为了省事把大量对象永久 Root。

### 152. UE 对象指针选择

TObjectPtr 用于 UObject 成员强引用并配合 UPROPERTY；TWeakObjectPtr 不阻止 GC，对象销毁后可检测失效；TSoftObjectPtr 保存软路径，可在未加载时引用资产并支持异步加载；TStrongObjectPtr 在自身生命周期内把对象作为强 GC 引用，适合非 UPROPERTY 的临时所有权。

### 153. TSharedPtr 与 UObject

TSharedPtr 的引用计数系统和 UObject GC 相互独立，引用计数大于零不能让 GC 看到对象，GC 回收后 SharedPtr 仍可能保存地址。TSharedPtr 用于纯 C++/Slate 类型；UObject 使用 UE 的反射引用、Weak、Soft 或 Strong Object Ptr。

### 154. 三种创建方式

CreateDefaultSubobject 在构造阶段创建默认组件/子对象，参与 CDO 和蓝图默认值体系；NewObject 创建普通 UObject，需要 Outer、Class 和强引用管理；SpawnActor 通过 UWorld 创建 Actor，进入 Spawn、组件初始化、网络和 BeginPlay 流程，并可配置 Owner、Instigator 和碰撞处理。

### 155. Actor 生命周期

稳定主线是构造与默认子对象、组件注册、`PostInitializeComponents`、`BeginPlay`、运行期 Tick、`EndPlay`，随后等待销毁和 GC。编辑器放置、运行时 Spawn、网络客户端复制和延迟 Spawn 的详细回调不同；运行期依赖应放在合适初始化回调，而不是构造函数。

### 156. Destroy 与释放

`Destroy()` 标记 Actor 结束生命周期，触发 EndPlay 并从 World 中移除相关行为，但 C++ 内存不会在调用点立即释放。对象仍由 GC 在之后回收；外部观察者应使用 IsValid、Weak 引用和 EndPlay 通知，而不是调用后继续假定可用。

### 157. Actor 与 Component

Actor 是 World 中具有身份、变换、网络通道和生命周期的实体；Component 为 Actor 提供可组合的渲染、碰撞或逻辑能力。独立生成、相关性和网络身份放 Actor；依附拥有者、可复用且不需独立存在的功能放 Component。

### 158. Tick、Timer、Delegate 与 GameplayEvent

每帧连续更新使用 Tick，并只在需要时启用；低频或延迟执行使用 Timer；对象间数据变化通知使用 Delegate；跨 Gameplay 系统传递带语义的瞬时消息可使用 GameplayEvent。选择时同时考虑频率、取消、所有权、网络与生命周期。

### 159. UE Delegate 类型

普通单播委托保存一个回调，普通多播保存多个回调，性能和类型自由度较好但不能直接序列化给蓝图。动态委托通过反射绑定 UFUNCTION，可暴露蓝图和序列化，但开销更高、签名限制更多。绑定后要防重复并在生命周期结束时解绑或使用弱绑定。

### 160. Interface 与 Cast

Cast 适合调用者确实依赖具体类型的数据；Interface 适合只依赖“可交互、可受伤”等能力，并允许多个无共同实现基类的对象提供行为。BlueprintNativeEvent 接口应经 `IInterface::Execute_Function(Object, ...)` 调用，以兼容蓝图覆盖。

### 161. GameInstance、World 与 Level

GameInstance 通常从游戏启动持续到退出，可跨地图保留非复制的全局服务。一次地图/Travel 对应一个或多个 World 生命周期，World 包含 Persistent Level 和流送 Level；Travel 时旧 World/Level 销毁，GameInstance 仍可存在。

### 162. Gameplay Framework 分工

GameMode 只在服务端定义规则与出生；GameState 把全局比赛状态复制给客户端；PlayerController 表示玩家连接与控制，存在于服务端和所属客户端；PlayerState 保存所有客户端可见的玩家长期状态；Pawn/Character 是可被控制和替换的场景 Avatar。

### 163. 客户端没有 GameMode

GameMode 包含权威规则和安全敏感逻辑，只在服务端生成。客户端通过复制的 GameState 观察需要公开的比赛状态，不能直接依赖或修改 GameMode。

### 164. PlayerController 可见范围

服务端拥有所有 PlayerController，每个客户端通常只拥有自己的 PlayerController，不会复制其他玩家的 Controller。其他玩家公开信息应放 PlayerState，场景表现放 Pawn；这也决定 Client RPC 和 Ownership 链的目标。

### 165. Subsystem 生命周期

UEngineSubsystem 随 Engine，UGameInstanceSubsystem 随 GameInstance，UWorldSubsystem 随 World，ULocalPlayerSubsystem 随本地玩家。选择依据是服务对象的自然生命周期和访问范围，不应把所有全局逻辑都塞进 GameInstanceSubsystem。

## 资源、网络与性能

### 166. 硬引用加载链

资产 A 的硬引用要求其依赖 B 在 A 加载时可用，B 的硬引用继续展开，最终形成依赖链并扩大内存和加载时间。可用 Reference Viewer/Size Map 检查；对可选大资源改用 Soft 引用和显式异步加载。

### 167. DataTable、DataAsset 与 PrimaryDataAsset

DataTable 适合大量同结构行和表格导入；DataAsset 适合每项独立资产、结构复杂且需要编辑器引用；PrimaryDataAsset 具有 PrimaryAssetId，可由 AssetManager 扫描、分组和按 Bundle 加载。选择依据是编辑方式、独立身份和加载策略。

### 168. AssetManager

项目在设置或代码中定义 PrimaryAssetType 和扫描路径，资产提供稳定 PrimaryAssetId。AssetManager 根据注册信息查询资产数据，并通过 StreamableManager 异步加载指定 PrimaryAsset 或 Bundle；调用者持有加载句柄和使用期引用。

### 169. 异步回调生命周期

请求者可能在加载完成前销毁，所以回调不能无条件捕获裸 this。使用弱 UObject/Lambda、保存并取消 StreamableHandle，回调中再次检查对象与 World，有效后再安装资源；资源使用期间还需保留强引用或句柄。

### 170. Level Streaming 与 World Partition

Level Streaming 由开发者组织子关卡并按逻辑加载卸载。World Partition 把大世界划为网格 Cell，结合 Streaming Source 自动管理相关区域，并支持 Data Layer、HLOD 等工作流；它减少手工子关卡管理，但仍需控制依赖和内存预算。

### 171. Authority、Ownership、Relevancy

Authority 表示谁对 Actor 状态有最终决定权，常规模型是服务端；Ownership 表示 Actor 属于哪个连接，影响 Server RPC 资格、Client RPC 和条件复制；Relevancy 决定某连接当前是否需要接收该 Actor。三者分别回答“谁决定、属于谁、谁看得到”。

### 172. 属性复制、RepNotify 与 RPC

需要长期保持且晚加入者也应看到的数据使用属性复制；客户端在值更新时需要响应使用 RepNotify；一次性请求或通知使用 RPC。状态应由复制变量表达，RPC 不应成为唯一真相来源。

### 173. 三类 RPC 条件

Server RPC 从拥有该 Actor/Component 的客户端发向服务端，服务端验证请求；Client RPC 由服务端发给 Actor 的 owning connection；NetMulticast 必须由服务端调用，发送给当前相关客户端。Actor 必须支持复制，调用端与 Ownership 也必须满足规则。

### 174. Reliable RPC 风险

Reliable 保证在连接存续期间有序送达，但丢包会阻塞后续同通道可靠消息，发送过快还会挤满可靠缓冲并断开连接。高频移动、瞄准和可丢视觉事件应使用复制、压缩输入或不可靠消息。

### 175. Dormancy 与 NetUpdateFrequency

NetUpdateFrequency 控制 Actor 被考虑发送更新的频率；Dormancy 让长期不变 Actor 暂停常规属性比较与发送，状态改变前需唤醒/Flush。它们减少 CPU 与带宽，但更新延迟和错误唤醒会造成状态不及时。

### 176. Fast Array Serializer

Fast Array 为数组条目维护复制 ID/Key，只发送新增、修改和删除，并提供客户端回调，适合背包、Buff 和任务列表。静态定义使用稳定 ID，数组条目只复制实例状态；所有服务端修改必须正确 MarkItemDirty/MarkArrayDirty。

### 177. CharacterMovement 预测

Autonomous Proxy 本地立即模拟并保存输入 Move，再把压缩 Move 发给服务端；服务端权威模拟并确认或返回位置校正。客户端收到校正后恢复服务端状态，重放尚未确认的 Move；其他客户端作为 Simulated Proxy 对复制快照平滑插值。

### 178. Insights 定位卡顿

先在可复现场景录制 CPU/GPU/加载等 Trace，找到超预算帧，再比较 Game、Render、RHI 和任务线程。展开最长事件和调用关系，形成假设后只改一个因素，重新录制并比较帧时间分位数，而不是只截取最好一帧。

### 179. stat 命令

`stat unit` 快速比较 Game、Draw、GPU 与 Frame；`stat game` 展开游戏线程各 Tick/系统组耗时；`stat gpu` 展示 GPU 各 Pass 粗略耗时。定位复杂问题仍应进入 Insights 或 GPU Profiler。

### 180. 三类卡顿区分

GC 卡顿通常伴随 CollectGarbage 和对象扫描/销毁事件；加载卡顿伴随同步 Load、包 IO、序列化和资源创建；Shader/PSO 卡顿表现为编译、管线创建或驱动等待。用 Trace、日志和复现条件验证，不能仅凭周期猜测。

### 181. 大量 Actor/AI 优化

先用 Insights 识别 Tick、组件更新、导航、动画、网络还是渲染占用。随后采用距离/重要度更新、关闭无用 Tick、降低 Service 频率、共享数据、批量查询、Mass/数据导向方案、网络相关性和 LOD；对象池只解决创建销毁确实昂贵的部分。

### 182. UMG 成本

频繁 Tick/Binding、布局失效、Prepass、复杂层级、透明重叠和大量独立绘制都会增加 CPU/GPU 成本。使用事件驱动更新、Invalidation、Retainer 的适当场景、减少层级和材质切换，并用 Widget Reflector/Insights 测量。

### 183. ListView 虚拟化

ListView 只为可见及少量缓冲条目创建 Entry Widget，滚动时复用它们，而不是为全部数据创建 Widget。数据对象与 Entry Widget 生命周期不同，Entry 重新分配时必须完整刷新并解除旧绑定。

### 184. 对象池边界

高频创建销毁且初始化成本显著、对象类型有限时，对象池可降低分配和构造成本。对象稀少、状态重置复杂、池长期占内存或引擎已有高效生命周期时，池可能更慢、更难维护；必须用创建峰值和内存指标证明收益。

### 185. 优化验证指标

记录同一硬件、构建配置、场景和操作下的平均值、P95/P99 帧时间、最长帧、线程/GPU 分项、内存峰值、分配次数、加载时间和网络带宽。保留前后 Trace 与可复现步骤，避免用不同场景的 FPS 作结论。
