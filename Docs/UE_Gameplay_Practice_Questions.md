# UE 客户端 Gameplay 刷题清单

> 配套能力地图：`UE_Gameplay_Written_Interview_Roadmap.md`。先独立回答或编码，再查资料。`P0` 用于过笔试与通用一面，`P1` 用于游戏客户端技术面，`P2` 用于 UE 项目组和简历深挖。

配套答案入口：[UE Gameplay 刷题答案索引](UE_Gameplay_Practice_Answers_Index.md)。

## C++：语言与对象模型（P0）

- [ ] 1. 静态多态和动态多态分别如何实现？
- [ ] 2. 一次虚函数调用从对象指针到目标函数经历什么？
- [ ] 3. 为什么基类析构函数通常应声明为 virtual？
- [ ] 4. 构造函数为什么不能是虚函数？析构函数为什么可以？
- [ ] 5. 构造与析构阶段调用虚函数会发生什么？
- [ ] 6. overload、override 和 name hiding 有什么区别？
- [ ] 7. pure virtual、abstract class、interface 的关系是什么？
- [ ] 8. 多继承对象可能有几个 vptr？
- [ ] 9. 菱形继承产生什么问题？虚继承如何解决？
- [ ] 10. 空类为什么通常占 1 字节？
- [ ] 11. 计算包含虚函数、继承和不同成员顺序的类大小。
- [ ] 12. 内存对齐为什么存在？`alignof` 与 `sizeof` 如何影响数组？
- [ ] 13. static 成员为什么不计入对象大小？
- [ ] 14. final 和 override 分别提供什么编译期保证？
- [ ] 15. RTTI、typeid 和 dynamic_cast 如何工作？

## C++：内存与生命周期（P0）

- [ ] 16. 进程地址空间通常有哪些区域？
- [ ] 17. 栈与堆在分配、释放、局部性和容量上有什么区别？
- [ ] 18. `new int` 与 `new int()` 有什么区别？
- [ ] 19. new/delete 与 malloc/free 有什么区别和联系？
- [ ] 20. placement new 做什么？何时必须显式调用析构？
- [ ] 21. delete 如何知道数组元素数量或分配大小？
- [ ] 22. 浅拷贝为什么可能造成 double free？
- [ ] 23. Rule of Three、Five、Zero 分别是什么？
- [ ] 24. RAII 如何处理文件、锁和内存？
- [ ] 25. unique_ptr 为什么不能复制？如何转移所有权？
- [ ] 26. shared_ptr 的控制块保存什么？
- [ ] 27. make_shared 与直接 `shared_ptr(new T)` 有何差异？
- [ ] 28. weak_ptr 如何解决循环引用？
- [ ] 29. shared_ptr 是否线程安全？“控制块安全”与“对象安全”有何区别？
- [ ] 30. 左值、右值、xvalue 和 prvalue 如何区分？
- [ ] 31. `std::move` 是否真的移动对象？
- [ ] 32. 移动构造为什么常声明 noexcept？
- [ ] 33. 返回局部对象时 RVO、NRVO 和 move 如何配合？
- [ ] 34. 四种 C++ cast 的使用边界是什么？
- [ ] 35. Lambda 捕获局部引用后异步执行有什么风险？

## C++：const、模板与编译（P0/P1）

- [ ] 36. `const int*`、`int* const`、`const int* const` 分别是什么？
- [ ] 37. 顶层 const 与底层 const 如何影响拷贝和重载？
- [ ] 38. const 成员函数中的 `this` 是什么类型？
- [ ] 39. 只有返回类型不同能否形成函数重载？
- [ ] 40. 参数 `int` 与 `const int` 能否形成重载？
- [ ] 41. 模板声明和定义为什么通常放在头文件？
- [ ] 42. 模板特化与偏特化有什么区别？
- [ ] 43. SFINAE 的“替换失败不是错误”是什么意思？
- [ ] 44. Concept 相比 enable_if 改善了什么？
- [ ] 45. 源文件如何经过预处理、编译、汇编和链接成为程序？
- [ ] 46. 编译错误与链接错误分别在什么阶段产生？
- [ ] 47. ODR 是什么？头文件中定义普通函数为什么可能重复符号？
- [ ] 48. inline、static、匿名命名空间在链接层面有何区别？
- [ ] 49. 宏、constexpr 和 inline function 如何选择？
- [ ] 50. 大端与小端是什么？如何用代码检测？

## STL 与数据结构（P0）

