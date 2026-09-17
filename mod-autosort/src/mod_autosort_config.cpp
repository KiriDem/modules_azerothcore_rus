/*
 * Credits: silviu20092
 */

#include <cstring>
#include "Config.h"
#include "mod_autosort_config.h"

ModAutosortConfig::ModAutosortConfig()
{
    std::memset(boolConfigs, 0, sizeof(boolConfigs));
    std::memset(intConfigs, 0, sizeof(intConfigs));
}

void ModAutosortConfig::Initialize()
{
    boolConfigs[AUTOSORT_CONFIG_IS_ENABLED] = sConfigMgr->GetOption<bool>("AutoSort.Enable", true);
    boolConfigs[AUTOSORT_CONFIG_MERGE_STACKS] = sConfigMgr->GetOption<bool>("AutoSort.MergeStacks", true);
    boolConfigs[AUTOSORT_CONFIG_SORT_ENABLED] = sConfigMgr->GetOption<bool>("AutoSort.SortBags", true);
    boolConfigs[AUTOSORT_CONFIG_SORT_ON_LOGIN] = sConfigMgr->GetOption<bool>("AutoSort.OnLogin", false);
    boolConfigs[AUTOSORT_CONFIG_IS_PERIODIC_ENABLED] = sConfigMgr->GetOption<bool>("AutoSort.Periodic.Enable", false);
    boolConfigs[AUTOSORT_CONFIG_SKIP_PERIODIC_IF_LOOTING] = sConfigMgr->GetOption<bool>("AutoSort.Periodic.SkipIfLooting", true);
    boolConfigs[AUTOSORT_CONFIG_SKIP_PERIODIC_IF_IN_COMBAT] = sConfigMgr->GetOption<bool>("AutoSort.Periodic.SkipInCombat", true);
    boolConfigs[AUTOSORT_CONFIG_SORT_BANK_ON_PERIODIC] = sConfigMgr->GetOption<bool>("AutoSort.Periodic.SortBank", false);

    intConfigs[AUTOSORT_CONFIG_COOLDOWN] = sConfigMgr->GetOption<int32>("AutoSort.Cooldown", 5);
    if (intConfigs[AUTOSORT_CONFIG_COOLDOWN] <= 0)
        intConfigs[AUTOSORT_CONFIG_COOLDOWN] = 5;
    intConfigs[AUTOSORT_CONFIG_PERIODIC_INTERVAL] = sConfigMgr->GetOption<int32>("AutoSort.Periodic.Interval", 60);
    if (intConfigs[AUTOSORT_CONFIG_PERIODIC_INTERVAL] <= 0)
        intConfigs[AUTOSORT_CONFIG_PERIODIC_INTERVAL] = 60;

    stringConfigs[AUTOSORT_CONFIG_PINNED_ITEMS] = sConfigMgr->GetOption<std::string>("AutoSort.PinnedItems", "6948");
    stringConfigs[AUTOSORT_CONFIG_PERIODIC_SORT_MESSAGE] = sConfigMgr->GetOption<std::string>("AutoSort.Periodic.Message", "");
}

bool ModAutosortConfig::GetBoolConfig(ModAutosortBoolConfigs index) const
{
    return boolConfigs[index];
}

std::string ModAutosortConfig::GetStringConfig(ModAutosortStringConfigs index) const
{
    return stringConfigs[index];
}

int32 ModAutosortConfig::GetIntConfig(ModAutosortIntConfigs index) const
{
    return intConfigs[index];
}
