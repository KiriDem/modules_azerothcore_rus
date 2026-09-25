/*
 * mod-dungeon-master — dm_command_script.cpp
 * GM commands: .dm reload, .dm status, .dm list, .dm end, .dm clearcooldown
 */

#include "ScriptMgr.h"
#include "Chat.h"
#include "ChatCommand.h"
#include "Player.h"
#include "Group.h"
#include "DungeonMasterMgr.h"
#include "DMConfig.h"
#include <cstdio>

using namespace Acore::ChatCommands;
using namespace DungeonMaster;

class dm_command_script : public CommandScript
{
public:
    dm_command_script() : CommandScript("dm_command_script") {}

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable dmTable =
        {
            { "reload",        HandleReload,        SEC_ADMINISTRATOR,  Console::Yes },
            { "status",        HandleStatus,        SEC_GAMEMASTER,     Console::Yes },
            { "list",          HandleList,           SEC_GAMEMASTER,     Console::Yes },
            { "end",           HandleEnd,            SEC_ADMINISTRATOR,  Console::No  },
            { "clearcooldown", HandleClearCD,        SEC_GAMEMASTER,     Console::No  },
        };
        static ChatCommandTable root = { { "dm", dmTable } };
        return root;
    }

    static bool HandleReload(ChatHandler* h)
    {
        sDMConfig->LoadConfig(true);
        h->SendSysMessage("DungeonMaster: Конфигурация перезагруженна.");
        return true;
    }

    static bool HandleStatus(ChatHandler* h)
    {
        char buf[256];
        h->SendSysMessage("=== Dungeon Master Статус ===");
        snprintf(buf, sizeof(buf), "Включенно: %s", sDMConfig->IsEnabled() ? "Yes" : "No");
        h->SendSysMessage(buf);
        snprintf(buf, sizeof(buf), "Активно: %u / %u",
            sDungeonMasterMgr->GetActiveSessionCount(), sDMConfig->GetMaxConcurrentRuns());
        h->SendSysMessage(buf);
        snprintf(buf, sizeof(buf), "Диапазон Уровней: +/-%u", sDMConfig->GetLevelBand());
        h->SendSysMessage(buf);
        snprintf(buf, sizeof(buf), "Сложность: %u  Тема: %u  Подземелье: %u",
            uint32(sDMConfig->GetDifficulties().size()),
            uint32(sDMConfig->GetThemes().size()),
            uint32(sDMConfig->GetDungeons().size()));
        h->SendSysMessage(buf);
        return true;
    }

    static bool HandleList(ChatHandler* h)
    {
        uint32 n = sDungeonMasterMgr->GetActiveSessionCount();
        char buf[128];
        snprintf(buf, sizeof(buf), "Активные сессии ДМ: %u", n);
        h->SendSysMessage(buf);
        return true;
    }

    static bool HandleEnd(ChatHandler* h, Optional<uint32> sessionId)
    {
        char buf[128];
        if (sessionId)
        {
            Session* s = sDungeonMasterMgr->GetSession(*sessionId);
            if (!s) { snprintf(buf, sizeof(buf), "Сеанс %u не найден.", *sessionId); h->SendSysMessage(buf); return false; }
            sDungeonMasterMgr->EndSession(*sessionId, false);
            snprintf(buf, sizeof(buf), "Сеанс %u закончен.", *sessionId); h->SendSysMessage(buf);
        }
        else
        {
            // Try the invoker's own session first
            Player* invoker = h->GetSession() ? h->GetSession()->GetPlayer() : nullptr;
            Session* s = invoker ? sDungeonMasterMgr->GetSessionByPlayer(invoker->GetGUID()) : nullptr;

            // Fall back to selected player's session
            if (!s)
            {
                Player* t = h->getSelectedPlayer();
                s = t ? sDungeonMasterMgr->GetSessionByPlayer(t->GetGUID()) : nullptr;
            }

            if (!s) { h->SendSysMessage("Вы не находитесь в сеансе мастера (DM). Выберите игрока или укажите ID сеанса."); return false; }
            uint32 id = s->SessionId;
            sDungeonMasterMgr->EndSession(id, false);
            snprintf(buf, sizeof(buf), "Сесси %u завершилась (все игроки телепортировались).", id); h->SendSysMessage(buf);
        }
        return true;
    }

    static bool HandleClearCD(ChatHandler* h)
    {
        Player* invoker = h->GetSession() ? h->GetSession()->GetPlayer() : nullptr;
        if (!invoker) { h->SendSysMessage("Только в игре."); return false; }

        // If invoker is in a group, clear cooldown for ALL group members
        Group* g = invoker->GetGroup();
        if (g)
        {
            uint32 cleared = 0;
            for (GroupReference* ref = g->GetFirstMember(); ref; ref = ref->next())
            {
                Player* member = ref->GetSource();
                if (member)
                {
                    sDungeonMasterMgr->ClearCooldown(member->GetGUID());
                    ++cleared;
                }
            }
            char buf[128];
            snprintf(buf, sizeof(buf), "Время восстановления сброшено для %u участника(-ов) группы.", cleared);
            h->SendSysMessage(buf);
        }
        else
        {
            // Solo — clear for self or selected player
            Player* t = h->getSelectedPlayer();
            if (!t) t = invoker;
            sDungeonMasterMgr->ClearCooldown(t->GetGUID());
            char buf[128];
            snprintf(buf, sizeof(buf), "Время восстановления для %s сброшено.", t->GetName().c_str());
            h->SendSysMessage(buf);
        }
        return true;
    }
};

void AddSC_dm_command_script()
{
    new dm_command_script();
}
