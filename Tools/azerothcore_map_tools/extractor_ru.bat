@chcp 1251
@ECHO OFF
call :setESC
CLS

:MENU
ECHO .................................................
ECHO  %ESC%[97mЭкстракторы AzerothCore dbc, maps, vmaps, mmaps%ESC%[0m
ECHO .................................................
ECHO.
ECHO  %ESC%[93mПРЕДУПРЕЖДЕНИЕ!%ESC%[0m %ESC%[92mпри извлечении экстрактор vmaps%ESC%[0m
ECHO  %ESC%[92mвыводит текст ниже, что НЕ ЯВЛЯЕТСЯ ОШИБКОЙ:%ESC%[0m
ECHO .................................................
ECHO Extracting World\Wmo\Band\Final_Stage.wmo
ECHO No such file.
ECHO Couldn't open RootWmo!!!
ECHO Done!
ECHO .................................................
ECHO.
ECHO  %ESC%[97m1%ESC%[0m - %ESC%[96mИзвлечение base%ESC%[0m %ESC%[96mи cameras.%ESC%[0m %ESC%[97m(НЕОБХОДИМО)%ESC%[0m
ECHO  %ESC%[97m2%ESC%[0m - %ESC%[96mИзвлечение vmaps.%ESC%[0m (нужны файлы maps) %ESC%[92m(%ESC%[0m%ESC%[97mОПЦИОНАЛЬНО%ESC%[0m, %ESC%[92mочень рекомендуется)%ESC%[0m
ECHO  %ESC%[97m3%ESC%[0m - %ESC%[96mИзвлечение mmaps.%ESC%[0m (нужны файлы vmaps, %ESC%[36mможет занять много времени%ESC%[0m) %ESC%[92m(%ESC%[0m%ESC%[97mОПЦИОНАЛЬНО%ESC%[0m, %ESC%[92mочень рекомендуется)%ESC%[0m
ECHO  %ESC%[97m4%ESC%[0m - %ESC%[96mИзвлечение всех файлов.%ESC%[0m (%ESC%[36mзанимает много времени%ESC%[0m)
ECHO  %ESC%[97m5%ESC%[0m - %ESC%[1mВЫХОД%ESC%[0m
ECHO.
SET /P M=%ESC%[93m Введите 1, 2, 3, 4 или 5 затем нажмите ENTER: %ESC%[0m
IF %M%==1 GOTO MAPS
IF %M%==2 GOTO VMAPS
IF %M%==3 GOTO MMAPS
IF %M%==4 GOTO ALL
IF %M%==5 GOTO :EOF

:MAPS
start /b /w map_extractor.exe
GOTO MENU

:VMAPS
start /b /w vmap4_extractor.exe
if exist vmaps\ (
    echo папка "vmaps" найдена.
) else (
    echo создание папки "vmaps".
    mkdir "vmaps"
)
start /b /w vmap4_assembler.exe Buildings vmaps
rmdir Buildings /s /q
GOTO MENU

:MMAPS
ECHO Это может занять несколько часов. Пожалуйста, наберитесь терпения.
PAUSE
if exist mmaps\ (
    echo папка "mmaps" найдена.
) else (
    echo создание папки "mmaps".
    mkdir "mmaps"
)
start /b /w mmaps_generator.exe
GOTO MENU

:ALL
ECHO Это может занять несколько часов. Пожалуйста, наберитесь терпения.
PAUSE
if exist vmaps\ (
    echo папка "vmaps" найдена.
) else (
    echo создание папки "vmaps".
    mkdir "vmaps"
)
if exist mmaps\ (
    echo папка "mmaps" найдена.
) else (
    echo создание папки "mmaps".
    mkdir "mmaps"
)
start /b /w map_extractor.exe
start /b /w vmap4_extractor.exe
start /b /w vmap4_assembler.exe Buildings vmaps
rmdir Buildings /s /q
start /b /w mmaps_generator.exe
GOTO MENU
:setESC
for /F "tokens=1,2 delims=#" %%a in ('"prompt #$H#$E# & echo on & for %%b in (1) do rem"') do (
  set ESC=%%b
  exit /B 0
)
exit /B 0