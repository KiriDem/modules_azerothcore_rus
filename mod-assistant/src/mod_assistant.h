#ifndef MOD_ASSISTANT_H
#define MOD_ASSISTANT_H

#include "Chat.h"
#include "Config.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"

enum
{
    ASSISTANT_GOSSIP_HEIRLOOM        = 100,
    ASSISTANT_GOSSIP_GLYPH           = 200,
    ASSISTANT_GOSSIP_GEM             = 400,
    ASSISTANT_GOSSIP_ELIXIRS         = 500,
    ASSISTANT_GOSSIP_FOOD            = 600,
    ASSISTANT_GOSSIP_ENCHANTS        = 700,
    ASSISTANT_GOSSIP_CONTAINER       = 800,
    ASSISTANT_GOSSIP_FLIGHT_PATHS    = 900,
    ASSISTANT_GOSSIP_UTILITIES       = 1000,
    ASSISTANT_GOSSIP_PROFESSIONS     = 1100,
    ASSISTANT_GOSSIP_INSTANCES       = 1200,

    ASSISTANT_GOSSIP_TEXT            = 48,

    ASSISTANT_VENDOR_HEIRLOOM_WEAPON = 9000000,
    ASSISTANT_VENDOR_HEIRLOOM_ARMOR  = 9000001,
    ASSISTANT_VENDOR_HEIRLOOM_OTHER  = 9000002,
    ASSISTANT_VENDOR_GLYPH           = 9000003,
    ASSISTANT_VENDOR_GEM             = 9000023,
    ASSISTANT_VENDOR_ELIXIR          = 9000030,
    ASSISTANT_VENDOR_FOOD            = 9000033,
    ASSISTANT_VENDOR_ENCHANT         = 9000034,
    ASSISTANT_VENDOR_CONTAINER       = 9000045,

    PROFESSION_LEVEL_APPRENTICE      = 75,
    PROFESSION_LEVEL_JOURNEYMAN      = 150,
    PROFESSION_LEVEL_EXPERT          = 225,
    PROFESSION_LEVEL_ARTISAN         = 300,
    PROFESSION_LEVEL_MASTER          = 375,
    PROFESSION_LEVEL_GRAND_MASTER    = 450,

    INSTANCE_TYPE_HEROIC             = 0,
    INSTANCE_TYPE_RAID               = 1,

    GLYPH_WARRIOR                    = 0,
    GLYPH_PALADIN                    = 1,
    GLYPH_HUNTER                     = 2,
    GLYPH_ROGUE                      = 3,
    GLYPH_PRIEST                     = 4,
    GLYPH_DEATH_KNIGHT               = 5,
    GLYPH_SHAMAN                     = 6,
    GLYPH_MAGE                       = 7,
    GLYPH_WARLOCK                    = 8,
    GLYPH_DRUID                      = 9
};

#define GOSSIP_HEIRLOOMS "Мне нужны фамильные предметы"
#define GOSSIP_HEIRLOOMS_WEAPONS "Мне нужно оружие"
#define GOSSIP_HEIRLOOMS_ARMOR "Мне нужна броня"
#define GOSSIP_HEIRLOOMS_OTHER "Мне нужно что-то другое"

#define GOSSIP_GLYPHS "Мне нужны символы"
#define GOSSIP_GLYPHS_MAJOR "Мне нужны большие символы"
#define GOSSIP_GLYPHS_MINOR "Мне нужны малые символы"

#define GOSSIP_GEMS "Мне нужны камни"
#define GOSSIP_GEMS_META "Мне нужны особые камни"
#define GOSSIP_GEMS_RED "Мне нужны красные камни"
#define GOSSIP_GEMS_BLUE "Мне нужны синие камни"
#define GOSSIP_GEMS_YELLOW "Мне нужны желтые камни"
#define GOSSIP_GEMS_PURPLE "Мне нужны фиолетовые камни"
#define GOSSIP_GEMS_GREEN "Мне нужны зеленые камни"
#define GOSSIP_GEMS_ORANGE "Мне нужны оранжевые камни"

