using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

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

    // Guns Scripts
    public Boltgun boltgun;
    public Shotgun shotgun;
    private Railgun railgun;


    private Audio sound;
    private string boltgunEquiped = "Assets/Audio/SFX/Weapons/Boltgun/BoltgunEqquiped.wav";
    private string shotgunEquiped = "Assets/Audio/SFX/Weapons/Shotgun/ShotgunEqquiped.wav";
    private string railgunEquiped = "Assets/Audio/SFX/Weapons/Railgun/RailgunEqquiped.wav";

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

        transform = gameObject.GetComponent<Transform>();
        if (transform == null)
        {
            Engineson.print("ERROR: PlayerShooting requires a Transform component!");
        }

        sound = gameObject.GetComponent<Audio>();
        if (sound == null)
        {
            Engineson.print("PlayerShooting: Audio component not found");
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

        playerController = gameObject.GetComponent<PlayerController>();
        playerData = playerController.playerData;

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

    public override void Update(float deltaTime)
    {


        //Engineson.print($"Shoot Timer: {shootTimer}");

        if (playerInput.IsChangingWeaponRight() || Input.GetKeyDown(KeyCode.Y))
        {
            ChangeWeaponRight();
        }
        else if (playerInput.IsChangingWeaponLeft() || Input.GetKeyDown(KeyCode.T))
        {
            ChangeWeaponLeft();
        }

        if (playerInput.IsChangingRailgunMode() && currentGun == GunType.RAILGUN)
        {
            railgun.ChangeMode();
        }


        if (playerInput?.IsShooting() == true)
        {
            Shoot();
        }
        else if (playerInput.IsShooting() == false)
        {
            switch (currentGun)
            {
                case GunType.BOLTGUN:
                    shootTimer = 0;
                    break;
                case GunType.SHOTGUN:
                    shootTimer = 0.5f;
                    break;
                case GunType.RAILGUN:
                    shootTimer = shootCooldown;
                    break;
            }
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
            Engineson.print("Ability 2 pressed");
            UseAbility2();
        }

    }

    private void Shoot()
    {
        try
        {
            
            switch(currentGun)
            {
                case GunType.BOLTGUN:
                    boltgun.Shoot();
                    break;
                case GunType.SHOTGUN:
                    shotgun.Shoot();
                    break;
                case GunType.RAILGUN:
                    railgun.Shoot();
                    break;
            }


            
        }
        catch (System.Exception e)
        {
            Engineson.print($"Error creating projectile: {e.Message}");
        }
    }

    private void Reload()
    {
        switch (currentGun)
        {
            case GunType.BOLTGUN:
                boltgun.Reload();
                break;
            case GunType.SHOTGUN:
                shotgun.Reload();
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

        switch (currentGun)
        {
            case GunType.BOLTGUN:
                shootCooldown = 1f / boltgun.shootCadence * playerData.bonusCadence;
                shootTimer = 0;
                sound?.LoadAudio(boltgunEquiped);
                sound?.Play();
                break;
            case GunType.SHOTGUN:
                shootCooldown = 1f / shotgun.shootCadence * playerData.bonusCadence;
                shootTimer = 0;
                sound?.LoadAudio(shotgunEquiped);
                sound?.Play();
                break;
            case GunType.RAILGUN:
                shootCooldown = 1f / railgun.shootCadence * playerData.bonusCadence;
                shootTimer = 0;
                sound?.LoadAudio(railgunEquiped);
                sound?.Play();
                break;
        }
    }

    private void ChangeWeaponLeft()
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

        switch (currentGun)
        {
            case GunType.BOLTGUN:
                shootCooldown = 1f / boltgun.shootCadence * playerData.bonusCadence;
                shootTimer = 0;
                sound?.LoadAudio(boltgunEquiped);
                sound?.Play();
                break;
            case GunType.SHOTGUN:
                shootCooldown = 1f / shotgun.shootCadence * playerData.bonusCadence;
                shootTimer = 0;
                sound?.LoadAudio(shotgunEquiped);
                sound?.Play();
                break;
            case GunType.RAILGUN:
                shootCooldown = 1f / railgun.shootCadence * playerData.bonusCadence;
                shootTimer = 0;
                sound?.LoadAudio(railgunEquiped);
                sound?.Play();
                break;
        }
    }

    private void UseAbility1()
    {
        switch (currentGun)
        {
            case GunType.BOLTGUN:
                boltgun.UseAbility1();
                break;
            case GunType.SHOTGUN:

                break;
            case GunType.RAILGUN:

                break;
        }
    }

     private void UseAbility2()
     {
        switch (currentGun)
        {
            case GunType.BOLTGUN:
                boltgun.UseAbility2();
                break;
            case GunType.SHOTGUN:

                break;
            case GunType.RAILGUN:

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
        
    }

}
