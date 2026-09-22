# Модуль трансмогрификации

## Важные примечания

Необходимо использовать как минимум этот коммит AzerothCore:

<https://github.com/azerothcore/azerothcore-wotlk/commit/b6cb9247ba96a862ee274c0765004e6d2e66e9e4>

Если вы используете этот модуль с коммитом AzerothCore, предшествующим этому:

<https://github.com/azerothcore/azerothcore-wotlk/commit/b34bc28e5b02514fca3519beac420c58faa89cad>

пожалуйста, удалите записи с ID 50000 и 50001 из таблицы `npc_text` перед обновлением AzerothCore:
```sql
DELETE FROM `npc_text` WHERE `ID` IN (50000,50001);
```
В противном случае возникнут конфликты из-за этих ID. Теперь модуль будет использовать ID 601083 и 601084 по умолчанию.

## Требования

Для работы модуля трансмогрификации в данный момент требуются:

AzerothCore v1.0.2+

## Установка

### 1) Просто поместите модуль в папку `modules` внутри директории с исходным кодом AzerothCore.

Вы можете клонировать его с помощью git в директорию `azerothcore/modules`:

```sh
cd path/to/azerothcore/modules
git clone https://github.com/azerothcore/mod-transmog.git
```

или вы можете вручную [скачать модуль](https://github.com/azerothcore/mod-transmog/archive/master.zip), распакуйте папку Transmog и поместите её в директорию `azerothcore/modules`.

### 2) Импортируйте SQL-файл в соответствующую базу данных (auth, world или characters)

Импортируйте SQL-файл вручную в нужную базу данных (auth, world или characters) либо воспользуйтесь скриптом `db_assembler.sh` (если предоставлен файл `include.sh`).

### 3) Повторно запустите cmake и выполните чистую сборку AzerothCore

### 4) Разместите NPC для трансмогрификации

Зайдя в игру под учетной записью ГМ (GM), переместитесь в то место, где вы хотите разместить NPC, и введите следующую команду:

```
.npc add 190010
```

**Вот и всё.**

### (Необязательно) Редактирование конфигурации модуля

Если вам нужно изменить конфигурацию модуля, перейдите в папку конфигурации вашего сервера (например, **etc**), скопируйте файл `transmog.conf.dist` в файл `transmog.conf` и отредактируйте его по своему усмотрению.

## Лицензия

Этот модуль распространяется под лицензией [GNU AGPL license](https://github.com/azerothcore/mod-transmog/blob/master/LICENSE).





