/*
 * Credits: silviu20092
 */

#include "Config.h"
#include "Tokenize.h"
#include "StringConvert.h"
#include "mod_autosort.h"

#ifdef MOD_PLAYERBOTS
#include "Playerbots.h"
#endif

ModAutosort::ModAutosort()
{
}

ModAutosort::~ModAutosort()
{
}

ModAutosort* ModAutosort::instance()
{
    static ModAutosort instance;
    return &instance;
}

// this is the expected format for SwapItem
/*static*/ uint16 ModAutosort::MakePos(uint8 bag, uint8 slot)
{
    return uint16(bag) << 8 | slot;
}

/*static*/ int32 ModAutosort::BagOrderKey(uint8 bagSlot)
{
    // special case for Backpack, this is always the first bag
    if (bagSlot == uint8(INVENTORY_SLOT_BAG_0))
        return -1;

    return int32(bagSlot);
}

/*static*/ bool ModAutosort::IsNormalBag(Bag* bag)
{
    ItemTemplate const* proto = bag->GetTemplate();
    return proto && proto->Class == ITEM_CLASS_CONTAINER && proto->SubClass == ITEM_SUBCLASS_CONTAINER;
}

void ModAutosort::LoadConfig(bool /*reload*/)
{
    autosortConfig.Initialize();

    BuildPinnedPriorityMap();
}

// master enable switch for the whole module
bool ModAutosort::IsEnabled() const
{
    return autosortConfig.GetBoolConfig(AUTOSORT_CONFIG_IS_ENABLED);
}

// whether stackable items will automatically be stacked to max stack(s)
bool ModAutosort::MergeStacks() const
{
    return autosortConfig.GetBoolConfig(AUTOSORT_CONFIG_MERGE_STACKS);
}

// whether items will automatically be sorted in bags (retail-like sort functionality)
bool ModAutosort::SortEnabled() const
{
    return autosortConfig.GetBoolConfig(AUTOSORT_CONFIG_SORT_ENABLED);
}

// automatically sort bags when player logs in
bool ModAutosort::SortOnLogin() const
{
    return autosortConfig.GetBoolConfig(AUTOSORT_CONFIG_SORT_ON_LOGIN);
}

// cooldown (in seconds) for the sort commands, this prevents players from spamming
uint32 ModAutosort::SortCooldown() const
{
    return (uint32)autosortConfig.GetIntConfig(AUTOSORT_CONFIG_COOLDOWN);
}

std::string ModAutosort::GetPinnedItems() const
{
    return autosortConfig.GetStringConfig(AUTOSORT_CONFIG_PINNED_ITEMS);
}

bool ModAutosort::IsPeriodicEnabled() const
{
    return autosortConfig.GetBoolConfig(AUTOSORT_CONFIG_IS_PERIODIC_ENABLED);
}

uint32 ModAutosort::PeriodicInterval() const
{
    return (uint32)autosortConfig.GetIntConfig(AUTOSORT_CONFIG_PERIODIC_INTERVAL);
}

bool ModAutosort::SkipPeriodicIfLooting() const
{
    return autosortConfig.GetBoolConfig(AUTOSORT_CONFIG_SKIP_PERIODIC_IF_LOOTING);
}

bool ModAutosort::SkipPeriodicIfInCombat() const
{
    return autosortConfig.GetBoolConfig(AUTOSORT_CONFIG_SKIP_PERIODIC_IF_IN_COMBAT);
}

bool ModAutosort::SortBankOnPeriodic() const
{
    return autosortConfig.GetBoolConfig(AUTOSORT_CONFIG_SORT_BANK_ON_PERIODIC);
}

std::string ModAutosort::PeriodicSortMessage() const
{
    return autosortConfig.GetStringConfig(AUTOSORT_CONFIG_PERIODIC_SORT_MESSAGE);
}

bool ModAutosort::IsOnCooldown(const Player* player) const
{
    uint32 cooldown = SortCooldown();
    if (cooldown == 0)
        return false;

    auto it = lastSortTime.find(player->GetGUID());
    if (it == lastSortTime.end())
        return false;

    return (time(nullptr) - it->second) < time_t(cooldown);
}

void ModAutosort::RefreshCooldown(Player* player)
{
    lastSortTime[player->GetGUID()] = time(nullptr);
}