#define GOSSIP_ELIXIRS "Мне нужны эликсиры"
#define GOSSIP_ELIXIRS_BATTLE "Мне нужны боевые эликсиры"
#define GOSSIP_ELIXIRS_GUARDIAN "Мне нужны эликсиры стража"
#define GOSSIP_ELIXIRS_FLASK "Мне нужны настои"

#define GOSSIP_FOOD "Мне нужна еда"

#define GOSSIP_ENCHANTS "Мне нужны чары"
#define GOSSIP_ENCHANTS_WEAPON "Мне нужны чары для оружия"
#define GOSSIP_ENCHANTS_HEAD "Мне нужны чары для головы"
#define GOSSIP_ENCHANTS_SHOULDER "Мне нужны чары для плеч"
#define GOSSIP_ENCHANTS_CHEST "Мне нужны чары для нагрудника"
#define GOSSIP_ENCHANTS_BRACER "Хочу чары для наручей"
#define GOSSIP_ENCHANTS_GLOVES "Хочу чары для перчаток"
#define GOSSIP_ENCHANTS_WAIST "Хочу чары для пояса"
#define GOSSIP_ENCHANTS_LEGS "Хочу чары для поножей"
#define GOSSIP_ENCHANTS_FEET "Хочу чары для сапог"
#define GOSSIP_ENCHANTS_CLOAK "Хочу чары для плаща"
#define GOSSIP_ENCHANTS_SHIELD "Хочу чары для щита"

#define GOSSIP_CONTAINERS "Хочу сумку"

#define GOSSIP_UTILITIES "Полезные функции"
#define GOSSIP_UTILITIES_NAME "Хочу сменить имя"
#define GOSSIP_UTILITIES_APPEARANCE "Хочу изменить внешность"
#define GOSSIP_UTILITIES_RACE "Хочу сменить расу"
#define GOSSIP_UTILITIES_FACTION "Хочу сменить фракцию"
#define GOSSIP_UTILITIES_IN_PROGRESS "Необходимо завершить выполнение ранее активированной функции, прежде чем переходить к другой."
#define GOSSIP_UTILITIES_DONE "Теперь вы можете выйти из игры, чтобы продолжить использование активированной функции."
#define GOSSIP_FLIGHT_PATHS "Я хочу открыть маршруты полетов"
#define GOSSIP_FLIGHT_PATHS_KALIMDOR_EASTERN_KINGDOMS "Калимдор и Восточные королевства"
#define GOSSIP_FLIGHT_PATHS_OUTLAND "Запределье"
#define GOSSIP_FLIGHT_PATHS_NORTHREND "Нордскол"

#define GOSSIP_PROFESSIONS "Мне нужна помощь с профессиями"
#define GOSSIP_PROFESSIONS_CHOOSE "Мне нужна помощь с навыком..."
#define GOSSIP_PROFESSIONS_FIRST_AID "Первая помощь"
#define GOSSIP_PROFESSIONS_BLACKSMITHING "Кузнечное дело"
#define GOSSIP_PROFESSIONS_LEATHERWORKING "Кожевничество"
#define GOSSIP_PROFESSIONS_ALCHEMY "Алхимия"
#define GOSSIP_PROFESSIONS_HERBALISM "Травничество"
#define GOSSIP_PROFESSIONS_COOKING "Кулинария"
#define GOSSIP_PROFESSIONS_MINING "Горное дело"
#define GOSSIP_PROFESSIONS_TAILORING "Портняжное дело"
#define GOSSIP_PROFESSIONS_ENGINEERING "Инженерное дело"
#define GOSSIP_PROFESSIONS_ENCHANTING "Наложение чар"
#define GOSSIP_PROFESSIONS_FISHING "Рыбная ловля"
#define GOSSIP_PROFESSIONS_SKINNING "Снятие шкур"
#define GOSSIP_PROFESSIONS_INSCRIPTION "Начертание"
#define GOSSIP_PROFESSIONS_JEWELCRAFTING "Ювелирное дело"

