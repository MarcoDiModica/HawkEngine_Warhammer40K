@echo off
echo Compilando proyecto...
cd /d "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\x64\Release\..\..\Script"
"C:\Program Files\dotnet\dotnet.exe" build "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\x64\Release\..\..\Script\C#Assembly.csproj" -c Release -v quiet
echo Código de salida: %ERRORLEVEL% > build_result.txt