void ModAutosort::BuildPinnedPriorityMap()
{
    pinnedPriorityMap.clear();

    std::string raw = GetPinnedItems();
    std::vector<std::string_view> tokenized = Acore::Tokenize(raw, ',', false);

    uint32 priority = 0;
    for (const auto& token : tokenized)
    {
        uint32 entry = *Acore::StringTo<uint32>(Acore::String::Trim(std::string(token), std::locale()));
        if (pinnedPriorityMap.find(entry) == pinnedPriorityMap.end())
            pinnedPriorityMap[entry] = priority++;
    }
}

int32 ModAutosort::GetClassSortWeight(uint32 itemClass) const
{
    switch (itemClass)
    {
        case ITEM_CLASS_WEAPON:      return 0;
        case ITEM_CLASS_ARMOR:       return 1;
        case ITEM_CLASS_GEM:         return 2;
        case ITEM_CLASS_CONSUMABLE:  return 3;
        case ITEM_CLASS_TRADE_GOODS: return 4;
        case ITEM_CLASS_RECIPE:      return 5;
        case ITEM_CLASS_QUEST:       return 6;
        case ITEM_CLASS_CONTAINER:   return 7;
        case ITEM_CLASS_QUIVER:      return 8;
        case ITEM_CLASS_KEY:         return 9;
        case ITEM_CLASS_GLYPH:       return 10;
        case ITEM_CLASS_MISC:        return 11;
        default:                     return 99;
    }
}

bool ModAutosort::CompareItems(Item* a, Item* b)
{
    ItemTemplate const* ta = a->GetTemplate();
    ItemTemplate const* tb = b->GetTemplate();

    if (!ta || !tb)
        return false;

    int32 wa = GetClassSortWeight(ta->Class);
    int32 wb = GetClassSortWeight(tb->Class);
    if (wa != wb)
        return wa < wb;

    if (ta->SubClass != tb->SubClass)
        return ta->SubClass < tb->SubClass;

    // higher quality first
    if (ta->Quality != tb->Quality)
        return ta->Quality > tb->Quality;

    // higher ilvl first
    if (ta->ItemLevel != tb->ItemLevel)
        return ta->ItemLevel > tb->ItemLevel;

    // alphabetical
    if (ta->Name1 != tb->Name1)
        return ta->Name1 < tb->Name1;

    // sort by stack size, so the smaller stacks are always placed at the end
    if (a->GetEntry() == b->GetEntry() && a->GetCount() != b->GetCount())
        return a->GetCount() > b->GetCount();

    // default sort so exact items won't be touched (no flicker when sorting)
    return a->GetGUID().GetCounter() < b->GetGUID().GetCounter();
}

void ModAutosort::MergeStacksInSlots(Player* player, std::vector<SlotPos> const& positions)
{
    std::unordered_map<uint64, std::vector<Item*>> groups;

    for (SlotPos const& pos : positions)
    {
        Item* item = player->GetItemByPos(pos.first, pos.second);
        if (!item)
            continue;

        ItemTemplate const* proto = item->GetTemplate();
        if (!proto || proto->Stackable <= 1)
            continue;

        int32 randomProp = item->GetItemRandomPropertyId();
        uint64 key = (uint64(proto->ItemId) << 32) ^ uint64(uint32(randomProp));
        groups[key].push_back(item);
    }

    for (auto& kv : groups)
    {
        std::vector<Item*>& items = kv.second;
        if (items.size() < 2)
            continue;

        std::sort(items.begin(), items.end(), [](Item* a, Item* b)
        {
            int32 ka = BagOrderKey(a->GetBagSlot());
            int32 kb = BagOrderKey(b->GetBagSlot());
            if (ka != kb)
                return ka < kb;

            return a->GetSlot() < b->GetSlot();
        });

        uint32 maxStack = items[0]->GetTemplate()->Stackable;

        uint32 total = 0;
        for (Item* it : items)
            total += it->GetCount();

        uint32 remaining = total;
        for (Item* it : items)
        {
            uint32 assign = std::min<uint32>(maxStack, remaining);
            remaining -= assign;

            if (assign == 0)
            {
                player->DestroyItem(it->GetBagSlot(), it->GetSlot(), true);
            }
            else if (assign != it->GetCount())
            {
                it->SetCount(assign);
                it->SetState(ITEM_CHANGED, player);

                if (player->IsInWorld())
                    it->SendUpdateToPlayer(player);
            }
        }
    }
}