#define GOSSIP_INSTANCES "Я хочу сбросить подземелья"
#define GOSSIP_INSTANCES_HEROIC "Я хочу сбросить героические подземелья"
#define GOSSIP_INSTANCES_RAID "Я хочу сбросить рейды"
#define GOSSIP_INSTANCES_PLAYER "Только для меня"
#define GOSSIP_INSTANCES_GROUP "Для всей моей группы"
#define GOSSIP_INSTANCES_HEROIC_RESET "Все героические подземелья сброшены." #define GOSSIP_INSTANCES_HEROIC_GROUP_RESET "Героические подземелья вашей группы были сброшены."
#define GOSSIP_INSTANCES_RAID_RESET "Все рейды были сброшены."
#define GOSSIP_INSTANCES_RAID_GROUP_RESET "Рейды вашей группы были сброшены."

#define GOSSIP_CONTINUE_TRANSACTION "Вы хотите продолжить транзакцию?"
#define GOSSIP_PREVIOUS_PAGE "Предыдущая страница"

class Assistant : public CreatureScript, WorldScript
{
public:
    Assistant();

    // CreatureScript
    bool OnGossipHello(Player* /*player*/, Creature* /*creature*/) override;
    bool OnGossipSelect(Player* /*player*/, Creature* /*creature*/, uint32 /*sender*/, uint32 /*action*/) override;

    // WorldScript
    void OnAfterConfigLoad(bool /*reload*/) override;

private:
    bool HeirloomsEnabled;
    bool GlyphsEnabled;
    bool GemsEnabled;
    bool ElixirsEnabled;
    bool FoodEnabled;
    bool EnchantsEnabled;
    bool ContainersEnabled;

    uint32 GetGlyphId(uint32 /*id*/, bool /*major*/);

    // Utilities
    bool UtilitiesEnabled;
    uint32 NameChangeCost;
    uint32 CustomizeCost;
    uint32 RaceChangeCost;
    uint32 FactionChangeCost;

    bool HasLoginFlag(Player* /*player*/);
    void SetLoginFlag(Player* /*player*/, AtLoginFlags /*flag*/, uint32 /*cost*/);

    // Flight Paths
    bool FlightPathsEnabled[EXPANSION_WRATH_OF_THE_LICH_KING + 1];
    uint32 FlightPathsRequiredLevel[EXPANSION_WRATH_OF_THE_LICH_KING + 1];
    uint32 FlightPathsCost[EXPANSION_WRATH_OF_THE_LICH_KING + 1];

    bool CanUnlockFlightPaths(Player* /*player*/);
    std::vector<int> GetAvailableFlightPaths(Player* /*player*/, uint8 /*expansion*/);
    bool HasAvailableFlightPaths(Player* /*player*/, uint8 /*expansion*/);
    void UnlockFlightPaths(Player* /*player*/, uint8 /*expansion*/);

    // Professions
    bool ApprenticeProfessionEnabled;
    uint32 ApprenticeProfessionCost;
    bool JourneymanProfessionEnabled;
    uint32 JourneymanProfessionCost;
    bool ExpertProfessionEnabled;
    uint32 ExpertProfessionCost;
    bool ArtisanProfessionEnabled;
    uint32 ArtisanProfessionCost;
    bool MasterProfessionEnabled;
    uint32 MasterProfessionCost;
    bool GrandMasterProfessionEnabled;
    uint32 GrandMasterProfessionCost;

    void ListProfession(Player* /*player*/, uint32 /*id*/);
    void SetProfession(Player* /*player*/, uint32 /*id*/);
    bool HasValidProfession(Player* /*player*/);
    bool IsValidProfession(Player* /*player*/, uint32 /*id*/);
    uint32 GetProfessionCost(Player* /*player*/, uint32 /*id*/);

    // Instances
    bool HeroicInstanceEnabled;
    uint32 HeroicInstanceCost;
    bool RaidInstanceEnabled;
    uint32 RaidInstanceCost;

    bool CanResetInstances(Player* /*player*/);
    bool HasSavedInstances(Player* /*player*/, uint8 /*type*/);
    void ResetInstances(Player* /*player*/, uint8 /*type*/);
};

#endif
