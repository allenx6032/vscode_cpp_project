@echo off
setlocal enabledelayedexpansion
set "suffix=2"  REM Ҫ��ӵĺ�׺

for %%f in (*.*) do (
    if not "%%~xf"==".bat" (  REM �ų�bat�ļ�����
        set "name=%%~nf"
        set "ext=%%~xf"
        ren "%%f" "!name!%suffix%!ext!"
    )
)
echo �������
pause