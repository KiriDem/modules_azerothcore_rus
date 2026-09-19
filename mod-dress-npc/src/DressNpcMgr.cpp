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
#include "Creature.h"
#include "DBCStores.h"
#include "DatabaseEnv.h"
#include "Log.h"
#include "Map.h"
#include "MapMgr.h"
#include "ObjectMgr.h"
#include "Opcodes.h"
#include "Timer.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldSession.h"

DressNpcMgr* DressNpcMgr::instance()
{
    static DressNpcMgr instance;
    return &instance;
}

void DressNpcMgr::LoadNpcSounds()
{
    std::string path = sWorld->GetDataPath() + "dbc/NPCSounds.dbc";
    _npcSoundsLoaded = _npcSounds.Load(path.c_str());

    if (_npcSoundsLoaded)
        LOG_INFO("server.loading", ">> Loaded {} NPC sounds from {}", _npcSounds.GetNumRows(), path);
    else
        LOG_WARN("server.loading", ">> Could not load {}. Dressed NPCs will have no greeting sounds.", path);
}

void DressNpcMgr::LoadFromDB()
{
    uint32 oldMSTime = getMSTime();

    _outfits.clear();
    _outfitsByEntry.clear();
    _outfitsBySpawnId.clear();

    QueryResult result = WorldDatabase.Query(
        "SELECT entry, npcsoundsid, race, class, gender, skin, face, hair, haircolor, facialhair, "
        "head, shoulders, body, chest, waist, legs, feet, wrists, hands, back, tabard, guildid "
        "FROM creature_template_outfits");

    if (!result)
    {
        LOG_INFO("server.loading", ">> Loaded 0 creature outfits. DB table `creature_template_outfits` is empty.");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        uint8 i = 0;
        std::shared_ptr<CreatureOutfit> outfit = std::make_shared<CreatureOutfit>();

        outfit->id = fields[i++].Get<uint32>();

        outfit->npcsoundsid = fields[i++].Get<uint32>();
        if (outfit->npcsoundsid && _npcSoundsLoaded && !_npcSounds.LookupEntry(outfit->npcsoundsid))
        {
            LOG_ERROR("sql.sql", "Outfit {} in `creature_template_outfits` has invalid npcsoundsid ({}). Using 0.", outfit->id, outfit->npcsoundsid);
            outfit->npcsoundsid = 0;
        }

        outfit->race = fields[i++].Get<uint8>();
        ChrRacesEntry const* raceEntry = sChrRacesStore.LookupEntry(outfit->race);
        if (!raceEntry)
        {
            LOG_ERROR("sql.sql", "Outfit {} in `creature_template_outfits` has invalid race ({}). Ignoring.", outfit->id, outfit->race);
            continue;
        }

        outfit->Class = fields[i++].Get<uint8>();
        if (!sChrClassesStore.LookupEntry(outfit->Class))
        {
            LOG_ERROR("sql.sql", "Outfit {} in `creature_template_outfits` has invalid class ({}). Ignoring.", outfit->id, outfit->Class);
            continue;
        }

        outfit->gender = fields[i++].Get<uint8>();
        switch (outfit->gender)
        {
            case GENDER_MALE:
                outfit->displayId = raceEntry->model_m;
                break;
            case GENDER_FEMALE:
                outfit->displayId = raceEntry->model_f;
                break;
            default:
                LOG_ERROR("sql.sql", "Outfit {} in `creature_template_outfits` has invalid gender ({}). Ignoring.", outfit->id, outfit->gender);
                continue;
        }

        outfit->skin = fields[i++].Get<uint8>();
        outfit->face = fields[i++].Get<uint8>();
        outfit->hair = fields[i++].Get<uint8>();
        outfit->haircolor = fields[i++].Get<uint8>();
        outfit->facialhair = fields[i++].Get<uint8>();

        for (EquipmentSlots slot : CreatureOutfit::ItemSlots)
        {
            int32 value = fields[i++].Get<int32>();
            if (value > 0) // item entry
            {
                uint32 itemEntry = static_cast<uint32>(value);
                if (ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itemEntry))
                    outfit->outfitdisplays[slot] = proto->DisplayInfoID;
                else if (ItemEntry const* dbcEntry = sItemStore.LookupEntry(itemEntry))
                    outfit->outfitdisplays[slot] = dbcEntry->DisplayInfoID;
                else
                {
                    LOG_ERROR("sql.sql", "Outfit {} in `creature_template_outfits` has invalid item entry ({}). Ignoring slot.", outfit->id, itemEntry);
                    outfit->outfitdisplays[slot] = 0;
                }
            }
            else // item displayid
                outfit->outfitdisplays[slot] = static_cast<uint32>(-value);
        }

        outfit->guildid = fields[i++].Get<uint32>();

        _outfits[outfit->id] = std::move(outfit);
        ++count;
    } while (result->NextRow());

    LOG_INFO("server.loading", ">> Loaded {} creature outfits in {} ms", count, GetMSTimeDiffToNow(oldMSTime));

    uint32 npcCount = 0;
    if (QueryResult npcs = WorldDatabase.Query("SELECT creature_entry, outfit_entry FROM creature_template_outfits_npcs"))
    {
        do
        {
            Field* fields = npcs->Fetch();
            uint32 creatureEntry = fields[0].Get<uint32>();
            uint32 outfitEntry = fields[1].Get<uint32>();

            auto itr = _outfits.find(outfitEntry);
            if (itr == _outfits.end())
            {
                LOG_ERROR("sql.sql", "`creature_template_outfits_npcs` creature {} references missing outfit {}. Ignoring.", creatureEntry, outfitEntry);
                continue;
            }

            if (!sObjectMgr->GetCreatureTemplate(creatureEntry))
            {
                LOG_ERROR("sql.sql", "`creature_template_outfits_npcs` references missing creature entry {}. Ignoring.", creatureEntry);
                continue;
            }

            _outfitsByEntry[creatureEntry] = itr->second;
            ++npcCount;
        } while (npcs->NextRow());
    }

    uint32 spawnCount = 0;
    if (QueryResult spawns = WorldDatabase.Query("SELECT spawn_guid, outfit_entry FROM creature_template_outfits_spawns"))
    {
        do
        {
            Field* fields = spawns->Fetch();
            ObjectGuid::LowType spawnId = fields[0].Get<uint32>();
            uint32 outfitEntry = fields[1].Get<uint32>();

            auto itr = _outfits.find(outfitEntry);
            if (itr == _outfits.end())
            {
                LOG_ERROR("sql.sql", "`creature_template_outfits_spawns` spawn {} references missing outfit {}. Ignoring.", spawnId, outfitEntry);
                continue;
            }

            _outfitsBySpawnId[spawnId] = itr->second;
            ++spawnCount;
        } while (spawns->NextRow());
    }

    LOG_INFO("server.loading", ">> Loaded {} outfit npc assignments and {} spawn assignments", npcCount, spawnCount);
}