- [ ] 51. vector、list、deque 的内存结构和适用场景是什么？
- [ ] 52. vector 为什么遍历快？
- [ ] 53. vector 扩容会发生什么？标准是否规定增长倍数？
- [ ] 54. reserve 与 resize 有何区别？
- [ ] 55. vector 的 push_back、insert、erase 分别会使哪些迭代器失效？
- [ ] 56. `emplace_back` 是否一定比 `push_back` 快？
- [ ] 57. map 为什么通常使用红黑树？
- [ ] 58. 红黑树保证了什么复杂度？它与 AVL 有何取舍？
- [ ] 59. unordered_map 如何组织桶？负载因子是什么？
- [ ] 60. 哈希冲突有哪些解决方法？
- [ ] 61. unordered_map 在什么情况下退化到 O(n)？
- [ ] 62. priority_queue 如何用堆实现？
- [ ] 63. stack 和 queue 在游戏客户端中有哪些应用？
- [ ] 64. 手写一个支持扩容、拷贝和移动的简化 vector。
- [ ] 65. 手写 LRU Cache，并说明为什么需要哈希表加双向链表。

## 算法：必写题（P0）

- [ ] 66. 二分查找及左右边界变体。
- [ ] 67. 反转链表，分别用迭代和递归实现。
- [ ] 68. 判断链表有环并找到入口，证明快慢指针公式。
- [ ] 69. 合并两个有序链表与 K 个有序链表。
- [ ] 70. 数组中寻找第 K 大，分别用堆和 Quickselect。
- [ ] 71. Top-K 高频元素。
- [ ] 72. 两数之和、三数之和与去重。
- [ ] 73. 最长无重复子串。
- [ ] 74. 最小覆盖子串。
- [ ] 75. 合并区间。
- [ ] 76. 旋转数组查找。
- [ ] 77. 用栈实现队列、用队列实现栈。
- [ ] 78. 有效括号与最小栈。
- [ ] 79. 单调栈解决每日温度或柱状图问题。
- [ ] 80. 二叉树前中后序的递归与迭代写法。
- [ ] 81. 二叉树层序遍历、最大深度和直径。
- [ ] 82. 最近公共祖先。
- [ ] 83. 验证二叉搜索树。
- [ ] 84. 岛屿数量与省份数量。
- [ ] 85. 拓扑排序判断课程依赖。
- [ ] 86. 并查集实现动态连通性。
- [ ] 87. Dijkstra 求非负权最短路。
- [ ] 88. A* 的流程、启发函数和最优条件。
- [ ] 89. 0/1 背包与完全背包。
- [ ] 90. 最长递增子序列。
- [ ] 91. 最长公共子序列。
- [ ] 92. 网格最短路与最少拆除障碍。
- [ ] 93. 最大人工岛。
- [ ] 94. 地下城游戏。
- [ ] 95. Trie 实现包含任意字符的敏感词过滤。

## 操作系统与并发（P0/P1）

- [ ] 96. 进程、线程、协程的资源和调度差异。
- [ ] 97. 用户态和内核态如何切换？
- [ ] 98. 一次线程上下文切换保存什么？
- [ ] 99. 虚拟内存解决什么问题？
- [ ] 100. 缺页中断发生后系统做什么？
- [ ] 101. 常见页面置换算法有哪些？
- [ ] 102. 进程间通信方式有哪些？
- [ ] 103. mutex、spinlock 和 semaphore 如何选择？
- [ ] 104. condition_variable 为什么要与谓词和锁一起使用？
- [ ] 105. 原子操作与锁有什么区别？
- [ ] 106. 死锁的四个必要条件是什么？
- [ ] 107. 如何预防、避免、检测和解除死锁？
- [ ] 108. 线程安全懒汉单例如何实现？
- [ ] 109. false sharing 是什么？为什么影响多线程性能？
- [ ] 110. 游戏主线程、渲染线程和工作线程为什么要减少同步等待？

## 网络与同步（P1）

- [ ] 111. TCP 与 UDP 的核心区别是什么？
- [ ] 112. TCP 为什么需要三次握手而不是两次？
- [ ] 113. 四次挥手与 TIME_WAIT 的作用是什么？
- [ ] 114. TCP 粘包不是“包粘在一起”，它的本质是什么？
- [ ] 115. 如何基于 UDP 实现可靠、有序传输？
- [ ] 116. 停等、滑动窗口和选择重传有什么差异？
- [ ] 117. 状态同步与帧同步如何选择？
- [ ] 118. 什么是服务端权威？
- [ ] 119. 客户端插值、外推、预测和服务器校正分别解决什么？
- [ ] 120. 延迟补偿和回滚需要保存哪些历史状态？
- [ ] 121. 商店购买时为什么不能相信客户端传来的价格？
- [ ] 122. 如何防止一个拾取物被多个客户端重复领取？
- [ ] 123. 序列化、量化和增量同步如何降低带宽？
- [ ] 124. RPC 与状态复制分别适合什么数据？
- [ ] 125. 丢包、延迟、抖动和带宽限制分别如何影响游戏体验？

