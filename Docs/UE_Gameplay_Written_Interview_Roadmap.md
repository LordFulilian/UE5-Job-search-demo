# UE 客户端 Gameplay 笔试与面试能力地图

> 调研时间：2026-07-16。目标岗位：国内游戏客户端、UE 客户端、Gameplay 开发的校招与实习岗位。

## 先确定考试结构

公开面经反复呈现出三层筛选。在线笔试首先考 C++、数据结构与算法，部分公司混合操作系统、网络、图形数学和选择题；这一步经常与 UE 无关。通用技术一面继续考 C++ 八股和一道手写算法，同时开始询问项目。进入具体 UE 项目组后，UObject、反射、GC、Gameplay Framework、网络复制、性能和简历中的 GAS、AI、动画等内容才明显增多。

因此，准备顺序不能从 GAS 开始。正确顺序是：先保证 C++ 和算法能过笔试，再补 408 与游戏数学，之后系统学习 UE，最后把项目和场景题练成口述答案。只会做 Demo 但写不出链表、堆、DFS 或中等动态规划，很可能拿不到面试机会；只背算法但解释不了 UObject、Actor 生命周期和项目设计，也很难通过组内面试。

| 层级 | 主要用途 | 应掌握内容 | 验收方式 |
| --- | --- | --- | --- |
| S | 过笔试和通用一面 | C++、数据结构、算法 | 90 分钟稳定完成 2 道中等题；常见 C++ 题能口述原理 |
| A | 过游戏客户端技术面 | 操作系统、网络、游戏数学、基础图形学 | 每个主题能回答概念、底层原因和游戏应用 |
| A | 过 UE 项目组面试 | UObject、GC、反射、Framework、资源、网络、性能 | 能画生命周期和网络时序，能解释源码调用关系 |
| B | 区分 Gameplay 方向 | GAS、AI、动画、UI、输入、数据驱动 | 结合自己做过的系统回答设计取舍和扩展方案 |
| B | 主管面与场景题 | 项目复盘、性能分析、协作与需求拆解 | 对陌生场景给出假设、方案、复杂度、风险和验证指标 |

## S 级：C++ 必须形成体系

近期面经仍把多态、智能指针和析构函数列为近乎每场都会触及的基础，随后向内存、容器、模板和并发扩展。笔试可能直接给代码判断输出、找未定义行为、计算对象大小，也可能要求实现字符串函数、单例或简化容器。

### 语言与对象模型

需要掌握静态多态与动态多态，虚函数调用过程，vptr、vtable 的典型实现，虚析构的作用，构造与析构顺序，纯虚函数与抽象类，override、final，函数重载与重写，多继承、菱形继承和虚继承。对象大小题要考虑空类占位、成员对齐、尾部填充、继承、虚表指针和静态成员不计入实例。

学习深度不是背“虚表是一个函数指针数组”，而是能解释构造阶段虚调用为何不会分派到尚未构造的派生部分，基类指针删除派生对象为何需要虚析构，虚继承为解决共享基类二义性付出了什么布局代价。

### 内存、生命周期与类型系统

需要掌握栈、堆、静态存储区、只读数据和代码段，new/delete 与 malloc/free，placement new，RAII，浅拷贝与深拷贝，Rule of Three/Five/Zero，左值、右值、引用折叠和移动语义。智能指针要能说明 unique_ptr、shared_ptr、weak_ptr 的所有权，控制块，循环引用，make_shared 的一次分配特点和 weak_ptr 的 lock。

const 需要覆盖顶层与底层 const、常量指针与指针常量、const 成员函数和 mutable。类型转换需要覆盖 static_cast、dynamic_cast、const_cast、reinterpret_cast 的用途和风险。Lambda 需要掌握值捕获、引用捕获、`this` 捕获、闭包对象与异步生命周期。

### STL、缓存与复杂度

需要掌握 vector、list、deque、array 的存储特征和迭代器失效，vector 的 size/capacity、reserve/resize 和扩容成本；map/set 的平衡树性质；unordered_map 的桶、负载因子、rehash 和冲突处理；priority_queue 的堆结构。需要能从缓存局部性解释为什么连续数组遍历通常快于链表，而不是只比较渐进复杂度。

高频手写包括去重 vector、合并有序数组、实现简化 vector、LRU、Top-K、堆排序、哈希函数与冲突解决。容器选择题要说明访问模式、内存占用、顺序要求、稳定性和插入删除位置。

### 模板、编译链接与现代 C++

需要掌握模板实例化为何影响声明与定义的放置，特化与偏特化，SFINAE 的基本含义，C++20 Concept 解决的问题。编译链路需要说清预处理、编译、汇编、链接，符号与重定位，静态库和动态库，头文件保护、ODR、inline 和 static 的不同作用域语义。

