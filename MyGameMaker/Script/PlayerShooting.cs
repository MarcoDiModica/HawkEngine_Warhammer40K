using System.Collections.Generic;
using System.Numerics;
using HawkEngine;
using System;
using static Railgun;

public class PlayerShooting : MonoBehaviour
{
    public float shootCooldown = 0.01f;
    public float projectileSpeed = 90.0f;
    public float projectileLifetime = 0.5f;
    private PlayerInput playerInput;
    private Transform transform;
    private float shootTimer = 0f;

    private PlayerController playerController;
    public PlayerData playerData;
    private RedThirstManager redThirstManager;

    private float abilityUseTimer = 0f;
    private int abilityCount = 0;
    private const float abilityTimeLimit = 3f;

    private bool firstShotReady = false;
    private float firstShotTimer = 0f;
    //private float firstShotDelay = 0.04f;

    // Guns Scripts
    public Boltgun boltgun;
    public Shotgun shotgun;
    public Railgun railgun;

    private GameObject boltgunMesh;
    private GameObject shotgunMesh;
    private GameObject railgunMesh;

    public bool hasShotgun = false;
    public bool hasRailgun = false;
    public bool hasBoltgun = true;

    //private AudioSource sound;
    private const string boltgunEquiped = "Assets/Audio/SFX/Weapons/Boltgun/BoltgunEqquiped.wav";
    private const string shotgunEquiped = "Assets/Audio/SFX/Weapons/Shotgun/ShotgunEqquiped.wav";
    private const string railgunEquiped = "Assets/Audio/SFX/Weapons/Railgun/RailgunEqquiped.wav";


    public ParticleFX rifleShotFX;
    public ParticleFX shotgunShotFX;
    public ParticleFX railgunShotSemiFX;
    public ParticleFX railgunShotAutoFX;
    public ParticleFX bulletcasingFX;
    public ParticleFX grenadeShotFX;
    


    private float changeWeaponDelay = 0.5f;
    private float currentdelay = 0.5f;
    private bool autoReloading = false;
    private float autoReloadTimer = 0f;
    private const float autoReloadDelay = 2f;

    private enum GunType
    {
        BOLTGUN,
        SHOTGUN,
        RAILGUN
    }
    private GunType currentGun = GunType.BOLTGUN;

    public int GetCurrentGun()
    {
        return (int)currentGun;
    }