std::shared_ptr<CreatureOutfit> DressNpcMgr::GetOutfitFor(Creature* creature) const
{
    if (ObjectGuid::LowType spawnId = creature->GetSpawnId())
    {
        auto itr = _outfitsBySpawnId.find(spawnId);
        if (itr != _outfitsBySpawnId.end())
            return itr->second;
    }

    auto itr = _outfitsByEntry.find(creature->GetEntry());
    if (itr != _outfitsByEntry.end())
        return itr->second;

    return nullptr;
}

void DressNpcMgr::ApplyOutfit(Creature* creature, std::shared_ptr<CreatureOutfit> const& outfit) const
{
    creature->SetDisplayId(outfit->displayId);
    creature->SetNativeDisplayId(outfit->displayId);
    creature->SetUnitFlag2(UNIT_FLAG2_MIRROR_IMAGE);
}

void DressNpcMgr::OnCreatureAddWorld(Creature* creature) const
{
    if (std::shared_ptr<CreatureOutfit> const& outfit = GetOutfitFor(creature))
        ApplyOutfit(creature, outfit);
}

void DressNpcMgr::OnCreatureUpdate(Creature* creature) const
{
    std::shared_ptr<CreatureOutfit> const& outfit = GetOutfitFor(creature);
    if (!outfit)
        return;

    uint32 display = creature->GetDisplayId();
    if (display == outfit->displayId)
    {
        // Creature::UpdateEntry rewrites UNIT_FIELD_FLAGS_2 from the template
        if (!creature->HasUnitFlag2(UNIT_FLAG2_MIRROR_IMAGE))
            creature->SetUnitFlag2(UNIT_FLAG2_MIRROR_IMAGE);
        return;
    }

    // mid re-dress: a delayed event from RefreshCreature will apply the outfit
    if (display == CreatureOutfit::INVISIBLE_MODEL)
        return;

    // the core reset the display to a creature_template model (respawn, entry update)
    if (creature->GetCreatureTemplate()->GetModelWithDisplayId(display))
        ApplyOutfit(creature, outfit);

    // any other display is an intentional morph (scripts, spells): leave it alone
}