现代 C++ 至少掌握 auto、decltype、范围 for、nullptr、移动语义、智能指针、Lambda、constexpr、结构化绑定、optional、variant、string_view 和并发库基础。POD、trivial、standard-layout 不必作为第一批背诵，但在冲刺米哈游、引擎工具或偏底层岗位时需要补齐。

## S 级：算法决定能否拿到面试

公开记录显示，面试手写大多落在 LeetCode 中等题附近，笔试更可能出现多道编程题、动态规划或图论难题。旧面经中出现过 LeetCode 547、827、174、14、232、113；近期记录仍反复提到中等原题、原创中等题、Top-K、链表环入口和场景算法。

### 第一阶段：必须无提示写出

数组与字符串要覆盖双指针、滑动窗口、前缀和、差分、排序、二分、哈希计数和区间合并。链表要覆盖反转、合并、快慢指针、环与入口、倒数节点和 LRU。栈队列要覆盖括号、单调栈、最小栈、用栈实现队列和 BFS。

树要覆盖递归与迭代遍历、层序、深度、高度、路径、公共祖先、BST 验证和序列化。堆要覆盖第 K 大、Top-K、合并 K 个有序序列和数据流中位数。每道题必须在写代码前说明时间复杂度、空间复杂度和边界。

### 第二阶段：笔试主战区

图论需要掌握 DFS、BFS、并查集、拓扑排序、Dijkstra、网格搜索和连通分量。动态规划需要掌握一维 DP、二维 DP、0/1 背包、完全背包、最长子序列、路径计数和状态压缩的基本识别方法。回溯需要掌握排列、组合、子集、棋盘和剪枝。

游戏相关算法要理解 A* 的 `f=g+h`、启发函数的可采纳性和一致性，网格寻路、Flood Fill、AOE 范围、空间划分和 Trie 敏感词过滤。A* 不等于每场笔试必考，但它经常作为游戏岗位口述题和简历追问。

### 第三阶段：冲刺题

笔试中可能出现困难动态规划、复杂网格、字符串匹配或原创构造题。冲刺阶段应练习最大人工岛、地下城游戏、困难背包变体、KMP、Trie、区间 DP 和常见数学题，但不能用它们替代中等题基本功。稳定完成简单与中等题的收益高于零散刷困难题。

## A 级：操作系统与并发

操作系统在近期 UE 求职总结中仍被列为仅次于 C++ 的通用基础。需要掌握进程、线程和协程的资源与调度差异；用户态与内核态；上下文切换；虚拟内存、分页、缺页和页替换；堆与栈的增长；进程间通信；线程同步和死锁。

并发题需要说明 mutex、spinlock、读写锁、condition_variable、semaphore 和 atomic 的使用条件。死锁要能写出四个必要条件并给出固定加锁顺序、一次性获取、超时和无锁设计等处理方式。内存序只需先掌握 data race、原子性、可见性和 happens-before；偏底层岗位再深入 acquire/release。

验收题包括：自旋锁和互斥锁如何选择；条件变量为什么必须配合谓词循环等待；单例如何保证线程安全；游戏主线程为什么不能等待耗时 IO；任务图和线程池解决了什么问题。

## A 级：网络与多人游戏基础

TCP/UDP 是公开面经中最稳定的网络题。需要掌握连接、可靠性、顺序、拥塞控制和消息边界差异，三次握手、四次挥手、TIME_WAIT、粘包拆包，以及在 UDP 上实现序号、ACK、重传、滑动窗口、乱序重排和拥塞策略的基本思路。

游戏网络需要掌握状态同步与帧同步的差异，服务端权威，输入同步，快照，插值、外推、客户端预测、服务器校正、延迟补偿和回滚。还要理解 RPC、序列化、量化、带宽、丢包、抖动和 Tick Rate。这些概念先学通用模型，再映射到 UE Replication、CharacterMovement 和 GAS Prediction。

典型场景题包括：购买商品时价格能否由客户端上传；拾取物如何防重复领取；高延迟下如何保证移动手感；射击命中如何防作弊；背包数组如何增量同步。

## A 级：游戏数学与基础图形学

游戏客户端岗位的图形学考察具有公司和项目差异，但渲染管线、向量运算和四元数经常出现。即使目标是 Gameplay，也不能完全跳过。

数学部分需要掌握向量长度与归一化、点积的夹角和前后判断、叉积的方向与面积、坐标空间变换、矩阵组合顺序、欧拉角万向锁、四元数旋转与 Slerp。几何题需要会判断点在三角形内、射线与平面/球/AABB 相交、圆或球内均匀随机、锥体内随机方向和基础碰撞检测。

