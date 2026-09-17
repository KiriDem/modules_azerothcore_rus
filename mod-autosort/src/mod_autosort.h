/*
 * Credits: silviu20092
 */

#ifndef _MOD_AUTOSORT_H_
#define _MOD_AUTOSORT_H_

#include "Player.h"
#include "Item.h"
#include "mod_autosort_config.h"

#include <algorithm>
#include <vector>
#include <unordered_map>

class ModAutosort
{
private:
    ModAutosort();
    ~ModAutosort();
private:
    using SlotPos = std::pair<uint8, uint8>;

    struct SortScope
    {
        std::vector<SlotPos> mainPool;
        std::vector<std::vector<SlotPos>> specialPools;
        std::vector<SlotPos> allPositions;
    };

    std::unordered_map<ObjectGuid, time_t> lastSortTime;

    ModAutosortConfig autosortConfig;
    std::unordered_map<uint32, uint32> pinnedPriorityMap;

    static uint16 MakePos(uint8 bag, uint8 slot);
    static int32 BagOrderKey(uint8 bagSlot);
    static bool IsNormalBag(Bag* bag);

    void BuildPinnedPriorityMap();

    int32 GetClassSortWeight(uint32 itemClass) const;
    bool CompareItems(Item* a, Item* b);
    void MergeStacksInSlots(Player* player, std::vector<SlotPos> const& positions);
    void SortPositions(Player* player, std::vector<SlotPos> const& orderedPositions);

    SortScope BuildScope(Player* player, uint8 itemStart, uint8 itemEnd, uint8 bagStart, uint8 bagEnd);
    SortScope BuildBagScope(Player* player);
    SortScope BuildBankScope(Player* player);
    void RunScope(Player* player, SortScope const& scope, bool merge, bool sort);
public:
    static ModAutosort* instance();

    void LoadConfig(bool reload);

    bool IsEnabled() const;
    bool MergeStacks() const;
    bool SortEnabled() const;
    bool SortOnLogin() const;
    uint32 SortCooldown() const;
    std::string GetPinnedItems() const;
    bool IsPeriodicEnabled() const;
    uint32 PeriodicInterval() const;
    bool SkipPeriodicIfLooting() const;
    bool SkipPeriodicIfInCombat() const;
    bool SortBankOnPeriodic() const;
    std::string PeriodicSortMessage() const;

    bool IsOnCooldown(const Player* player) const;
    void RefreshCooldown(Player* player);

    void DoSort(Player* player, bool doBags, bool doBank);

    bool ShouldSkipPeriodicSort(Player* player) const;
};

#define sModAutosort ModAutosort::instance()

#endif // _MOD_AUTOSORT_H_
