@echo off
echo Compilando con CSC moderno...
"C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\MSBuild\Current\Bin\Roslyn\csc.exe" /target:library /out:"M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\obj\Script_temp.dll" /langversion:latest /nowarn:0626,0642,0649,0414,0169 /warnaserror- /reference:System.dll /reference:System.Core.dll /reference:System.Numerics.dll /reference:System.Drawing.dll /lib:"C:\WINDOWS\Microsoft.NET\Framework64\v4.0.30319" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\Audio.cs" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\BaseAbilities.cs" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\BaseWeapon.cs" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\Boltgun.cs" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\BoxCollider.cs" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\BulletData.cs" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\Camera.cs" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\CapsuleCollider.cs" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\Collider.cs" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\Component.cs" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\EnemyController.cs" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\Engineson.cs" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\GameObject.cs" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\Grenade.cs" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\GrenadeLauncher.cs" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\ImageAsSlider.cs" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\Input.cs" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\MenuButtons.cs" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\MeshCollider.cs" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\MeshRenderer.cs" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\MonoBehaviour.cs" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\PlayerAnimations.cs" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\PlayerCamera.cs" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\PlayerController.cs" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\PlayerDash.cs" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\PlayerData.cs" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\PlayerInput.cs" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\PlayerMovement.cs" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\PlayerShooting.cs" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\Railgun.cs" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\Rigidbody.cs" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\SceneManager.cs" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\Shotgun.cs" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\SkeletalAnimation.cs" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\Test1.cs" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\Transform.cs" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\UIButton.cs" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\UICanvas.cs" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\UIGameplay.cs" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\UIImage.cs" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\UITransform.cs"
if %ERRORLEVEL% EQU 0 (
  if exist "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\obj\Script_temp.dll" (
    for /L %%i in (1,1,10) do (
      copy /Y "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\obj\Script_temp.dll" "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\obj\Script.dll" > nul 2>&1
      if not errorlevel 1 goto :succeed
      timeout /t 1 > nul
    )
    echo Error: No se pudo copiar el assembly temporal a la ubicación final.
    goto :failed
  )
)
:succeed
echo Assembly copiado exitosamente.
del "M:\HawkEngine\HawkEngine_Warhammer40K\MyGameMaker\Script\obj\Script_temp.dll" > nul 2>&1
goto :end
:failed
echo Compilación fallida o no se pudo copiar el assembly.
:end
echo Código de salida: %ERRORLEVEL% > compile_result.txt
