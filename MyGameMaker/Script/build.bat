@echo off
echo Compilando proyecto...
cd /d "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\x64\Release\..\..\Script"
"C:\Program Files\dotnet\dotnet.exe" build "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\x64\Release\..\..\Script\C#Assembly.csproj" -c Release > build_output.txt 2>&1
echo Código de salida: %ERRORLEVEL% > build_result.txt
findstr /C:"warning" build_output.txt > build_warnings.txt 2>nul
findstr /C:"error" build_output.txt > build_errors.txt 2>nul