图形部分需要能按阶段讲清 CPU 提交、顶点处理、裁剪、光栅化、片元/像素处理、深度模板、混合和呈现。随后掌握 Draw Call、Batch、GPU Instancing、前向与延迟渲染、透明混合、Early-Z、阴影贴图、抗锯齿、骨骼蒙皮、LOD、遮挡剔除、BVH/八叉树以及 CPU/GPU 同步。Gameplay 岗先掌握原理与性能影响，不要求推导 PBR 方程。

## A 级：UE 对象系统与 Gameplay Framework

UE 专岗的底座是 UObject，而不是 GAS。需要掌握 UHT 生成反射代码的作用，UCLASS、UPROPERTY、UFUNCTION，CDO，Outer，Class Default Object 与实例默认值，UObject 的创建和生命周期，GC 的 Root Set 与引用可达性，强引用、Weak、Soft 和 Strong Object Ptr。

Actor 体系需要掌握 `CreateDefaultSubobject`、`NewObject`、`SpawnActor`，Actor 与 Component 生命周期，注册、初始化、BeginPlay、Tick、EndPlay、Destroy 和延迟 GC。Delegate、Interface、Subsystem、Timer、TickGroup 和异步任务需要理解用途与生命周期风险。

Gameplay Framework 要能画出 GameInstance、World、GameMode、GameState、PlayerController、PlayerState、Pawn、Character、HUD 和 LocalPlayer 的存在位置、生命周期、网络可见性和职责。面试常从“为什么属性放 PlayerState”“客户端为什么没有 GameMode”“Controller 和 Pawn 如何分离”继续追问。

## A 级：UE 资源、网络与性能

资源系统需要掌握硬引用与软引用、DataTable 与 DataAsset、PrimaryAsset、AssetManager、异步加载、StreamableHandle、资源卸载、包依赖、关卡流送和 World Partition。面试不一定要求背 API，但必须能解释硬引用链为什么扩大启动加载，以及如何测量和拆分。

UE 网络需要掌握 Authority、Ownership、Role 的现代表达、Relevancy、NetUpdateFrequency、Dormancy、属性复制、RepNotify、Server/Client/Multicast RPC、条件复制、Fast Array、Actor Channel 和基本网络分析工具。进一步学习 CharacterMovement 的客户端预测与校正，以及 GameplayAbility 的预测键和 TargetData。

性能需要掌握 `stat unit`、`stat game`、`stat gpu`、Unreal Insights、MemReport 和 ProfileGPU 的用途。优化回答必须遵循“复现与指标、定位线程或阶段、缩小事件、修改、对比验证”，而不是只报对象池和关闭 Tick。常见对象包括 Tick、Actor/Component 数量、GC、异步加载、Draw Call、材质、骨骼、Behavior Tree Service、UMG Tick 和 ListView。

## B 级：Gameplay 系统按简历深挖

GAS 需要掌握 ASC 的 Owner/Avatar、初始化时机、AttributeSet、GameplayAbility、GameplayEffect、GameplayTag、GameplayCue、AbilityTask、Modifier、MMC、ExecutionCalculation、SetByCaller、TargetData、复制模式、预测与取消。只有简历写了 GAS 才需要深入到自定义 EffectContext、Aggregator 和 PredictionKey，但写了就必须能讲完整调用链。

AI 需要掌握 AIController、Behavior Tree、Blackboard、Task、Service、Decorator、AIPerception、EQS、NavMesh、MoveTo 失败原因和寻路性能。动画需要掌握 AnimInstance、状态机、BlendSpace、Montage、Section、Slot、Notify/NotifyState、Root Motion、Motion Warping、分层混合和动画线程。UI 需要掌握 UMG/Slate 关系、事件驱动刷新、Widget 生命周期、ListView 虚拟化、输入模式、焦点和常见性能问题。输入需要掌握 Enhanced Input 的 Mapping Context、Action、Trigger、Modifier 和动态切换。

这些模块的考察深度与简历强相关。简历写“实现了联机 GAS 战斗”，面试官自然会追问服务端权威、预测、重复初始化和 Ability 取消；简历只写“使用 GAS”，仍答不出 ASC 初始化，会被视为教程复现。

## B 级：项目与场景题

近期 UE 求职总结显示，项目和场景题在许多面试中占比高于引擎八股。每个项目模块必须准备六个答案：需求是什么，为什么这样拆，完整调用链，遇到的失败与定位过程，如何测量结果，如果支持联机、大规模或热更新要怎样改。

场景题回答采用固定顺序：先确认人数、平台、延迟、数据规模和一致性要求；再确定权威方和数据所有权；给出最简单可行方案；分析时间、空间、带宽和加载成本；指出故障与作弊边界；最后说明用什么指标验证。没有澄清约束就直接报技术名词，通常不是合格的工程回答。

