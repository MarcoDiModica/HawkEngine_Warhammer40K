@echo off
echo Compilando proyecto...
cd /d "C:\Users\Marco\Documents\GitHub\HawkEngine_Warhammer40K\MyGameMaker\x64\Release\..\..\Script"
"C:\Program Files\dotnet\dotnet.exe" build "C:\Users\Marco\Documents\GitHub\HawkEngine_Warhammer40K\MyGameMaker\x64\Release\..\..\Script\C#Assembly.csproj" -c Release -v quiet
echo Código de salida: %ERRORLEVEL% > build_result.txt
