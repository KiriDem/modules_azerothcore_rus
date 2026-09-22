 ![logo](https://raw.githubusercontent.com/azerothcore/azerothcore.github.io/master/images/logo-github.png) AzerothCore

# Модуль Solo Dungeon Finder


## Описание

Позволяет игрокам пользоваться системой поиска подземелий как в одиночку, так и в группах (численностью до 5 человек). Предназначено для использования с AzerothCore (версия 3.3.5a). Отлично дополняет модули mod-solocraft и mod-autobalance.

## Стандартная установка
```
1. Перейдите в папку `/modules/`.
2. Выполните команду `git clone https://github.com/azerothcore/mod-solo-lfg.git`.
3. Повторно запустите CMake и выполните чистую сборку AzerothCore. 
   a. При использовании Bash-панели управления AzerothCore: выполните `./acore.sh compiler configure`, а затем `./acore.sh compiler build`. 
   b. При стандартной установке обратитесь к руководству: https://www.azerothcore.org/wiki/installation
```

## Установка через Docker
```
1. Перейдите в папку `/modules/`.
2. Выполните команду `git clone https://github.com/azerothcore/mod-solo-lfg.git`.
3. Заново запустите cmake и выполните чистую сборку AzerothCore. 
   a. Перейдите в корневую папку и выполните `./bin/acore-docker-build`.
   b. Затем выполните `docker-compose up`.
```

## Редактирование конфигурации модуля (необязательно)

Если вам нужно изменить настройки модуля, перейдите в папку конфигурации сервера (туда, где находится `worldserver` или `worldserver.exe`),
скопируйте файл `SoloLfg.conf.dist` в `SoloLfg.conf` и отредактируйте его (сохраните оба файла).

## Стандартное удаление
```
1. Удалите папку `/modules/mod-solo-lfg`.
2. Повторно запустите CMake и выполните чистую сборку AzerothCore.
```

## Удаление Docker-версии
```
1. Удалите папку /modules/mod-solo-lfg
2. Повторно выполните cmake и запустите чистую сборку AzerothCore. 
a. Перейдите в корневую папку и выполните ./bin/acore-docker-build
b. Затем выполните docker-compose up
```
```

## Авторы
*  [Traesh: Оригинальный скрипт](https://github.com/Traesh)
*  [Micrah/Milestorme: Создатель модуля](https://github.com/milestorme).
*  [Conan513](https://github.com/conan513).
*  [Artanisx](https://github.com/Artanisx) - Обновление для установок Docker. Патч LFG-Solo.patch обновлен для совместимости с актуальной версией AzerothCore (от 17.09.2021). Обновлены стандартные инструкции по установке.
