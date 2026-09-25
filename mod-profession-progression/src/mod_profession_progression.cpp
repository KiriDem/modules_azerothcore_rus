/*
 * mod-profession-progression
 *
 * Progressive primary-profession slot/cap system for AzerothCore.
 *
 * Design:
 *  - Base primary profession slots: 2.
 *  - When the player's top two primary professions both reach UnlockThirdAt, one extra slot opens.
 *  - When the player's top two primary professions both reach UnlockFourthAt, a fourth slot opens.
 *  - Extra professions cannot exceed the lower skill of the pair above them.
 *  - Once professions #3 and #4 are both mastered for the current profession era, #5 and #6 open;
 *    once #5/#6 are mastered, #7/#8 open, and so on up to the active profession count.
 *  - Profession era controls the active skill cap and expansion professions:
 *      0 = Vanilla: 300 cap, 9 classic professions.
 *      1 = TBC:     375 cap, Jewelcrafting can be enabled.
 *      2 = Wrath:   425 cap by default here, Inscription can be enabled.
 *
 * This module does not unlearn professions and does not bypass trainer tier requirements.
 * It only controls free primary slots and caps max skill through PlayerScript hooks.
 */

#include "Chat.h"
#include "Config.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "SharedDefines.h"

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

namespace ProfessionProgression
{
    // Vanilla primary professions
    static constexpr uint32 SKILL_BLACKSMITHING = 164;
    static constexpr uint32 SKILL_LEATHERWORKING = 165;
    static constexpr uint32 SKILL_ALCHEMY = 171;
    static constexpr uint32 SKILL_HERBALISM = 182;
    static constexpr uint32 SKILL_MINING = 186;
    static constexpr uint32 SKILL_TAILORING = 197;
    static constexpr uint32 SKILL_ENGINEERING = 202;
    static constexpr uint32 SKILL_ENCHANTING = 333;
    static constexpr uint32 SKILL_SKINNING = 393;

    // Expansion primaries, off by default for a Vanilla-feeling server.
    static constexpr uint32 SKILL_JEWELCRAFTING = 755;
    static constexpr uint32 SKILL_INSCRIPTION = 773;

    struct ProfState
    {
        uint32 Skill = 0;
        uint32 Value = 0;
    };

    static bool Enabled()
    {
        return sConfigMgr->GetOption<bool>("ProfessionProgression.Enable", true);
    }

    static bool Announce()
    {
        return sConfigMgr->GetOption<bool>("ProfessionProgression.Announce", true);
    }

    static bool Debug()
    {
        return sConfigMgr->GetOption<bool>("ProfessionProgression.Debug", false);
    }

    static uint32 ExpansionStage()
    {
        // 0 = Vanilla, 1 = TBC, 2 = Wrath. Clamp so a typo does not open future professions.
        return std::min<uint32>(2, sConfigMgr->GetOption<uint32>("ProfessionProgression.ExpansionStage", 0));
    }

    static char const* EraName()
    {
        switch (ExpansionStage())
        {
            case 1: return "TBC";
            case 2: return "Wrath";
            default: return "Vanilla";
        }
    }

    static uint32 BaseSlots()
    {
        return std::max<uint32>(2, sConfigMgr->GetOption<uint32>("ProfessionProgression.BaseSlots", 2));
    }

    static uint32 MaxSkillCap()
    {
        // Manual cap override for quick testing. 0 means use the current era cap.
        uint32 manual = sConfigMgr->GetOption<uint32>("ProfessionProgression.MaxSkillCap", 0);
        if (manual > 0)
            return manual;

        switch (ExpansionStage())
        {
            case 1:
                return sConfigMgr->GetOption<uint32>("ProfessionProgression.TbcSkillCap", 375);
            case 2:
                return sConfigMgr->GetOption<uint32>("ProfessionProgression.WrathSkillCap", 425);
            default:
                return sConfigMgr->GetOption<uint32>("ProfessionProgression.VanillaSkillCap", 300);
        }
    }

    static uint32 UnlockThirdAt()
    {
        return sConfigMgr->GetOption<uint32>("ProfessionProgression.UnlockThirdAt", 75);
    }

    static uint32 UnlockFourthAt()
    {
        return sConfigMgr->GetOption<uint32>("ProfessionProgression.UnlockFourthAt", 150);
    }

    static uint32 PairUnlockAt()
    {
        // 0 = require mastery of the current era cap: 300/375/425 by default.
        uint32 configured = sConfigMgr->GetOption<uint32>("ProfessionProgression.PairUnlockAt", 0);
        return configured > 0 ? configured : MaxSkillCap();
    }