    public override void Awake()
    {

    }
    public override void Start()
    {
        playerInput = gameObject.GetComponent<PlayerInput>();
        if (playerInput == null)
        {
            Engineson.print("ERROR: PlayerShooting requires a PlayerInput component!");
        }
        redThirstManager = gameObject.GetComponent<RedThirstManager>();
        if (redThirstManager == null)
        {
            Engineson.print("ERROR: PlayerShooting requires a RedThirstManager component!");
        }
        transform = gameObject.GetComponent<Transform>();
        if (transform == null)
        {
            Engineson.print("ERROR: PlayerShooting requires a Transform component!");
        }

        boltgun = gameObject.GetComponent<Boltgun>();
        boltgun.Start();
        if (boltgun == null)
        {
            Engineson.print("ERROR: PlayerShooting requires a Boltgun component!");
        }

        shotgun = gameObject.GetComponent<Shotgun>();
        shotgun.Start();
        if (shotgun == null)
        {
            Engineson.print("ERROR: PlayerShooting requires a Shotgun component!");
        }

        railgun = gameObject.GetComponent<Railgun>();
        railgun.Start();
        if (railgun == null)
        {
            Engineson.print("ERROR: PlayerShooting requires a Ra ilgun component!");
        }

        rifleShotFX = GameObject.Find("RiffleShotFX").GetComponent<ParticleFX>();
        rifleShotFX.ApplyPreset(8);
        rifleShotFX.SetParticleStartRotation(90f);
        shotgunShotFX = GameObject.Find("ShotgunShotFX").GetComponent<ParticleFX>();
        shotgunShotFX.ApplyPreset(8);
        railgunShotSemiFX = GameObject.Find("RailgunShotSemiFX").GetComponent<ParticleFX>();
        railgunShotSemiFX.ApplyPreset(8);
        railgunShotAutoFX = GameObject.Find("RailgunShotAutoFX").GetComponent<ParticleFX>();
        railgunShotAutoFX.ApplyPreset(8);
        bulletcasingFX = GameObject.Find("BulletCasingFX").GetComponent<ParticleFX>();
        bulletcasingFX.ApplyPreset(44);
        grenadeShotFX = GameObject.Find("GrenadeShotFX").GetComponent<ParticleFX>();
        grenadeShotFX.ApplyPreset(43);


        boltgunMesh = GameObject.Find("Boltgun");
        shotgunMesh = GameObject.Find("Shotgun");
        railgunMesh = GameObject.Find("Railgun");

        if (shotgunMesh != null)
        {
            Engineson.print("Fallo por las putas meshes.");
            shotgunMesh.SetActive(false);
        }
        if (railgunMesh != null)
        {
            Engineson.print("Fallo por las putas meshes.");
            railgunMesh.SetActive(false);
        }
        if (boltgunMesh != null)
        {
            Engineson.print("Fallo por las putas meshes.");
            boltgunMesh.SetActive(true);
        }
        playerController = gameObject.GetComponent<PlayerController>();
        playerData = playerController.playerData;

        hasBoltgun = playerData.hasBoltgun;
        hasShotgun = playerData.hasShotgun;
        hasRailgun = playerData.hasRailgun;

        if (hasRailgun)
        {
            railgun.railgunMode = Railgun.RailgunMode.SEMIAUTOMATIC;
        }

        switch (currentGun)
        {
            case GunType.BOLTGUN:
                shootCooldown = 1f / boltgun.shootCadence * playerData.bonusCadence;
                shootTimer = 0;
                Engineson.print($"Shoot Cooldown: {shootCooldown}");
                break;
            case GunType.SHOTGUN:
                shootCooldown = 1f / shotgun.shootCadence * playerData.bonusCadence;
                shootTimer = 0;
                break;
            case GunType.RAILGUN:
                shootCooldown = 1f / railgun.shootCadence * playerData.bonusCadence;
                shootTimer = 0;
                break;
        }

    }

    public int GetCurrentAmmo()
    {
        switch (currentGun)
        {
            case GunType.BOLTGUN:
                return boltgun.GetCurrentAmmo();
                
            case GunType.SHOTGUN:
                return shotgun.GetCurrentAmmo();
                
            case GunType.RAILGUN:
                return railgun.GetCurrentAmmo();
                
        }
        return 0;
    }

    public int GetMaxAmmo()
    {
        switch (currentGun)
        {
            case GunType.BOLTGUN:
                return boltgun.GetMaxAmmo();
                
            case GunType.SHOTGUN:
                return shotgun.GetMaxAmmo();
               
            case GunType.RAILGUN:
                return railgun.GetMaxAmmo();
                
        }
        return 0;
    }

