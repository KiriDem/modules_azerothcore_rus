/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "DressNpcMgr.h"
#include "AllCreatureScript.h"
#include "Chat.h"
#include "CommandScript.h"
#include "Config.h"
#include "Creature.h"
#include "Opcodes.h"
#include "ServerScript.h"
#include "WorldPacket.h"
#include "WorldScript.h"
#include "WorldSession.h"

using namespace Acore::ChatCommands;

class DressNpc_WorldScript : public WorldScript
{
public:
    DressNpc_WorldScript() : WorldScript("DressNpc_WorldScript", {
        WORLDHOOK_ON_AFTER_CONFIG_LOAD,
        WORLDHOOK_ON_BEFORE_WORLD_INITIALIZED
    }) { }

    void OnAfterConfigLoad(bool /*reload*/) override
    {
        sDressNpcMgr->Enabled = sConfigMgr->GetOption<bool>("DressNpc.Enable", true);
        sDressNpcMgr->SoundsEnabled = sConfigMgr->GetOption<bool>("DressNpc.Sounds.Enable", true);
    }

    // Fires after all world tables and DBCs are loaded but before grid
    // preloading spawns any creature, so outfits are ready for OnCreatureAddWorld.
    void OnBeforeWorldInitialized() override
    {
        if (!sDressNpcMgr->Enabled)
            return;

        sDressNpcMgr->LoadNpcSounds();
        sDressNpcMgr->LoadFromDB();
    }
};

class DressNpc_AllCreatureScript : public AllCreatureScript
{
public:
    DressNpc_AllCreatureScript() : AllCreatureScript("DressNpc_AllCreatureScript") { }

    void OnCreatureAddWorld(Creature* creature) override
    {
        if (!sDressNpcMgr->Enabled)
            return;

        sDressNpcMgr->OnCreatureAddWorld(creature);
    }

    void OnAllCreatureUpdate(Creature* creature, uint32 /*diff*/) override
    {
        if (!sDressNpcMgr->Enabled)
            return;

        sDressNpcMgr->OnCreatureUpdate(creature);
    }
};

class DressNpc_ServerScript : public ServerScript
{
public:
    DressNpc_ServerScript() : ServerScript("DressNpc_ServerScript", { SERVERHOOK_CAN_PACKET_RECEIVE }) { }

    bool CanPacketReceive(WorldSession* session, WorldPacket const& packet) override
    {
        if (!sDressNpcMgr->Enabled || packet.size() < 8)
            return true;

        switch (packet.GetOpcode())
        {
            case CMSG_GET_MIRRORIMAGE_DATA:
                // when answered here the core handler must not run, it would
                // reply for real mirror image clones only
                return !sDressNpcMgr->HandleMirrorImageRequest(session, ReadGuid(packet));
            // Workaround for missing client-side greeting sounds of dressed
            // NPCs: play the NPCSounds.dbc 'hello' sound on interaction.
            case CMSG_GOSSIP_HELLO:
            case CMSG_QUESTGIVER_HELLO:
            case CMSG_TRAINER_LIST:
            case CMSG_LIST_INVENTORY:
            case CMSG_BANKER_ACTIVATE:
            case MSG_AUCTION_HELLO:
            case MSG_TABARDVENDOR_ACTIVATE:
            case CMSG_PETITION_SHOWLIST:
            case CMSG_TAXIQUERYAVAILABLENODES:
            case MSG_LIST_STABLED_PETS:
            case CMSG_SET_SELECTION:
            case CMSG_GUILD_BANKER_ACTIVATE:
                sDressNpcMgr->PlayHelloSound(session, ReadGuid(packet));
                return true;
            default:
                return true;
        }
    }

private:
    // All intercepted packets start with a raw 8-byte guid. Read from a copy:
    // the packet is shared with the core handler that runs afterwards.
    static ObjectGuid ReadGuid(WorldPacket const& packet)
    {
        WorldPacket copy(packet);
        ObjectGuid guid;
        copy >> guid;
        return guid;
    }
};

class DressNpc_CommandScript : public CommandScript
{
public:
    DressNpc_CommandScript() : CommandScript("DressNpc_CommandScript") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable dressNpcCommandTable =
        {
            { "reload", HandleDressNpcReloadCommand, SEC_ADMINISTRATOR, Console::Yes }
        };

        static ChatCommandTable commandTable =
        {
            { "dressnpc", dressNpcCommandTable }
        };

        return commandTable;
    }

    static bool HandleDressNpcReloadCommand(ChatHandler* handler)
    {
        if (!sDressNpcMgr->Enabled)
        {
            handler->SendSysMessage("DressNpc module is disabled.");
            return true;
        }

        sDressNpcMgr->Reload();
        handler->SendGlobalGMSysMessage("DB tables `creature_template_outfits*` reloaded.");
        return true;
    }
};

void AddDressNpcScripts()
{
    new DressNpc_WorldScript();
    new DressNpc_AllCreatureScript();
    new DressNpc_ServerScript();
    new DressNpc_CommandScript();
}