    static std::vector<uint32> PrimarySkillIds()
    {
        std::vector<uint32> skills = {
            SKILL_ALCHEMY,
            SKILL_BLACKSMITHING,
            SKILL_ENCHANTING,
            SKILL_ENGINEERING,
            SKILL_HERBALISM,
            SKILL_LEATHERWORKING,
            SKILL_MINING,
            SKILL_SKINNING,
            SKILL_TAILORING
        };

        bool autoExpansionProfessions = sConfigMgr->GetOption<bool>("ProfessionProgression.AutoExpansionProfessions", true);

        bool includeJewelcrafting = autoExpansionProfessions
            ? ExpansionStage() >= 1
            : sConfigMgr->GetOption<bool>("ProfessionProgression.IncludeJewelcrafting", false);

        bool includeInscription = autoExpansionProfessions
            ? ExpansionStage() >= 2
            : sConfigMgr->GetOption<bool>("ProfessionProgression.IncludeInscription", false);

        if (includeJewelcrafting)
            skills.push_back(SKILL_JEWELCRAFTING);

        if (includeInscription)
            skills.push_back(SKILL_INSCRIPTION);

        return skills;
    }

    static uint32 MaxSlots()
    {
        uint32 activeProfessions = static_cast<uint32>(PrimarySkillIds().size());

        if (sConfigMgr->GetOption<bool>("ProfessionProgression.AutoMaxPrimarySlots", true))
            return std::max<uint32>(BaseSlots(), activeProfessions);

        uint32 configured = sConfigMgr->GetOption<uint32>("ProfessionProgression.MaxPrimarySlots", activeProfessions);
        configured = std::max<uint32>(BaseSlots(), configured);
        return std::min<uint32>(configured, activeProfessions);
    }

    static bool IsPrimaryProfession(uint32 skillId)
    {
        for (uint32 skill : PrimarySkillIds())
            if (skill == skillId)
                return true;
        return false;
    }

    static std::vector<ProfState> KnownPrimaryProfessions(Player* player)
    {
        std::vector<ProfState> out;
        if (!player)
            return out;

        for (uint32 skill : PrimarySkillIds())
        {
            if (!player->HasSkill(skill))
                continue;

            uint32 value = player->GetSkillValue(skill);
            if (value == 0)
                value = 1;

            out.push_back({ skill, value });
        }

        std::sort(out.begin(), out.end(), [](ProfState const& a, ProfState const& b)
        {
            if (a.Value != b.Value)
                return a.Value > b.Value;
            return a.Skill < b.Skill;
        });

        return out;
    }

    static uint32 PairMin(std::vector<ProfState> const& profs, uint32 start)
    {
        if (start + 1 >= profs.size())
            return 0;
        return std::min(profs[start].Value, profs[start + 1].Value);
    }

    static uint32 AllowedSlots(Player* player)
    {
        std::vector<ProfState> profs = KnownPrimaryProfessions(player);
        uint32 allowed = BaseSlots();
        uint32 maxSlots = MaxSlots();

        if (profs.size() >= 2)
        {
            uint32 mainPairMin = PairMin(profs, 0);
            if (mainPairMin >= UnlockThirdAt())
                allowed = std::max<uint32>(allowed, 3);
            if (mainPairMin >= UnlockFourthAt())
                allowed = std::max<uint32>(allowed, 4);
        }

        // After slots 3/4 exist, each mastered pair opens the next pair.
        // Example: when #3 and #4 both hit 300, #5 and #6 open.
        for (uint32 pairStart = 2; pairStart + 1 < profs.size() && allowed >= pairStart + 2; pairStart += 2)
        {
            if (PairMin(profs, pairStart) >= PairUnlockAt())
                allowed += 2;
            else
                break;

            if (allowed >= maxSlots)
            {
                allowed = maxSlots;
                break;
            }
        }

        return std::min(allowed, maxSlots);
    }

    static int32 RankOfSkill(std::vector<ProfState> const& profs, uint32 skillId)
    {
        for (size_t i = 0; i < profs.size(); ++i)
            if (profs[i].Skill == skillId)
                return static_cast<int32>(i);
        return -1;
    }

    // Progressive cap for a known profession. This is an upper bound only; the normal trainer tier
    // system still controls whether the player has learned Journeyman/Expert/Artisan.
    static uint32 ProgressionCapFor(Player* player, uint32 skillId)
    {
        if (!player || !IsPrimaryProfession(skillId))
            return MaxSkillCap();

        std::vector<ProfState> profs = KnownPrimaryProfessions(player);
        int32 rank = RankOfSkill(profs, skillId);
        if (rank < 0)
            return MaxSkillCap();

        uint32 allowed = AllowedSlots(player);

        // Top two are the active mains and may always climb to the configured cap.
        if (rank < 2)
            return MaxSkillCap();

        // Known beyond the currently earned slots: do not delete it, but stop skill gains.
        if (static_cast<uint32>(rank) >= allowed)
            return profs[rank].Value;

        // Slots 3/4 are capped by slots 1/2; slots 5/6 by slots 3/4; etc.
        uint32 parentPairStart = ((static_cast<uint32>(rank) - 2) / 2) * 2;
        uint32 parentMin = PairMin(profs, parentPairStart);
        if (parentMin == 0)
            return profs[rank].Value;

        return std::min(parentMin, MaxSkillCap());
    }

