/*
 * Credits: silviu20092
 */

#include "ScriptMgr.h"
#include "Chat.h"
#include "ChatCommand.h"
#include "mod_autosort.h"

using namespace Acore::ChatCommands;

class mod_autosort_commandscript : public CommandScript
{
public:
    mod_autosort_commandscript() : CommandScript("mod_autosort_commandscript") {}

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable sortCommandTable =
        {
            { "bags", HandleSortBagsCommand, SEC_PLAYER, Console::No },
            { "bank", HandleSortBankCommand, SEC_PLAYER, Console::No },
            { "all",  HandleSortAllCommand,  SEC_PLAYER, Console::No },
        };

        static ChatCommandTable commandTable =
        {
            { "sort", sortCommandTable },
        };

        return commandTable;
    }

private:
    static bool CheckEnabledAndCooldown(ChatHandler* handler, Player* player)
    {
        if (!sModAutosort->IsEnabled())
        {
            handler->SendSysMessage("Автоматическая сортировка недоступна..");
            return false;
        }

        if (sModAutosort->IsOnCooldown(player))
        {
            handler->SendSysMessage("Вы слишком часто выполняете сортировку. Пожалуйста, подождите еще несколько секунд и попробуйте снова..");
            return false;
        }

        return true;
    }

    static bool HandleSortBagsCommand(ChatHandler* handler)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!CheckEnabledAndCooldown(handler, player))
            return true;

        sModAutosort->DoSort(player, true, false);
        sModAutosort->RefreshCooldown(player);
        handler->SendSysMessage("Ваша сумка успешно отсортированна!");

        return true;
    }

    static bool HandleSortBankCommand(ChatHandler* handler)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!CheckEnabledAndCooldown(handler, player))
            return true;

        sModAutosort->DoSort(player, false, true);
        sModAutosort->RefreshCooldown(player);
        handler->SendSysMessage("Ваш банк успешно отсортированны!");

        return true;
    }

    static bool HandleSortAllCommand(ChatHandler* handler)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!CheckEnabledAndCooldown(handler, player))
            return true;

        sModAutosort->DoSort(player, true, true);
        sModAutosort->RefreshCooldown(player);
        handler->SendSysMessage("Ваши сумки и банк успешно отсортированны.!");

        return true;
    }
};

void AddSC_mod_autosort_commandscript()
{
    new mod_autosort_commandscript();
}