    public override void Update(float deltaTime)
    {
        if(currentdelay < changeWeaponDelay)
        {
            currentdelay += deltaTime;            
        }


        playerInput.UpdateLookDirection();

        if (playerInput.IsChangingWeaponRight() || Input.GetKeyDown(KeyCode.Q) )
        {
            ChangeWeaponRight();
            currentdelay = 0f;
        }
        else if (playerInput.IsChangingWeaponLeft() || Input.GetKeyDown(KeyCode.C))
        {
            ChangeWeaponLeft();
            currentdelay = 0f;
        }

        if (playerInput.IsChangingRailgunMode() && currentGun == GunType.RAILGUN)
        {
            railgun?.ChangeMode();
            currentdelay = 0f;
        }

        if (playerInput?.IsShooting() == true && currentdelay >= changeWeaponDelay)
        {
            Shoot();
        }
        else
        {
            firstShotTimer = 0f;
            firstShotReady = false;

            switch (currentGun)
            {
                case GunType.BOLTGUN:
                    shootTimer = 0;
                    break;
                case GunType.SHOTGUN:
                    shootTimer = 0.5f;
                    break;
                case GunType.RAILGUN:
                    shootTimer = 0;
                    break;
            }

            rifleShotFX.Stop();
            shotgunShotFX.Stop();
            railgunShotSemiFX.Stop();
            railgunShotAutoFX.Stop();
        }

        if (playerInput?.IsReloading() == true && currentGun != GunType.RAILGUN)
        {
            Reload();
        }

        if (currentGun == GunType.RAILGUN && playerInput.IsShooting() == false && railgun.isCooling == false)
        {
            railgun.isReloading = true;
        }

        if (playerInput?.IsAbility1Pressed() == true)
        {
            Engineson.print("Ability 1 pressed");
            UseAbility1();
        }

        if (playerInput?.IsAbility2Pressed() == true)
        {
            //Engineson.print("Ability 2 pressed");
            //UseAbility2();
        }
        if (!autoReloading && GetCurrentAmmo() == 0 && currentGun != GunType.RAILGUN)
        {
            autoReloading = true;
            autoReloadTimer = 0f;
        }
        if (autoReloading)
        {
            autoReloadTimer += deltaTime;
            if (autoReloadTimer >= autoReloadDelay)
            {
                Reload();
                autoReloading = false;
            }
        }
        if (playerInput?.IsReloading() == true && currentGun != GunType.RAILGUN)
        {
            autoReloading = false;
        }
    }

    private void Shoot()
    {
        try
        {
            Vector3 lookDir = playerInput.GetCurrentLookDirection();

            if (lookDir == Vector3.Zero)
            {
                lookDir = playerInput.GetCurrentMoveDirection();
            }

            if (lookDir != Vector3.Zero)
            {
                lookDir = Vector3.Normalize(lookDir);
                float angle = (float)(Math.Atan2(lookDir.X, lookDir.Z) * (180.0 / Math.PI)) - 45;
                

                switch (currentGun)
                {
                    case GunType.BOLTGUN:
                        rifleShotFX.SetParticleStartRotation(angle);
                        rifleShotFX.EmitBurst(1);
                        boltgun?.Shoot();
                        bulletcasingFX.Play();
                        shotgunShotFX.Stop();
                        railgunShotAutoFX.Stop();
                        railgunShotSemiFX.Stop();
                        break;

                    case GunType.SHOTGUN:
                        shotgunShotFX.SetParticleStartRotation(angle);
                        shotgunShotFX.EmitBurst(1);
                        shotgun?.Shoot();
                        rifleShotFX.Stop();
                        railgunShotAutoFX.Stop();
                        railgunShotSemiFX.Stop();
                        break;

                    case GunType.RAILGUN:
                        if (railgun.railgunMode == Railgun.RailgunMode.SEMIAUTOMATIC)
                        {
                            railgunShotSemiFX.SetParticleStartRotation(angle);
                            railgunShotSemiFX.EmitBurst(1);
                        }
                        else
                        {
                            railgunShotAutoFX.SetParticleStartRotation(angle);
                            railgunShotAutoFX.EmitBurst(1);
                        }
                        railgun?.Shoot();
                        break;
                }
            }
            else
            {
                // Si no hay direccion valida, disparar sin rotar el efecto
                switch (currentGun)
                {
                    case GunType.BOLTGUN:
                        boltgun?.Shoot();
                        shotgunShotFX.Stop();
                        bulletcasingFX.Play();
                        railgunShotAutoFX.Stop();
                        railgunShotSemiFX.Stop();
                        break;

                    case GunType.SHOTGUN:
                        shotgun?.Shoot();
                        rifleShotFX.Stop();
                        bulletcasingFX.Stop();
                        railgunShotAutoFX.Stop();
                        railgunShotSemiFX.Stop();
                        break;

                    case GunType.RAILGUN:
                        railgun?.Shoot();
                        break;
                }
            }
        }
        catch (System.Exception e)
        {
            Engineson.print($"Error during Shoot(): {e.Message}");
        }
    }