## 游戏数学与图形学（P1）

- [ ] 126. 点积如何判断前后、夹角和视野范围？
- [ ] 127. 叉积如何判断左右和计算三角形面积？
- [ ] 128. 如何判断点在三角形内部？至少给出两种方法。
- [ ] 129. 如何在圆内均匀随机一个点？为什么半径不能直接均匀取 `[0,r]`？
- [ ] 130. 如何在锥体内生成射击散布方向？
- [ ] 131. 欧拉角为什么会产生万向锁？
- [ ] 132. 四元数如何表示旋转？Slerp 解决什么问题？
- [ ] 133. 局部坐标、世界坐标和观察空间如何转换？
- [ ] 134. 从 CPU 提交到屏幕显示，渲染管线经历哪些阶段？
- [ ] 135. Vertex Shader 和 Fragment/Pixel Shader 的执行次数由什么决定？
- [ ] 136. 前向渲染与延迟渲染有什么差异？
- [ ] 137. 透明物体为什么通常难以使用延迟光照？
- [ ] 138. 深度测试、Early-Z 和透明混合如何交互？
- [ ] 139. MSAA、TAA、FXAA 的基本原理和典型问题是什么？
- [ ] 140. Shadow Map 如何产生阴影？常见伪影有哪些？
- [ ] 141. 骨骼蒙皮在 CPU 或 GPU 上如何计算？
- [ ] 142. Draw Call 为什么昂贵？Batch 与 Instancing 如何减少它？
- [ ] 143. BVH、四叉树、八叉树和均匀网格如何选择？
- [ ] 144. CPU 与 GPU 同步为什么会造成帧卡顿？
- [ ] 145. 如何区分 Game、Render 和 GPU Bound？

## UE 对象系统与 Framework（P2）

- [ ] 146. UE 反射系统解决什么问题？UHT 做了什么？
- [ ] 147. UCLASS、UPROPERTY、UFUNCTION 分别把什么信息暴露给引擎？
- [ ] 148. CDO 是什么？构造函数何时运行？
- [ ] 149. UObject 的 Outer 表达什么关系？它是否等于 GC 强引用？
- [ ] 150. UE GC 如何从 Root Set 标记可达对象？
- [ ] 151. UObject 不参与 GC 时有哪些管理方式？
- [ ] 152. TObjectPtr、TWeakObjectPtr、TSoftObjectPtr、TStrongObjectPtr 如何选择？
- [ ] 153. 为什么不能用 TSharedPtr 管理 UObject 生命周期？
- [ ] 154. CreateDefaultSubobject、NewObject、SpawnActor 有何区别？
- [ ] 155. Actor 从构造到 BeginPlay 的主要生命周期是什么？
- [ ] 156. Destroy 是否立即释放 Actor 内存？
- [ ] 157. Actor 和 ActorComponent 应如何划分职责？
- [ ] 158. Tick、Timer、Delegate、GameplayEvent 如何选择？
- [ ] 159. 动态委托、普通委托、多播委托有什么差异？
- [ ] 160. UE Interface 与 Cast 的适用场景是什么？
- [ ] 161. GameInstance、World 和 Level 的生命周期关系是什么？
- [ ] 162. GameMode、GameState、PlayerController、PlayerState、Pawn 如何分工？
- [ ] 163. 为什么客户端没有 GameMode？
- [ ] 164. PlayerController 为什么只对所属客户端可见？
- [ ] 165. Subsystem 的四类常见生命周期分别是什么？

## UE 资源、网络与性能（P2）

- [ ] 166. 硬引用如何形成资源加载链？
- [ ] 167. DataTable、DataAsset、PrimaryDataAsset 如何选择？
- [ ] 168. AssetManager 如何发现和加载 PrimaryAsset？
- [ ] 169. 异步加载完成前如何管理回调对象生命周期？
- [ ] 170. Level Streaming 与 World Partition 解决什么问题？
- [ ] 171. Authority、Ownership、Relevancy 有什么区别？
- [ ] 172. Replicated 属性、RepNotify 与 RPC 如何选择？
- [ ] 173. Server、Client、NetMulticast RPC 的调用条件是什么？
- [ ] 174. Reliable RPC 为什么不能高频滥用？
- [ ] 175. Dormancy 和 NetUpdateFrequency 如何减少带宽？
- [ ] 176. Fast Array Serializer 为什么适合背包和 Buff 列表？
- [ ] 177. CharacterMovement 如何完成客户端预测与服务器校正？
- [ ] 178. 如何使用 Unreal Insights 定位一次卡顿？
- [ ] 179. `stat unit`、`stat game`、`stat gpu` 分别看什么？
- [ ] 180. GC 卡顿、资源加载卡顿和 Shader/PSO 卡顿如何区分？
- [ ] 181. 大量 Actor、Component、Tick 和 Behavior Tree 如何优化？
- [ ] 182. UMG 为什么可能产生高 Tick、布局和绘制成本？
- [ ] 183. ListView 的虚拟化解决了什么？
- [ ] 184. 对象池什么时候有效，什么时候只会增加复杂度？
- [ ] 185. 优化完成后应记录哪些前后指标？

