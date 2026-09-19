-- Put only sql data in this file (insert, update, replace into, delete etc...).
-- If you don't use this database, then delete this file.
-- If no data, just delete this file.


DELETE FROM `command` WHERE name IN ('npc_scale', 'npc_scale set','npc_scale reset','gob_scale','gob_scale set','gob_scale reset');

INSERT INTO `command` (`name`, `security`, `help`) VALUES 
('npc_scale', 3, 'Синтаксис: .npc_objscale $subcommand\nВведите .help creature_objscale, чтобы просмотреть список подкоманд,\nили .help xp $subcommand, чтобы получить информацию о подкоманде.'),
('npc_scale set', 3, 'Синтаксис: .npc_objscale set $scale\nУстановить масштаб выбранного существа.'),
('npc_scale reset', 3, 'Синтаксис: .npc_objscale reset \nСбросить масштаб выбранного существа.'),
('gob_scale', 3, 'Синтаксис: .gob_objscale $subcommand\nВведите .help gob_objscale, чтобы просмотреть список подкоманд,\nили .help xp $subcommand, чтобы получить информацию о подкоманде.'),
('gob_scale set', 3, 'Синтаксис: .gob_objscale set $identifier $scale\nУстановить масштаб выбранного игрового объекта (GameObject).'),
('gob_scale reset', 3, 'Синтаксис: .gob_objscale set $identifier \nСбросить масштаб выбранного игрового объекта.');