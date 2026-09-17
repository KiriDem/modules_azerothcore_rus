local CUSTOM_ICON_PATH = "Interface\\AddOns\\CleanupBags\\Textures\\broom.blp"
local FALLBACK_ICON = "Interface\\Icons\\INV_Misc_Bag_08"

local function ApplyIconToButton(btn)
    if not btn.icon then
        btn.icon = btn:CreateTexture(nil, "ARTWORK")
        btn.icon:SetAllPoints(btn)
    end

    btn.icon:SetTexture(CUSTOM_ICON_PATH)
    if not btn.icon:GetTexture() or btn.icon:GetTexture() == "" then
        btn.icon:SetTexture(FALLBACK_ICON)
    end

    btn.icon:SetDrawLayer("ARTWORK")
    btn.icon:SetTexCoord(0, 1, 0, 1)
    btn.icon:SetAlpha(1.0)
end

local function CreateCleanupButton(parentFrame)
    if parentFrame.CleanupBagsButton then
        return parentFrame.CleanupBagsButton
    end

    local btn = CreateFrame("Button", nil, parentFrame, "SecureActionButtonTemplate")
    btn:SetSize(24, 24)
    btn:SetFrameStrata("HIGH")
    btn:SetFrameLevel((parentFrame:GetFrameLevel() or 0) + 50)
    btn:SetPoint("TOPRIGHT", parentFrame, "TOPRIGHT", -28, -6)
    btn:EnableMouse(true)

    local t = btn:GetNormalTexture()
    if t then t:Hide() end
    local pt = btn:GetPushedTexture()
    if pt then pt:Hide() end
    local ht = btn:GetHighlightTexture()
    if ht then ht:Hide() end

    ApplyIconToButton(btn)

    local hl = btn:CreateTexture(nil, "HIGHLIGHT")
    hl:SetTexture("Interface\\Buttons\\ButtonHilight-Square")
    hl:SetAllPoints(btn)
    btn:SetHighlightTexture(hl)

    btn:RegisterForClicks("LeftButtonUp")

    btn:SetScript("OnMouseDown", function(self)
        if self.icon then self.icon:SetAlpha(0.6) end
    end)
    btn:SetScript("OnMouseUp", function(self)
        if self.icon then self.icon:SetAlpha(1.0) end
    end)
    btn:SetScript("OnLeave", function(self)
        if self.icon then self.icon:SetAlpha(1.0) end
        GameTooltip:Hide()
    end)

    btn:SetAttribute("type1", "macro")
    btn:SetAttribute("macrotext1", "/say .sort bags")

    btn:SetAttribute("shift-type1", "macro")
    btn:SetAttribute("shift-macrotext1", "/say .sort all")

    btn:SetAttribute("ctrl-type1", "macro")
    btn:SetAttribute("ctrl-macrotext1", "/say .sort bank")

    btn:SetScript("OnEnter", function(self)
        GameTooltip:SetOwner(self, "ANCHOR_RIGHT")
        GameTooltip:AddLine("Очистка")
        GameTooltip:AddLine("Левый-клик: Сортировка сумки", 1, 1, 1)
        GameTooltip:AddLine("Ctrl + Левый-клик: Сортировка банка", 0.8, 0.8, 0.8)
        GameTooltip:AddLine("Shift + Левый-клик: Сортировка сумки и банка", 0.8, 0.8, 0.8)
        GameTooltip:Show()
    end)

    parentFrame.CleanupBagsButton = btn
    return btn
end

local function EnsureBackpackButtonOnFrames()
    if not NUM_CONTAINER_FRAMES then return end
    for i = 1, NUM_CONTAINER_FRAMES do
        local frame = _G["ContainerFrame"..i]
        if frame and frame.GetID and frame:GetID() == 0 then
            local btn = CreateCleanupButton(frame)
            if btn then btn:Show() end
        elseif frame and frame.CleanupBagsButton then
            frame.CleanupBagsButton:Hide()
        end
    end
end

if type(ContainerFrame_SetID) == "function" then
    hooksecurefunc("ContainerFrame_SetID", function(containerFrame, id)
        if id == 0 then
            local btn = CreateCleanupButton(containerFrame)
            if btn then btn:Show() end
        else
            if containerFrame.CleanupBagsButton then
                containerFrame.CleanupBagsButton:Hide()
            end
        end
    end)
else
    if type(ToggleBackpack) == "function" then
        hooksecurefunc("ToggleBackpack", EnsureBackpackButtonOnFrames)
    end
    local ev = CreateFrame("Frame")
    ev:RegisterEvent("PLAYER_LOGIN")
    ev:RegisterEvent("BAG_UPDATE")
    ev:RegisterEvent("BAG_OPEN")
    ev:SetScript("OnEvent", EnsureBackpackButtonOnFrames)
end

local initFrame = CreateFrame("Frame")
initFrame:RegisterEvent("PLAYER_LOGIN")
initFrame:SetScript("OnEvent", function()
    EnsureBackpackButtonOnFrames()
    DEFAULT_CHAT_FRAME:AddMessage("CleanupBags: инициализирован")
end)