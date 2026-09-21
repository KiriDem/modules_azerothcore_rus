DELETE FROM `command` WHERE `name`='item_upgrade list';
INSERT INTO `command`(`name`, `security`, `help`) VALUES ('item_upgrade list', 0, 'Синтаксис: .item_upgrade list
Выводит список улучшенных предметов игрока; поиск выполняется только среди экипированных предметов.');
 