    private void Reload()
    {
        switch (currentGun)
        {
            case GunType.BOLTGUN:
                boltgun?.Reload();
                break;
            case GunType.SHOTGUN:
                shotgun?.Reload();
                break;
            case GunType.RAILGUN:
                
                break;
        }
    }

    public void ApplyBonusCadence()
    {
        switch (currentGun)
        {
            case GunType.BOLTGUN:
                shootCooldown = 1f / boltgun.shootCadence * playerData.bonusCadence;
                shootTimer = 0;
                break;
            case GunType.SHOTGUN:
                shootCooldown = 1f / shotgun.shootCadence * playerData.bonusCadence;
                shootTimer = 0;
                break;
            case GunType.RAILGUN:
                shootCooldown = 1f / railgun.shootCadence * playerData.bonusCadence;
                shootTimer = 0;
                break;
        }
    }

    private void ChangeWeaponRight()
    {

        if (hasShotgun && !hasRailgun)
        {
            if (currentGun == GunType.BOLTGUN)
            {
                currentGun = GunType.SHOTGUN;

            }
            else if (currentGun == GunType.SHOTGUN)
            {
                currentGun = GunType.BOLTGUN;
            }
           
        }
        else if (hasShotgun && hasRailgun)
        {
            if (currentGun == GunType.BOLTGUN)
            {
                currentGun = GunType.SHOTGUN;
            }
            else if (currentGun == GunType.SHOTGUN)
            {
                currentGun = GunType.RAILGUN;
            }
            else if (currentGun == GunType.RAILGUN)
            {
                currentGun = GunType.BOLTGUN;
            }
        }

        

        switch (currentGun)
        {
            case GunType.BOLTGUN:
                shootCooldown = 1f / boltgun.shootCadence * playerData.bonusCadence;
                shootTimer = 0;
                shotgunMesh?.SetActive(false);
                boltgunMesh?.SetActive(true);
                railgunMesh?.SetActive(false);
                int audio = Audio.PlayOneShot(boltgunEquiped);
                break;
            case GunType.SHOTGUN:
                shootCooldown = 1f / shotgun.shootCadence * playerData.bonusCadence;
                shootTimer = 0;
                shotgunMesh?.SetActive(true);
                boltgunMesh?.SetActive(false);
                railgunMesh?.SetActive(false);
                int audioo = Audio.PlayOneShot(shotgunEquiped);
                break;
            case GunType.RAILGUN:
                shootCooldown = 1f / railgun.shootCadence * playerData.bonusCadence;
                shootTimer = 0;
                shotgunMesh?.SetActive(false);
                boltgunMesh?.SetActive(false);
                railgunMesh?.SetActive(true);
                int audiooo = Audio.PlayOneShot(railgunEquiped);
                break;
        }
        Engineson.print("Changed weapon right");
    }

    private void ChangeWeaponLeft()
    {

        if (hasShotgun && !hasRailgun)
        {
            if (currentGun == GunType.BOLTGUN)
            {
                currentGun = GunType.SHOTGUN;
            }
            else if (currentGun == GunType.SHOTGUN)
            {
                currentGun = GunType.BOLTGUN;
            }

        }
        else if (hasShotgun && hasRailgun)
        {
            if (currentGun == GunType.BOLTGUN)
            {
                currentGun = GunType.RAILGUN;
            }
            else if (currentGun == GunType.SHOTGUN)
            {
                currentGun = GunType.BOLTGUN;
            }
            else if (currentGun == GunType.RAILGUN)
            {
                currentGun = GunType.SHOTGUN;
            }
        }

        switch (currentGun)
        {
            case GunType.BOLTGUN:
                shootCooldown = 1f / boltgun.shootCadence * playerData.bonusCadence;
                shootTimer = 0;
                boltgunMesh.SetActive(true);
                shotgunMesh.SetActive(false);
                railgunMesh.SetActive(false);
                int audio = Audio.PlayOneShot(boltgunEquiped);
                break;
            case GunType.SHOTGUN:
                shootCooldown = 1f / shotgun.shootCadence * playerData.bonusCadence;
                shootTimer = 0;
                boltgunMesh.SetActive(false);
                shotgunMesh.SetActive(true);
                railgunMesh.SetActive(false);
                int audioo = Audio.PlayOneShot(shotgunEquiped);
                break;
            case GunType.RAILGUN:
                shootCooldown = 1f / railgun.shootCadence * playerData.bonusCadence;
                shootTimer = 0;
                boltgunMesh.SetActive(false);
                shotgunMesh.SetActive(false);
                railgunMesh.SetActive(true);
                int audiooo = Audio.PlayOneShot(railgunEquiped);
                break;
        }
        Engineson.print("Changed weapon left");

    }

