set files=%~dp0\main\src\main.cpp %~dp0\main\src\engine.cpp

set libs=%~dp0\lib\SDL2main.lib %~dp0\lib\SDL2.lib shell32.lib

call "%ProgramFiles%\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=amd64
@echo on

cl /Zi /I "include\*" /I "main\src" %files% /link %libs% /ENTRY:mainCRTStartup /SUBSYSTEM:WINDOWS /OUT:Tetris.exe
::call C:\Windows\SysWOW64\WindowsPowerShell\v1.0\powershell.exe -noe -c "&{Import-Module """C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\Common7\Tools\Microsoft.VisualStudio.DevShell.dll"""; Enter-VsDevShell 602e473e}; cl /Zi /I '%~dp0\lib\include\*' %files% /link %libs% /ENTRY:mainCRTStartup /OUT:Tetris.exe"

pause