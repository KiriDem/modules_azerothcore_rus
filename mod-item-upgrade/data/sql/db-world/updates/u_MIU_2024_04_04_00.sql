DELETE FROM `command` WHERE `name`='item_upgrade lock';
INSERT INTO `command`(`name`, `security`, `help`) VALUES ('item_upgrade lock', 3, 'Синтаксис: .item_upgrade lock
Блокирует NPC, отвечающего за улучшение предметов, для безопасного редактирования базы данных. Игроки больше не смогут пользоваться этим NPC, блокировка будет снята после выполнения команды .item_upgrade reload.');
 