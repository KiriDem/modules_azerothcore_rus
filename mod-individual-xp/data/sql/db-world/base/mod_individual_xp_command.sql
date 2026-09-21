DELETE FROM `command` WHERE `name` IN ('xp', 'xp set', 'xp view', 'xp default', 'xp enable', 'xp disable');

INSERT INTO `command` (`name`, `security`, `help`) VALUES 
('xp', 0, 'Синтаксис: .xp $subcommand\nВведите .help xp, чтобы просмотреть список подкоманд.\nили .help xp $subcommand, чтобы получить информацию о конкретной подкоманде.'),
('xp set', 0, 'Синтаксис: .xp set $rate\nУстановите собственный коэффициент получения опыта.'),
('xp view', 0, 'Синтаксис: .xp view\nПосмотрите текущий коэффициент получения опыта.'),
('xp default', 0, 'Синтаксис: .xp default\nУстановите скорость получения опыта по умолчанию.'),
('xp enable', 0, 'Синтаксис: .xp enable\nВключите пользовательский множитель опыта.'),
('xp disable', 0, 'Синтаксис: .xp disable\nВыключите пользовательский множитель опыта.');
