/*
 * Credits: silviu20092
 */

#ifndef _MOD_AUTOSORT_CONFIG_H_
#define _MOD_AUTOSORT_CONFIG_H_

#include <string>
#include "Define.h"

enum ModAutosortBoolConfigs
{
    AUTOSORT_CONFIG_IS_ENABLED,
    AUTOSORT_CONFIG_MERGE_STACKS,
    AUTOSORT_CONFIG_SORT_ENABLED,
    AUTOSORT_CONFIG_SORT_ON_LOGIN,
    AUTOSORT_CONFIG_IS_PERIODIC_ENABLED,
    AUTOSORT_CONFIG_SKIP_PERIODIC_IF_LOOTING,
    AUTOSORT_CONFIG_SKIP_PERIODIC_IF_IN_COMBAT,
    AUTOSORT_CONFIG_SORT_BANK_ON_PERIODIC,
    MAX_MOD_AUTOSORT_BOOL_CONFIGS
};

enum ModAutosortIntConfigs
{
    AUTOSORT_CONFIG_COOLDOWN,
    AUTOSORT_CONFIG_PERIODIC_INTERVAL,
    MAX_MOD_AUTOSORT_INT_CONFIGS
};

enum ModAutosortStringConfigs
{
    AUTOSORT_CONFIG_PINNED_ITEMS,
    AUTOSORT_CONFIG_PERIODIC_SORT_MESSAGE,
    MAX_MOD_AUTOSORT_STRING_CONFIGS
};

class ModAutosortConfig
{
private:
    bool boolConfigs[MAX_MOD_AUTOSORT_BOOL_CONFIGS];
    std::string stringConfigs[MAX_MOD_AUTOSORT_STRING_CONFIGS];
    int32 intConfigs[MAX_MOD_AUTOSORT_INT_CONFIGS];
public:
    ModAutosortConfig();

    void Initialize();

    bool GetBoolConfig(ModAutosortBoolConfigs index) const;
    std::string GetStringConfig(ModAutosortStringConfigs index) const;
    int32 GetIntConfig(ModAutosortIntConfigs index) const;
};

#endif // _MOD_AUTOSORT_CONFIG_H_
