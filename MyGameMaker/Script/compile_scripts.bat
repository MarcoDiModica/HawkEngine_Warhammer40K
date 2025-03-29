@echo off
cd /d "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script"
echo Compilando scripts...
"M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\External\Mono\lib\mono\4.5\mcs.exe" @mcs_response.rsp
if %ERRORLEVEL% EQU 0 (
  echo Compilación exitosa
) else (
  echo Error en la compilación: %ERRORLEVEL%
)
echo Código de salida: %ERRORLEVEL% > compile_result.txt
