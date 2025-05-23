@echo off
setlocal enabledelayedexpansion
set "suffix=2"  REM 要添加的后缀

for %%f in (*.*) do (
    if not "%%~xf"==".bat" (  REM 排除bat文件自身
        set "name=%%~nf"
        set "ext=%%~xf"
        ren "%%f" "!name!%suffix%!ext!"
    )
)
echo 操作完成
pause