## Gameplay、AI、动画与 UI（P2，简历相关）

- [ ] 186. ASC 的 OwnerActor 和 AvatarActor 分别是什么？
- [ ] 187. 玩家 ASC 为什么常放 PlayerState？
- [ ] 188. AbilityActorInfo 在服务端和客户端分别何时初始化？
- [ ] 189. GameplayEffect、Modifier、MMC、ExecutionCalculation 如何选择？
- [ ] 190. Instant、Duration、Infinite GE 的生命周期是什么？
- [ ] 191. Attribute 为什么使用 RepNotify？Meta Attribute 有什么用途？
- [ ] 192. GameplayTag 如何表达输入、状态、能力和伤害类型？
- [ ] 193. AbilityTask 如何等待 Montage、输入和 GameplayEvent？
- [ ] 194. Local Predicted Ability 如何避免重复结算？
- [ ] 195. Full、Mixed、Minimal GAS 复制模式有什么区别？
- [ ] 196. Behavior Tree 的 Task、Service、Decorator 各负责什么？
- [ ] 197. Blackboard 中哪些数据适合由 Service 更新？
- [ ] 198. EQS 与 NavMesh 如何配合？如何限制候选点在同一楼层？
- [ ] 199. MoveTo 失败时如何系统排查？
- [ ] 200. AnimBP、状态机、BlendSpace、Montage 的职责是什么？
- [ ] 201. Montage Section、Slot、Notify 和 NotifyState 如何配合连招？
- [ ] 202. Root Motion 与 In-Place 动画如何选择？
- [ ] 203. Motion Warping 解决什么问题？
- [ ] 204. Enhanced Input 的 Context、Action、Trigger、Modifier 分别是什么？
- [ ] 205. Widget 为什么不应直接绑定所有 Gameplay 对象？

## 项目与场景题（P2）

- [ ] 206. 用一分钟说明项目目标、你的职责、核心系统和量化结果。
- [ ] 207. 从一次输入到伤害结算，讲完整调用链。
- [ ] 208. 为什么选择组件、继承、接口或 Subsystem？
- [ ] 209. 项目中最难定位的问题是什么？证据链是什么？
- [ ] 210. 哪项优化有优化前后的指标？如何复现？
- [ ] 211. 如果将单机系统改为服务端权威，数据所有权如何变化？
- [ ] 212. 1000 个敌人同时存在时，CPU、GPU、网络分别怎么处理？
- [ ] 213. 20 格背包如何扩展到大量物品和联机同步？
- [ ] 214. 商店购买流程如何防篡改、重复请求和超卖？
- [ ] 215. 全局事件总线可能造成哪些生命周期和调试问题？
- [ ] 216. UI 同时适配 PC、移动端和多分辨率要考虑什么？
- [ ] 217. 异步加载 A，A 又依赖 B，B 回调中再请求 A，如何处理？
- [ ] 218. 如何设计射击散布、后坐力和服务端命中验证？
- [ ] 219. 如何设计随机地图并保证可达性与可复现？
- [ ] 220. 游戏突然每隔几十秒卡一帧，你如何定位？

## 模拟笔试组合

### 套卷 A：基础过筛

选择题范围为对象模型、const、智能指针、vector 失效、线程死锁和 TCP/UDP。编程题依次完成链表环入口、Top-K 和网格 BFS。总时长 90 分钟，目标是前两题完整通过，第三题至少写出正确思路和复杂度。

### 套卷 B：游戏客户端

选择题范围为 C++ 内存、哈希、操作系统、四元数和渲染管线。编程题依次完成 LRU、0/1 背包和 A* 网格寻路。问答题说明状态同步与帧同步、对象池边界以及一次 Draw Call 优化的验证方式。

### 套卷 C：UE 专项

问答题覆盖 UObject GC、Actor 生命周期、Gameplay Framework、RPC、软引用和 Insights。编码题实现可复制背包条目的数据结构设计伪代码，并说明服务端验证与 Fast Array 更新。项目题要求从 Enhanced Input 讲到 GAS 伤害、死亡和 UI 更新。
