@echo off
rem Play the portable build on Windows: tools\run-civ2.bat [GAMEDIR]
rem GAMEDIR: the installed game (default %%USERPROFILE%%\Civ2); the game saves there.
rem A maximized window with a Windows menu bar; set CIV2_FULLSCREEN=1 for full
rem screen, CIV2_CLASSIC=1 for the original game: menus and caption drawn as the
rem original's, and the original's bugs, which OpenCiv2 otherwise fixes (README,
rem docs\BUGS.md).
setlocal
set "HERE=%~dp0.."
set "GAME=%~1"
if "%GAME%"=="" set "GAME=%USERPROFILE%\Civ2"
if not exist "%GAME%\RULES.TXT" (echo no game in %GAME%: install Civilization II there or name its directory & exit /b 1)
if not exist "%GAME%\CIV2.DAT" copy "%HERE%\harness\CIV2.DAT" "%GAME%\" >nul
set "CIV2_DATA=%HERE%\build\data"
"%HERE%\build\OpenCiv2.exe" "%GAME%"
