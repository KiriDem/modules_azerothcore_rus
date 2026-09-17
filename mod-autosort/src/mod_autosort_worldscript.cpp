/*
 * Credits: silviu20092
 */

#include "ScriptMgr.h"
#include "mod_autosort.h"

class mod_autosort_worldscript : public WorldScript
{
public:
    mod_autosort_worldscript() : WorldScript("mod_autosort_worldscript",
        {
            WORLDHOOK_ON_AFTER_CONFIG_LOAD
        }) {
    }

    void OnAfterConfigLoad(bool reload) override
    {
        sModAutosort->LoadConfig(reload);
    }
};

void AddSC_mod_autosort_worldscript()
{
    new mod_autosort_worldscript();
}