高频场景包括大量敌人、超大世界、背包购买与防作弊、对象池、全局事件系统、UI 多分辨率、异步资源循环依赖、AI 寻路与楼层过滤、射击散布、随机地图、无缝演出切换和性能卡顿排查。

## 不同阶段的时间分配

### 距离笔试一个月以上

每天至少一半时间放在 C++ 和算法。建议算法 90 分钟、C++ 60 分钟、408 或数学 45 分钟、UE 60 分钟、项目复盘 30 分钟。每周做两次 90 分钟模拟笔试，不查资料，结束后重写未通过题。

### 距离笔试两周

停止漫无目的扩展 UE 功能。算法集中复习数组、链表、树、堆、图和基础 DP；C++ 集中复习对象模型、内存、智能指针、容器和并发；每天做一套选择题和一道中等编程题。UE 只保持 GC、反射、Framework 和网络主线。

### 已经拿到面试

算法保持每天一道，剩余时间转向职位描述、简历逐行追问、项目调用链、UE 专项和公司面经。至少完成两次 45 分钟模拟面试，并录音检查答案是否先给结论、是否超过两分钟、是否只说名词不讲因果。

## 30 天执行计划

| 天数 | 主线 | 当天验收 |
| --- | --- | --- |
| 1-3 | C++ 多态、继承、对象大小 | 手画 vtable，完成 30 道判断题 |
| 4-6 | 内存、RAII、智能指针、移动 | 手写资源类和线程安全单例 |
| 7-9 | STL、哈希、红黑树、堆 | 手写 LRU、Top-K、vector 去重 |
| 10-12 | 数组、字符串、链表 | 每天 3 题，限时完成 |
| 13-15 | 树、堆、DFS/BFS | 完成遍历、LCA、连通分量 |
| 16-18 | DP、背包、图论、A* | 完成 0/1 背包和网格寻路 |
| 19-20 | 线程、锁、死锁、虚拟内存 | 口述 20 道 OS 题 |
| 21-22 | TCP/UDP、可靠 UDP、同步模型 | 画出可靠 UDP 和状态同步流程 |
| 23-24 | 向量、四元数、渲染管线 | 完成前后判断和渲染管线口述 |
| 25-26 | UObject、反射、GC、生命周期 | 画 UObject 与 Actor 生命周期 |
| 27 | Gameplay Framework | 画服务端与客户端对象分布 |
| 28 | UE 网络、资源与性能 | 写一份背包联机和加载优化方案 |
| 29 | GAS、AI、动画、UI | 按简历逐条回答完整调用链 |
| 30 | 全真模拟 | 90 分钟笔试 + 45 分钟技术面 |

## 刷题合格线

算法不是以题数验收。简单题应在 10 到 15 分钟写完，中等题在 25 到 35 分钟形成可运行解法；连续三套模拟笔试都能完成至少两道题，才算具备过筛稳定性。错题必须记录错误类型：没识别模型、数据结构选错、边界遗漏、复杂度不合格或编码失误。

C++ 不是以背定义验收。每个问题应能先用 30 秒给结论，再用两分钟解释内存、生命周期或编译机制，并能读代码判断输出。UE 不是以记 API 验收，应能从对象职责、生命周期、网络位置、数据所有权和性能代价解释设计。

## 调研来源与使用限制

本总结参考了公开牛客帖子中的题目与求职复盘。近期来源包括 [27 暑期 UE 游开总结及面经分享](https://www.nowcoder.com/feed/main/detail/d67c1a39343742dcbbe014c0bebe63b0)、[27 届 UE 游戏客户端开发暑期实习总结](https://www.nowcoder.com/discuss/887447858369036288) 和 [游戏客户端面经及经历分享](https://www.nowcoder.com/discuss/860341579272212480)。这些帖子共同指出 C++、算法和操作系统在通用筛选中的作用，并显示 UE 内容在具体项目组与简历追问中加深。

历年对照来源包括 [2022 年（2023 届）游戏客户端开发秋招总结贴](https://www.nowcoder.com/discuss/402937866072993792)、[库洛 UE4 客户端开发一面](https://www.nowcoder.com/discuss/353159605586960384)、[游戏客户端秋招面经总结](https://www.nowcoder.com/discuss/702122998706262016) 和 [数字天空 UE 客户端实习笔面经](https://www.nowcoder.com/discuss/722526137238237184)。旧题用于识别长期稳定主题，不代表 2026 年某公司的固定题库。

面经是候选人的回忆样本，会受岗位、项目组、简历和个人表达影响。本手册因此只使用“必修、常见、简历相关、冲刺”分级，不把收藏数或少量帖子转换成虚假的精确频率。