    private void UseAbility1()
    {
        switch (currentGun)
        {
            case GunType.BOLTGUN:

                if ((playerData.BoltgunUpgraded == true))
                {
                    if (boltgun.grenadeLauncher.canThrow)
                    {
                        grenadeShotFX.Play();
                      
                    }
                    boltgun?.UseAbility1();
                }
                

              

                break;
            case GunType.SHOTGUN:

                if(playerData.ShotgunUpgraded == true)
                {
                    if (shotgun.barrage.canThrow)
                    {
                       
                    }
                    shotgun?.UseAbility1();
                }
               

               
                break;
            case GunType.RAILGUN:

                if (playerData.RailgunUpgraded == true)
                {
                    if (railgun.energyBall.canThrow)
                    {
                   
                    }
                    railgun?.UseAbility1();
                }


                break;
        }
    }

     private void UseAbility2()
     {
        switch (currentGun)
        {
            case GunType.BOLTGUN:

                if (playerData.RailgunUpgraded == true)
                {

                    boltgun?.UseAbility2();
                }
                   
                
               
                break;
            case GunType.SHOTGUN:
               
                if(playerData.ShotgunUpgraded == true)
                {

                    shotgun?.UseAbility2();
                }
                   
                
               
                break;
            case GunType.RAILGUN:
                if(playerData.RailgunUpgraded == true)
                {

                    railgun?.UseAbility2();
                }
                
                break;
        }
     }

    public void CounterAttack(GameObject target)
    {
        if (target.GetComponent<EnemyControllerMelee>() != null)
        {
            target.GetComponent<EnemyControllerMelee>().currentHealth -= 10;
            if (target.GetComponent<EnemyControllerMelee>().currentHealth > 0)
            {
                target.GetComponent<EnemyControllerMelee>().isStunned = true;
            }
            Engineson.print("Counter Attack to: " + target.name);
            Engineson.print("Current Enemy Health: " + target.GetComponent<EnemyControllerMelee>().currentHealth);
        }
        else if (target.GetComponent<EnemyControllerRanged>() != null)
        {
            target.GetComponent<EnemyControllerRanged>().currentHealth -= 10;
            if (target.GetComponent<EnemyControllerRanged>().currentHealth > 0)
            {
                target.GetComponent<EnemyControllerRanged>().isStunned = true;
            }
            Engineson.print("Counter Attack to: " + target.name);
            Engineson.print("Current Enemy Health: " + target.GetComponent<EnemyControllerRanged>().currentHealth);
        }
        else if (target.GetComponent<EnemyControllerStalker>() != null)
        {
            target.GetComponent<EnemyControllerStalker>().currentHealth -= 10;
            if (target.GetComponent<EnemyControllerStalker>().currentHealth > 0)
            {
                target.GetComponent<EnemyControllerStalker>().isStunned = true;
            }
            Engineson.print("Counter Attack to: " + target.name);
            Engineson.print("Current Enemy Health: " + target.GetComponent<EnemyControllerStalker>().currentHealth);
        }
    }

    public void ResetAbilityCooldown()
    {
        boltgun.ResetCooldowns();
        shotgun.ResetCooldowns();
    }
}
