/*
 * Credits: silviu20092
 */

#include "ScriptMgr.h"
#include "Chat.h"
#include "mod_autosort.h"

class mod_autosort_playerscript : public PlayerScript
{
private:
    std::mutex periodicElapsedMutex;
    std::unordered_map<ObjectGuid, uint32> periodicElapsed;

    class SendAutosortEvent : public BasicEvent
    {
    public:
        SendAutosortEvent(Player* player) : player(player)
        {
            player->m_Events.AddEvent(this, player->m_Events.CalculateTime(DELAY_MS));
        }

        bool Execute(uint64 /*e_time*/, uint32 /*p_time*/)
        {
            if (sModAutosort->IsEnabled() && sModAutosort->SortOnLogin())
                sModAutosort->DoSort(player, true, false);

            return true;
        }
    private:
        static constexpr uint64 DELAY_MS = 4000;

        Player* player;
    };
public:
    mod_autosort_playerscript() : PlayerScript("mod_autosort_playerscript",
        {
            PLAYERHOOK_ON_LOGIN,
            PLAYERHOOK_ON_UPDATE,
            PLAYERHOOK_ON_LOGOUT
        }
    )
    {
    }

    void OnPlayerLogin(Player* player) override
    {
        new SendAutosortEvent(player);
    }

    void OnPlayerLogout(Player* player) override
    {
        periodicElapsed.erase(player->GetGUID());
    }

    void OnPlayerUpdate(Player* player, uint32 diff) override
    {
        if (!sModAutosort->IsEnabled())
            return;

        if (!sModAutosort->IsPeriodicEnabled())
            return;

        if (sModAutosort->ShouldSkipPeriodicSort(player))
            return;

        uint32 intervalMs = sModAutosort->PeriodicInterval() * 1000;
        if (intervalMs == 0)
            return;

        {
            std::lock_guard<std::mutex> lock(periodicElapsedMutex);

            uint32& elapsed = periodicElapsed[player->GetGUID()];
            elapsed += diff;

            if (elapsed < intervalMs)
                return;

            elapsed = 0;
        }

        bool skipIfLooting = sModAutosort->SkipPeriodicIfLooting();
        if (skipIfLooting && !player->GetLootGUID().IsEmpty())
            return;

        bool skipInCombat = sModAutosort->SkipPeriodicIfInCombat();
        if (skipInCombat && player->IsInCombat())
            return;

        bool sortBank = sModAutosort->SortBankOnPeriodic();
        sModAutosort->DoSort(player, true, sortBank);

        std::string message = sModAutosort->PeriodicSortMessage();
        if (!message.empty())
            ChatHandler(player->GetSession()).SendSysMessage(message);
    }
};

void AddSC_mod_autosort_playerscript()
{
    new mod_autosort_playerscript();
}
