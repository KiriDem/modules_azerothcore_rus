DELETE FROM `command` WHERE `name`='item_upgrade reload';
INSERT INTO `command`(`name`, `security`, `help`) VALUES ('item_upgrade reload', 3, 'Синтаксис: .item_upgrade reload
Перезагружает все необходимые данные для модуля Улучшение предметов.');
 