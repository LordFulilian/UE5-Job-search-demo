# Demo 项目 UnLua 接入说明

## 当前状态

- 插件位置：`Plugins/UnLua`
- 已在 `Demo.uproject` 中启用。
- 使用腾讯 UnLua 官方 `develop` 分支，以适配本项目的 Unreal Engine 5.6。
- 已完成 `DemoEditor Win64 Development` 全量编译。
- 游戏模块不直接依赖 UnLua 模块。Lua 通过 Unreal 反射调用 `UCLASS`、`UPROPERTY` 和 `UFUNCTION`，以后升级或移除脚本插件时不会污染核心玩法代码。

## 已提供的稳定入口

`UDemoLuaLibrary` 是 Lua/蓝图共用的门面，避免脚本到处查找 PlayerController、PlayerState 和组件：

- `GetDemoPlayerController`
- `GetDemoPlayerState`
- `GetDemoPlayerCharacter`
- `GetPartyComponent`
- `GetQuestComponent`
- `GetInventoryComponent`
- `GetExperienceComponent`
- `SwitchPartySlot`
- `TogglePartyPage`
- `ToggleQuestList`

核心角色、PlayerController、PlayerState、队伍、任务、背包、经验和技能数据类型也已设置为可反射类型。现有蓝图接口可以直接被 UnLua 调用，不需要再写一套重复的 Lua 胶水代码。

## Lua 文件位置

UnLua 默认从 `Content/Script` 加载 Lua 模块。本项目提供示例：

```text
Content/Script/Demo/PartyApi.lua
```

Lua 层使用 1～4 号队伍位置；示例内部会转换成 C++ 的 0～3 索引：

```lua
local PartyApi = require("Demo.PartyApi")

PartyApi.SwitchSlot(self, 2)       -- 切到第二名角色
PartyApi.TogglePartyPage(self)     -- 打开/关闭队伍页

local PartyComponent = PartyApi.GetPartyComponent(self)
```

## 给蓝图绑定 Lua

1. 重启编辑器，确认 `编辑(Edit) -> 插件(Plugins)` 中 UnLua 已启用。
2. 打开需要脚本化的蓝图。
3. 使用 UnLua 工具栏的绑定功能，为蓝图选择 Lua 模块。
4. Lua 模块名使用点号路径，例如 `Demo.PlayerCharacter` 对应 `Content/Script/Demo/PlayerCharacter.lua`。
5. 在 Lua 模块中实现蓝图/父类的可覆写函数；普通 C++ `BlueprintCallable`/`BlueprintPure` 接口可直接调用。

建议先只把高频变化的任务流程、UI 流程和数值规则放进 Lua。角色移动、GAS 权威逻辑、复制和底层组件生命周期仍保留在 C++，这样多人同步和性能更稳定。

## 新增接口规范

后续希望 Lua 调用的 C++ 接口遵循以下规则：

- 类：使用 `UCLASS(BlueprintType)` 或适合继承时使用 `Blueprintable`。
- 函数：查询使用 `UFUNCTION(BlueprintPure)`，修改状态使用 `UFUNCTION(BlueprintCallable)`。
- 字段：只读数据使用 `UPROPERTY(BlueprintReadOnly)`，确实需要脚本修改时才使用 `BlueprintReadWrite`。
- 结构和枚举：使用 `USTRUCT(BlueprintType)`、`UENUM(BlueprintType)`。
- 服务器权威操作仍通过 C++ RPC 或门面函数执行，不允许 Lua 直接绕过权限校验。

