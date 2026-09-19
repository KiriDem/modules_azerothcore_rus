CREATE TABLE IF NOT EXISTS `creature_template_outfits` (
    `entry` INT UNSIGNED NOT NULL,
    `npcsoundsid` INT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'вход в NPCSounds.dbc',
    `race` TINYINT UNSIGNED NOT NULL DEFAULT 1,
    `class` TINYINT UNSIGNED NOT NULL DEFAULT 1,
    `gender` TINYINT UNSIGNED NOT NULL DEFAULT 0 COMMENT '0 для мужчин, 1 для женщин',
    `skin` TINYINT UNSIGNED NOT NULL DEFAULT 0,
    `face` TINYINT UNSIGNED NOT NULL DEFAULT 0,
    `hair` TINYINT UNSIGNED NOT NULL DEFAULT 0,
    `haircolor` TINYINT UNSIGNED NOT NULL DEFAULT 0,
    `facialhair` TINYINT UNSIGNED NOT NULL DEFAULT 0,
    `head` INT NOT NULL DEFAULT 0,
    `shoulders` INT NOT NULL DEFAULT 0,
    `body` INT NOT NULL DEFAULT 0,
    `chest` INT NOT NULL DEFAULT 0,
    `waist` INT NOT NULL DEFAULT 0,
    `legs` INT NOT NULL DEFAULT 0,
    `feet` INT NOT NULL DEFAULT 0,
    `wrists` INT NOT NULL DEFAULT 0,
    `hands` INT NOT NULL DEFAULT 0,
    `back` INT NOT NULL DEFAULT 0,
    `tabard` INT NOT NULL DEFAULT 0,
    `guildid` INT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'ID гильдии, Использующий эмблемы на гербе.',
    `description` TEXT NULL DEFAULT NULL,
    PRIMARY KEY (`entry`)
) ENGINE=InnoDB
  COMMENT='Слоты предметов: положительные значения — это записи предметов, отрицательные — их displayID.';

CREATE TABLE IF NOT EXISTS `creature_template_outfits_npcs` (
    `creature_entry` INT UNSIGNED NOT NULL COMMENT 'creature_template.entry',
    `outfit_entry` INT UNSIGNED NOT NULL COMMENT 'creature_template_outfits.entry',
    PRIMARY KEY (`creature_entry`)
) ENGINE=InnoDB
  COMMENT='Одевает каждое порождение существа';

CREATE TABLE IF NOT EXISTS `creature_template_outfits_spawns` (
    `spawn_guid` INT UNSIGNED NOT NULL COMMENT 'creature.guid',
    `outfit_entry` INT UNSIGNED NOT NULL COMMENT 'creature_template_outfits.entry',
    PRIMARY KEY (`spawn_guid`)
) ENGINE=InnoDB
  COMMENT='Одевает одно существо, переопределяя начальное назначение';
