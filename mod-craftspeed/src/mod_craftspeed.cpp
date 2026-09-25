/*
 * AzerothCore module: mod_crafting_casttime
 *
 * Description:
 *   Globally modifies the cast time of crafting spells (those that create items
 *   or enchant items and have reagent1 > 0) by a configurable multiplier.
 *
 *   CraftingCastTime.Enable      (bool)   - enable/disable the module
 *   CraftingCastTime.Multiplier  (float)  - cast time multiplier (0.25 = 25% of normal)
 */

#include "Config.h"
#include "DBCStructure.h"
#include "Log.h"
#include "ScriptMgr.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "World.h"

namespace
{
    static constexpr bool  DEFAULT_ENABLE = true;
    static constexpr float DEFAULT_MULTIPLIER = 0.25f;
    static constexpr float MIN_MULTIPLIER = 0.01f;

    // Determine if a spell is a "crafting" spell for our purposes:
    //  - consumes reagent1 (Reagent[0] > 0)
    //  - AND has at least one effect that either creates or enchants an item
    bool IsCraftingSpell(SpellInfo const* spellInfo)
    {
        if (!spellInfo)
            return false;

        // Only spells that actually consume at least one reagent slot 1
        if (spellInfo->Reagent[0] <= 0)
            return false;

        bool isRelevant = false;

        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        {
            SpellEffectInfo const& effect = spellInfo->Effects[i];

            switch (effect.Effect)
            {
                // Classic crafting / trade skill
            case SPELL_EFFECT_TRADE_SKILL:
            case SPELL_EFFECT_CREATE_ITEM:
            case SPELL_EFFECT_CREATE_ITEM_2:
                isRelevant = true;
                break;

                // Enchanting
            case SPELL_EFFECT_ENCHANT_ITEM:
            case SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY:
                isRelevant = true;
                break;

            default:
                break;
            }

            if (isRelevant)
                break;
        }

        return isRelevant;
    }

    class CraftingCastTimeWorldScript : public WorldScript
    {
    public:
        CraftingCastTimeWorldScript()
            : WorldScript("CraftingCastTimeWorldScript")
        {
        }

        void OnAfterConfigLoad(bool /*reload*/) override
        {
            bool enable = sConfigMgr->GetOption<bool>("CraftingCastTime.Enable", DEFAULT_ENABLE);
            float multiplier = sConfigMgr->GetOption<float>("CraftingCastTime.Multiplier", DEFAULT_MULTIPLIER);

            if (multiplier <= 0.0f)
                multiplier = MIN_MULTIPLIER;

            LOG_INFO("module",
                "mod_crafting_casttime: config loaded: Enable = {}, Multiplier = {:.3f}",
                enable ? "true" : "false", multiplier);
        }

        void OnStartup() override
        {
            bool enable = sConfigMgr->GetOption<bool>("CraftingCastTime.Enable", DEFAULT_ENABLE);
            if (!enable)
            {
                LOG_INFO("module", "mod_crafting_casttime: disabled by config, skipping spell modification.");
                return;
            }

            float multiplier = sConfigMgr->GetOption<float>("CraftingCastTime.Multiplier", DEFAULT_MULTIPLIER);
            if (multiplier <= 0.0f)
                multiplier = MIN_MULTIPLIER;

            uint32 totalSpells = 0;
            uint32 modifiedSpells = 0;

            uint32 const spellStoreSize = sSpellMgr->GetSpellInfoStoreSize();

            for (uint32 spellId = 1; spellId < spellStoreSize; ++spellId)
            {
                SpellInfo const* spellInfoConst = sSpellMgr->GetSpellInfo(spellId);
                if (!spellInfoConst)
                    continue;

                ++totalSpells;

                if (!IsCraftingSpell(spellInfoConst))
                    continue;

                SpellInfo* spellInfo = const_cast<SpellInfo*>(spellInfoConst);

                if (!spellInfo->CastTimeEntry)
                    continue;

                uint32 baseCastTime = spellInfo->CalcCastTime();
                if (baseCastTime == 0)
                    continue;

                uint32 newCastTime = static_cast<uint32>(float(baseCastTime) * multiplier);
                if (newCastTime > baseCastTime && multiplier < 1.0f)
                    newCastTime = baseCastTime;

                // 0 is allowed (instant cast)
                // If you want to keep a minimum cast time (e.g. 500ms), clamp here.
                // if (newCastTime < 500u) newCastTime = 500u;

                SpellCastTimesEntry const* originalEntry = spellInfo->CastTimeEntry;
                SpellCastTimesEntry* clonedEntry = new SpellCastTimesEntry(*originalEntry);
                clonedEntry->CastTime = newCastTime;

                spellInfo->CastTimeEntry = clonedEntry;

                ++modifiedSpells;
            }

            LOG_INFO("module",
                "mod_crafting_casttime: processed {} spells, modified {} crafting/enchanting spells with multiplier {:.3f}",
                totalSpells, modifiedSpells, multiplier);
        }
    };
}

void AddCraftSpeedScripts()
{
    new CraftingCastTimeWorldScript();
}
