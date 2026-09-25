Craftspeed is an AzerothCore module that globally modifies the cast speed of crafting and tradeskill spells. Any spell that creates an item and requires reagent1 > 0 (DBC Reagent[0]) will have its cast time automatically scaled by a configurable multiplier.

✨ Функции

⚡ Автоматически ускоряет все заклинания создания предметов.

Сочетается с заклинаниями:

SPELL_EFFECT_TRADE_SKILL

SPELL_EFFECT_CREATE_ITEM

SPELL_EFFECT_CREATE_ITEM_2

And Reagent[0] > 0 ((т.е. требуется реагент #1))

🔧 Применяется по всему миру при старте сервера

Не требуются SpellScripts

Не требуется настройка для каждого игрока

Не требуется настройка для каждого заклинания или во время выполнения

💡 Нулевое влияние на производительность

Однократное сканирование DBC при запуске сервера

Поиск по времени выполнения не изменился

Чистая константная модификация: эффективно и безопасно

📄 Настраиваемый множитель

Настройки в mod_crafting_casttime.conf

Примеры:

1.0 – без изменений

0.5 – в два раза быстрее

0.25 – в четыре раза быстрее

0.01 – почти мгновенно
