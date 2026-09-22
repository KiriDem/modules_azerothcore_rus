# ![logo](https://raw.githubusercontent.com/azerothcore/azerothcore.github.io/master/images/logo-github.png) AzerothCore

## mod-solocraft

- Корректирует характеристики игроков в рейдах в зависимости от количества участников группы.
- Настраиваемый эффект ослабления (дебафф) для групп, пытающихся чрезмерно повысить модификатор сложности подземелья.
- Возможность настройки модификатора характеристик в файле конфигурации.
- Конфигурация: настройки сложности для каждого подземелья и его типа.
- Добавлен бонус к силе заклинаний (с настраиваемым коэффициентом).
- Сохранение настроек модификаторов в базе данных.
- Возможность установки пороговых значений максимального уровня, чтобы не усиливать игроков, чей уровень значительно превышает уровень подземелья.

## Использование в игре

1. Включите модуль в файле конфигурации.
2. Настройте нужные функции (включите или измените их параметры) в файле конфигурации.
3. Войдите в подземелье или рейд.

## Требования

- AzerothCore v1.0.1+

## Установка

```
1) Просто поместите модуль в директорию `modules` исходного кода AzerothCore.
2) Импортируйте SQL-файл в соответствующую базу данных (character).
3) Повторно запустите cmake и выполните чистую сборку AzerothCore.
```

## Изменение конфигурации модуля (необязательно)

Если вам нужно изменить конфигурацию модуля, перейдите в папку с конфигурацией сервера (туда, где находится `worldserver` или `worldserver.exe`), скопируйте файл `Solocraft.conf.dist` в `Solocraft.conf` и отредактируйте этот новый файл.

### Данные ###

------------------------------------------------------------------------------------------------------------------

- Тип: Сервер/Игрок
- Скрипт: Solocraft
- Конфигурация: Да
- SQL: Да

### Авторы ###

------------------------------------------------------------------------------------------------------------------
- [DavidMacalaster](https://github.com/DavidMacalaster/Solocraft)
- [Blizzard Entertainment](http://blizzard.com)
- [TrinityCore](https://github.com/TrinityCore/TrinityCore/blob/3.3.5/THANKS)
- [SunwellCore](http://www.azerothcore.org/pages/sunwell.pl/)
- [EMUDevs](https://youtube.com/user/EmuDevs)
- [AC-Web](http://ac-web.org/)
- [ModCraft.io](http://modcraft.io/)
- [OwnedCore](http://ownedcore.com/)
- [OregonCore](https://wiki.oregon-core.net/)
- [Wowhead.com](http://wowhead.com)
- [AoWoW](https://wotlk.evowow.com/)
- AzerothCore: [repository](https://github.com/azerothcore) - [website](http://azerothcore.org/) - [discord chat community](https://discord.gg/PaqQRkd)