    static void RefreshFreeSlots(Player* player, bool notify)
    {
        if (!player || !Enabled())
            return;

        std::vector<ProfState> profs = KnownPrimaryProfessions(player);
        uint32 known = static_cast<uint32>(profs.size());
        uint32 allowed = AllowedSlots(player);
        uint32 free = allowed > known ? allowed - known : 0;

        // In AzerothCore this field is remaining free primary-profession points, not total slots.
        player->SetFreePrimaryProfessions(free);

        if (notify && Announce() && player->GetSession())
        {
            ChatHandler(player->GetSession()).PSendSysMessage(
/*                "|cff00ff99Profession Progression:|r %u/%u основных профессий. Свободные слоты для профессий: %u. Эра: %s, кап: %u.", */
                "На этом сервере работает модуль |cff00ff99Profession Progression|r.",
                known, allowed, free, EraName(), MaxSkillCap());
        }
    }

    static void CapGainIfNeeded(Player* player, uint32 skillId, uint32 current, uint32& gain)
    {
        if (!player || !Enabled() || gain == 0 || !IsPrimaryProfession(skillId))
            return;

        uint32 cap = ProgressionCapFor(player, skillId);
        if (current >= cap)
        {
            gain = 0;
            if (Announce() && player->GetSession())
            {
                ChatHandler(player->GetSession()).PSendSysMessage(
                    "|cffffcc00Profession Progression:|r Уровень этой профессии в данный момент ограничен значением %u. Чтобы продолжить развитие, повысьте уровень связанной профессии или откройте следующую Эру профессий.",
                    cap);
            }
            return;
        }

        if (current + gain > cap)
            gain = cap - current;
    }
}

class ProfessionProgression_PlayerScript : public PlayerScript
{
public:
    ProfessionProgression_PlayerScript() : PlayerScript("ProfessionProgression_PlayerScript", {
        PLAYERHOOK_ON_LOGIN,
        PLAYERHOOK_ON_LEARN_SPELL,
        PLAYERHOOK_ON_GET_MAX_SKILL_VALUE,
        PLAYERHOOK_ON_UPDATE_GATHERING_SKILL,
        PLAYERHOOK_ON_UPDATE_CRAFTING_SKILL,
        PLAYERHOOK_ON_CAN_UPDATE_SKILL,
        PLAYERHOOK_ON_UPDATE_SKILL,
        PLAYERHOOK_ON_SET_SKILL
    }) { }

    void OnPlayerLogin(Player* player) override
    {
        ProfessionProgression::RefreshFreeSlots(player, true);
    }

    void OnPlayerLearnSpell(Player* player, uint32 /*spellID*/) override
    {
        ProfessionProgression::RefreshFreeSlots(player, false);
    }

    void OnPlayerGetMaxSkillValue(Player* player, uint32 skill, int32& result, bool /*IsPure*/) override
    {
        if (!ProfessionProgression::Enabled() || !ProfessionProgression::IsPrimaryProfession(skill))
            return;

        uint32 cap = ProfessionProgression::ProgressionCapFor(player, skill);
        if (cap > 0 && result > static_cast<int32>(cap))
            result = static_cast<int32>(cap);
    }

    bool OnPlayerCanUpdateSkill(Player* player, uint32 skillId) override
    {
        if (!ProfessionProgression::Enabled() || !ProfessionProgression::IsPrimaryProfession(skillId))
            return true;

        uint32 current = player ? player->GetSkillValue(skillId) : 0;
        uint32 cap = ProfessionProgression::ProgressionCapFor(player, skillId);
        return current < cap;
    }

    void OnPlayerUpdateGatheringSkill(Player* player, uint32 skillId, uint32 currentLevel,
        uint32 /*gray*/, uint32 /*green*/, uint32 /*yellow*/, uint32& gain) override
    {
        ProfessionProgression::CapGainIfNeeded(player, skillId, currentLevel, gain);
    }

    void OnPlayerUpdateCraftingSkill(Player* player, SkillLineAbilityEntry const* skill,
        uint32 currentLevel, uint32& gain) override
    {
        if (!skill)
            return;
        ProfessionProgression::CapGainIfNeeded(player, skill->SkillLine, currentLevel, gain);
    }

    void OnPlayerUpdateSkill(Player* player, uint32 skillId, uint32 /*value*/, uint32 /*max*/, uint32 /*step*/, uint32 /*newValue*/) override
    {
        if (ProfessionProgression::IsPrimaryProfession(skillId))
            ProfessionProgression::RefreshFreeSlots(player, false);
    }

    void OnPlayerSetSkill(Player* player, uint32 skillId, uint32 /*value*/, uint32 /*max*/, uint32 /*step*/, uint32 /*newValue*/) override
    {
        if (ProfessionProgression::IsPrimaryProfession(skillId))
            ProfessionProgression::RefreshFreeSlots(player, false);
    }
};

void AddProfessionProgressionScripts()
{
    new ProfessionProgression_PlayerScript();
}

void AddSC_profession_progression()
{
    AddProfessionProgressionScripts();
}

// Loader entry point. This name MUST match the module folder
// (mod-profession-progression -> Addmod_profession_progressionScripts).
void Addmod_profession_progressionScripts()
{
    AddSC_profession_progression();
}
