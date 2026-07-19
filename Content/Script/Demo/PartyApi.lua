-- Stable Lua-facing helpers for the Demo party/quest UI.
-- Lua callers use party slots 1..4; C++ stores them as 0..3.

local PartyApi = {}

local function player_index_or_default(PlayerIndex)
    return PlayerIndex or 0
end

function PartyApi.GetPlayerController(WorldContextObject, PlayerIndex)
    return UDemoLuaLibrary.GetDemoPlayerController(
        WorldContextObject,
        player_index_or_default(PlayerIndex))
end

function PartyApi.GetPlayerCharacter(WorldContextObject, PlayerIndex)
    return UDemoLuaLibrary.GetDemoPlayerCharacter(
        WorldContextObject,
        player_index_or_default(PlayerIndex))
end

function PartyApi.GetPartyComponent(WorldContextObject, PlayerIndex)
    return UDemoLuaLibrary.GetPartyComponent(
        WorldContextObject,
        player_index_or_default(PlayerIndex))
end

function PartyApi.SwitchSlot(WorldContextObject, LuaSlotIndex, PlayerIndex)
    local SlotIndex = math.max(1, math.min(4, LuaSlotIndex or 1)) - 1
    UDemoLuaLibrary.SwitchPartySlot(
        WorldContextObject,
        SlotIndex,
        player_index_or_default(PlayerIndex))
end

function PartyApi.TogglePartyPage(WorldContextObject, PlayerIndex)
    UDemoLuaLibrary.TogglePartyPage(
        WorldContextObject,
        player_index_or_default(PlayerIndex))
end

function PartyApi.ToggleQuestList(WorldContextObject, PlayerIndex)
    UDemoLuaLibrary.ToggleQuestList(
        WorldContextObject,
        player_index_or_default(PlayerIndex))
end

return PartyApi