void ModAutosort::SortPositions(Player* player, std::vector<SlotPos> const& orderedPositions)
{
    std::vector<Item*> items;
    items.reserve(orderedPositions.size());

    std::unordered_map<uint64, uint16> currentPosOf;

    for (SlotPos const& pos : orderedPositions)
    {
        Item* item = player->GetItemByPos(pos.first, pos.second);
        if (item)
        {
            items.push_back(item);
            currentPosOf[item->GetGUID().GetRawValue()] = MakePos(pos.first, pos.second);
        }
    }

    if (items.size() < 2)
        return;

    std::sort(items.begin(), items.end(), [this](Item* a, Item* b)
    {
        auto pa = pinnedPriorityMap.find(a->GetEntry());
        auto pb = pinnedPriorityMap.find(b->GetEntry());
        bool aPinned = pa != pinnedPriorityMap.end();
        bool bPinned = pb != pinnedPriorityMap.end();

        if (aPinned != bPinned)
            return aPinned;

        if (aPinned && bPinned && pa->second != pb->second)
            return pa->second < pb->second;

        return CompareItems(a, b);
    });

    for (size_t i = 0; i < items.size(); ++i)
    {
        SlotPos const& targetPos = orderedPositions[i];
        uint16 targetPacked = MakePos(targetPos.first, targetPos.second);

        Item* desired = items[i];
        uint64 guidKey = desired->GetGUID().GetRawValue();
        uint16 curPacked = currentPosOf[guidKey];

        if (curPacked == targetPacked)
            continue;

        Item* occupant = player->GetItemByPos(targetPos.first, targetPos.second);

        player->SwapItem(curPacked, targetPacked);

        currentPosOf[guidKey] = targetPacked;
        if (occupant)
            currentPosOf[occupant->GetGUID().GetRawValue()] = curPacked;
    }
}

ModAutosort::SortScope ModAutosort::BuildScope(Player* player, uint8 itemStart, uint8 itemEnd, uint8 bagStart, uint8 bagEnd)
{
    SortScope scope;

    for (uint8 slot = itemStart; slot < itemEnd; ++slot)
    {
        scope.mainPool.emplace_back(uint8(INVENTORY_SLOT_BAG_0), slot);
        scope.allPositions.emplace_back(uint8(INVENTORY_SLOT_BAG_0), slot);
    }

    for (uint8 bagSlot = bagStart; bagSlot < bagEnd; ++bagSlot)
    {
        Bag* bag = player->GetBagByPos(bagSlot);
        if (!bag)
            continue;

        std::vector<SlotPos> slots;
        for (uint32 s = 0; s < bag->GetBagSize(); ++s)
        {
            slots.emplace_back(bagSlot, uint8(s));
            scope.allPositions.emplace_back(bagSlot, uint8(s));
        }

        if (IsNormalBag(bag))
            scope.mainPool.insert(scope.mainPool.end(), slots.begin(), slots.end());
        else
            scope.specialPools.push_back(std::move(slots));
    }

    return scope;
}

ModAutosort::SortScope ModAutosort::BuildBagScope(Player* player)
{
    return BuildScope(player, INVENTORY_SLOT_ITEM_START, INVENTORY_SLOT_ITEM_END,
        INVENTORY_SLOT_BAG_START, INVENTORY_SLOT_BAG_END);
}

ModAutosort::SortScope ModAutosort::BuildBankScope(Player* player)
{
    return BuildScope(player, BANK_SLOT_ITEM_START, BANK_SLOT_ITEM_END,
        BANK_SLOT_BAG_START, BANK_SLOT_BAG_END);
}

void ModAutosort::RunScope(Player* player, SortScope const& scope, bool merge, bool sort)
{
    if (merge)
        MergeStacksInSlots(player, scope.allPositions);

    if (sort)
    {
        SortPositions(player, scope.mainPool);
        for (std::vector<SlotPos> const& pool : scope.specialPools)
            SortPositions(player, pool);
    }
}

void ModAutosort::DoSort(Player* player, bool doBags, bool doBank)
{
    if (!player)
        return;

    bool mergeStacks = MergeStacks();
    bool sortEnabled = SortEnabled();

    if (!mergeStacks && !sortEnabled)
        return;

    if (doBags)
        RunScope(player, BuildBagScope(player), mergeStacks, sortEnabled);

    if (doBank)
        RunScope(player, BuildBankScope(player), mergeStacks, sortEnabled);
}

// check if player is a playerbot, in which case we don't need to sort its bags
bool ModAutosort::ShouldSkipPeriodicSort(Player* player) const
{
#ifndef MOD_PLAYERBOTS
    return false;
#else
    PlayerbotAI* playerAI = GET_PLAYERBOT_AI(player);
    return playerAI != nullptr && !IsSelfBot(player);
#endif
}
