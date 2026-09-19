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

#ifndef MOD_DRESS_NPC_MGR_H
#define MOD_DRESS_NPC_MGR_H

#include "DBCStore.h"
#include "ObjectGuid.h"
#include "Player.h"
#include "SharedDefines.h"
#include <memory>
#include <unordered_map>

class Creature;
class WorldSession;

// NPCSounds.dbc is not loaded by the core; the module loads it itself
// to play greeting sounds for dressed NPCs (client plays none for them).
struct NPCSoundsEntry
{
    uint32 Id;
    uint32 hello;
    uint32 goodbye;
    uint32 pissed;
    uint32 ack;
};

class CreatureOutfit
{
public:
    // Model 11686 has no visual. Shown while re-dressing a visible creature so
    // clients drop their cached render and re-request the mirror image data.
    static constexpr uint32 INVISIBLE_MODEL = 11686;

    static constexpr EquipmentSlots ItemSlots[] =
    {
        EQUIPMENT_SLOT_HEAD,
        EQUIPMENT_SLOT_SHOULDERS,
        EQUIPMENT_SLOT_BODY,
        EQUIPMENT_SLOT_CHEST,
        EQUIPMENT_SLOT_WAIST,
        EQUIPMENT_SLOT_LEGS,
        EQUIPMENT_SLOT_FEET,
        EQUIPMENT_SLOT_WRISTS,
        EQUIPMENT_SLOT_HANDS,
        EQUIPMENT_SLOT_BACK,
        EQUIPMENT_SLOT_TABARD
    };

    uint32 id = 0;
    uint32 npcsoundsid = 0;
    uint8 race = RACE_HUMAN;
    uint8 Class = CLASS_WARRIOR;
    uint8 gender = GENDER_MALE;
    uint8 skin = 0;
    uint8 face = 0;
    uint8 hair = 0;
    uint8 haircolor = 0;
    uint8 facialhair = 0;
    uint32 guildid = 0;
    uint32 displayId = 0; // base ChrRaces display for race+gender
    uint32 outfitdisplays[EQUIPMENT_SLOT_END] = {};
};

class DressNpcMgr
{
public:
    static DressNpcMgr* instance();

    bool Enabled = true;
    bool SoundsEnabled = true;

    void LoadFromDB();
    void LoadNpcSounds();
    // Reloads the tables and re-dresses/restores creatures already in world.
    // Must run from the world-update thread (maps idle), e.g. a chat command.
    void Reload();

    // Lookups are read-only after LoadFromDB and therefore safe from
    // concurrent map-update threads. Spawn-guid assignment wins over entry.
    std::shared_ptr<CreatureOutfit> GetOutfitFor(Creature* creature) const;

    void ApplyOutfit(Creature* creature, std::shared_ptr<CreatureOutfit> const& outfit) const;

    void OnCreatureAddWorld(Creature* creature) const;
    void OnCreatureUpdate(Creature* creature) const;

    // Returns true if the request was answered (caller must block the core handler).
    bool HandleMirrorImageRequest(WorldSession* session, ObjectGuid guid) const;
    void PlayHelloSound(WorldSession* session, ObjectGuid guid) const;

private:
    DressNpcMgr() = default;

    void RefreshCreature(Creature* creature) const;

    std::unordered_map<uint32, std::shared_ptr<CreatureOutfit>> _outfits;
    std::unordered_map<uint32, std::shared_ptr<CreatureOutfit>> _outfitsByEntry;
    std::unordered_map<ObjectGuid::LowType, std::shared_ptr<CreatureOutfit>> _outfitsBySpawnId;

    DBCStorage<NPCSoundsEntry> _npcSounds{ "niiii" };
    bool _npcSoundsLoaded = false;
};

#define sDressNpcMgr DressNpcMgr::instance()

#endif
