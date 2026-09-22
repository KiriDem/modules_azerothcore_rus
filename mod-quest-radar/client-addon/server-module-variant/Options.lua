-- QuestRadar (client addon) - Options.lua
--
-- Native options panel (Esc > Interface > AddOns > QuestRadar), in addition
-- to the /qr commands (cf. Core.lua). Both read/write the same
-- QuestRadar.db (SavedVariable QuestRadarDB): no "Okay/Cancel", each
-- checkbox applies immediately, just like the commands.
--
-- Standard Blizzard widgets only (InterfaceOptionsCheckButtonTemplate,
-- InterfaceOptions_AddCategory): no Ace3/AceConfig, to stay free of any
-- third-party Lua dependency (cf. README.md).

local panel = CreateFrame("Frame", "QuestRadarOptionsPanel", UIParent)
panel.name = "QuestRadar"

local title = panel:CreateFontString(nil, "ARTWORK", "GameFontNormalLarge")
title:SetPoint("TOPLEFT", 16, -16)
title:SetText("QuestRadar")

local subtitle = panel:CreateFontString(nil, "ARTWORK", "GameFontHighlightSmall")
subtitle:SetPoint("TOPLEFT", title, "BOTTOMLEFT", 0, -8)
subtitle:SetPoint("RIGHT", panel, "RIGHT", -16, 0)
subtitle:SetJustifyH("LEFT")
subtitle:SetText("Эти настройки также доступны через команды /qr (on|off), /qr zone (on|off) и /qr tracked (on|off).")

-- Gets a Blizzard checkbox's label text regardless of the exact template
-- version (parentKey "Text" normally exposed as check.Text, with a
-- fallback to the classic global naming <name>Text if absent).
local function GetCheckboxLabel(check)
    return check.Text or _G[check:GetName() .. "Text"]
end

local function CreateCheckbox(name, anchor, label, dbKey, tooltip)
    local check = CreateFrame("CheckButton", "QuestRadarOptions" .. name, panel, "InterfaceOptionsCheckButtonTemplate")
    check:SetPoint("TOPLEFT", anchor, "BOTTOMLEFT", 0, -8)

    local labelText = GetCheckboxLabel(check)
    if labelText then
        labelText:SetText(label)
    end
    check.tooltipText = label
    check.tooltipRequirement = tooltip

    check:SetScript("OnClick", function(self)
        QuestRadar.db[dbKey] = self:GetChecked() and true or false
        if QuestRadar.RefreshIcons then
            QuestRadar.RefreshIcons()
        end
    end)
    check:SetScript("OnShow", function(self)
        self:SetChecked(QuestRadar.db[dbKey] and true or false)
    end)

    return check
end

local enabledCheck = CreateCheckbox("Enabled", subtitle, "Включить QuestRadar", "enabled",
    "Включает или отключает значок, область и синхронизацию с сервером.")
local zoneCheck = CreateCheckbox("ShowArea", enabledCheck, "Показать область", "showArea",
    "Отображает полупрозрачный круг вокруг целей, не привязанных к конкретной точке.")
local trackedCheck = CreateCheckbox("OnlyTracked", zoneCheck, "Ограничить отслеживаемыми заданиями", "onlyTracked",
    "Отображаются только задания, отмеченные в списке целей (в противном случае — все задания, принятые на карте).")

InterfaceOptions_AddCategory(panel)
