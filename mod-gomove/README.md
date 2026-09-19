# Краткий обзор

Название:   GO-Move (с интеграцией Objscale)
Что делает: Этот модуль позволяет добавлять, удалять, перемещать, копировать, поворачивать и масштабировать игровые объекты (GameObjects) с помощью внутриигрового аддона и графического интерфейса (GUI).
Автор:   Tralenor
Репозиторий:   https://github.com/Tralenor/mod-gomove
Скачать оригинал:   https://github.com/Tralenor/mod-gomove/archive/refs/heads/main.zip
Лицензия:   MIT

# GO Move (с интеграцией Objscale)

## Описание

Этот модуль позволяет использовать внутриигровой аддон (доступный в репозитории) для более удобного добавления, удаления, перемещения, копирования и поворота игровых объектов (GameObject). (Смотрите [ видео от Rochet2's](https://youtu.be/dqbUQrvIlcA?list=PL5CF7437D71BB0795) для примера или получения дополнительной информации)
Его также можно использовать для изменения масштаба или размера игровых объектов (GameObjects), однако для постоянного сохранения этих изменений требуется модуль [Objscale Module](https://github.com/Tralenor/mod-objscale). 

Это порт патча ядра TC-Corepatch от [Rochet2](https://github.com/Rochet2): https://github.com/Rochet2/TrinityCore/tree/gomove_3.3.5/src/server/scripts/Custom/GOMove
Обратите внимание: [GOCommand](https://github.com/Rochet2/GOCommand) от Rochet2 — это Lua-аддон, предоставляющий практически тот же функционал, что и упомянутый выше патч ядра и данный модуль, однако он зависит от функции TrinityCore под названием «протокол аддонов» (addon protocol), которая в данный момент не поддерживается в AzerothCore.
Этот протокол является частью изменений из [TrinityCore/TrinityCore#20074](https://github.com/TrinityCore/TrinityCore/pull/20074); если в будущем эти изменения будут перенесены в AzerothCore, данный модуль утратит актуальность, и я настоятельно рекомендую перейти на использование исключительно Lua-аддона.

## Как использовать в игре

Поместите папку GOMove Addon (находящуюся в этом репозитории по пути `client-addon`) в папку `Interface/Addon/` вашего клиента.
На экране выбора персонажа убедитесь, что аддон активирован.
Если интерфейс аддона не появился автоматически, вы можете сбросить его настройки (и тем самым открыть его) с помощью следующей команды:

```
/gomove reset
```

Подойдите к любому игровому объекту (GameObject) — найти их можно с помощью кнопки поиска. Теперь все объекты поблизости должны отобразиться в вашем списке.
Все объекты из списка можно выбирать и отменять выбор, а также перемещать, поворачивать, масштабировать, удалять и так далее с помощью этого аддона.
Приятного строительства!

### Список внутриигровых команд

```
/gomove
/gomove help
/gomove invertselection
/gomove reset
```



### UI-Overview

<img width="1920" height="1018" alt="image" src="https://github.com/user-attachments/assets/1e53ef33-7da8-44a6-a790-18291e6e42ea" />

### Продвинутое использование

Хотите размещать объекты с помощью заклинания (зеленый круг прицеливания)??
- Выполните это для базы данных `acore_world`: `INSERT INTO spell_script_names (spell_id, ScriptName) VALUES (27651, 'GOMove_spell_place');`
- `.learn spell 27651`
- Теперь можно ввести объект в поле «Оправить» и нажать кнопку «Отпрв.». После этого вы сможете призывать этот объект с помощью заклинания. Все призванные объекты также сохраняются для использования заклинанием призыва (например, если они были вызваны из списка избранного).


## Требования

Для работы GO-Move требуется:

- AzerothCore v4.0.0+
- Для постоянного сохранения масштабирования (изменений размера) необходим модуль [Objscale Module](https://github.com/Tralenor/mod-objscale). Его использование опционально, но настоятельно рекомендуется. 
(Objscale зависит от двух пользовательских хуков: OnCreatureSaveToDB и OnGameObjectSaveToDB. Они представлены в виде Pull-запроса (PR) для основного репозитория AC. Ссылка на PR: https://github.com/azerothcore/azerothcore-wotlk/pull/11246 (Соответствующую ветку git можно найти здесь: https://github.com/Tralenor/azerothcore-wotlk/tree/On-Creature/GameObject-SaveToDB-Hook ))


## Установка

```
1) Просто выполните `git clone` модуля в директорию `modules` исходного кода AzerothCore или скопируйте его вручную.
2) Импортируйте SQL-файл в соответствующую базу данных (auth, world или characters).
3) Повторно запустите cmake и выполните чистую сборку AzerothCore.
4) «Установите» LUA-аддон в клиент игры (подробности см. в разделе «Использование в игре»).
```

## Credits

* [Tralenor](https://github.com/Tralenor) (автор модуля)
* [Rochet2](https://github.com/Rochet2) (Первоначальный автор GOMove)
* Оригинальная идея GOMove от [Mordred](https://www.youtube.com/channel/UCi8ykEoD_0G2nHGc06ioqzA)
* AzerothCore: [репозиторий](https://github.com/azerothcore) - [сайт](http://azerothcore.org/) - [сообщество в чате Discord](https://discord.gg/PaqQRkd)