bool DressNpcMgr::HandleMirrorImageRequest(WorldSession* session, ObjectGuid guid) const
{
    Player* player = session->GetPlayer();
    if (!player || !player->IsInWorld())
        return false;

    if (!guid.IsAnyTypeCreature())
        return false;

    Creature* creature = player->GetMap()->GetCreature(guid);
    if (!creature)
        return false;

    std::shared_ptr<CreatureOutfit> const& outfit = GetOutfitFor(creature);
    if (!outfit)
        return false;

    WorldPacket data(SMSG_MIRRORIMAGE_DATA, 68);
    data << guid;
    data << uint32(outfit->displayId);
    data << uint8(outfit->race);
    data << uint8(outfit->gender);
    data << uint8(outfit->Class);
    data << uint8(outfit->skin);
    data << uint8(outfit->face);
    data << uint8(outfit->hair);
    data << uint8(outfit->haircolor);
    data << uint8(outfit->facialhair);
    data << uint32(outfit->guildid);

    for (EquipmentSlots slot : CreatureOutfit::ItemSlots)
        data << uint32(outfit->outfitdisplays[slot]);

    session->SendPacket(&data);
    return true;
}

void DressNpcMgr::PlayHelloSound(WorldSession* session, ObjectGuid guid) const
{
    if (!SoundsEnabled || !_npcSoundsLoaded)
        return;

    Player* player = session->GetPlayer();
    if (!player || !player->IsInWorld())
        return;

    if (!guid.IsAnyTypeCreature())
        return;

    Creature* creature = player->GetMap()->GetCreature(guid);
    if (!creature)
        return;

    std::shared_ptr<CreatureOutfit> const& outfit = GetOutfitFor(creature);
    if (!outfit || !outfit->npcsoundsid)
        return;

    if (NPCSoundsEntry const* sounds = _npcSounds.LookupEntry(outfit->npcsoundsid))
        if (sounds->hello)
            creature->PlayDistanceSound(sounds->hello, player);
}

void DressNpcMgr::RefreshCreature(Creature* creature) const
{
    if (std::shared_ptr<CreatureOutfit> const& outfit = GetOutfitFor(creature))
    {
        // The client caches the rendered look per displayid: pass through an
        // invisible model so it re-requests the mirror image data.
        creature->SetDisplayId(CreatureOutfit::INVISIBLE_MODEL);
        creature->m_Events.AddEventAtOffset([creature]()
        {
            if (std::shared_ptr<CreatureOutfit> const& outfit = sDressNpcMgr->GetOutfitFor(creature))
                sDressNpcMgr->ApplyOutfit(creature, outfit);
        }, 500ms);
    }
    else if (creature->HasUnitFlag2(UNIT_FLAG2_MIRROR_IMAGE) && !(creature->GetCreatureTemplate()->unit_flags2 & UNIT_FLAG2_MIRROR_IMAGE))
    {
        // outfit assignment was removed: restore the template model
        creature->RemoveUnitFlag2(UNIT_FLAG2_MIRROR_IMAGE);
        if (CreatureModel const* model = creature->GetCreatureTemplate()->GetRandomValidModel())
        {
            creature->SetDisplayId(model->CreatureDisplayID);
            creature->SetNativeDisplayId(model->CreatureDisplayID);
        }
    }
}

void DressNpcMgr::Reload()
{
    LoadFromDB();

    sMapMgr->DoForAllMaps([this](Map* map)
    {
        for (auto const& pair : map->GetCreatureBySpawnIdStore())
            RefreshCreature(pair.second);
    });
}
