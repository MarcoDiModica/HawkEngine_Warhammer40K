@echo off
echo Compilando con mono + mcs...
cd /d "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script"
set MONO_PATH=M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\External\Mono\lib
"M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\External\Mono\bin\mono.exe" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\External\Mono\lib\mono\4.5\mcs.exe" @compile.rsp
echo Código de salida: %ERRORLEVEL% > compile_result.txt
