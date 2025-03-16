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

    // Guns Scripts
    private Boltgun boltgun;

    private Audio sound;

    private enum GunType
    {
        BOLTGUN,
        SHOTGUN,
        RAILGUN
    }
    private GunType currentGun = GunType.BOLTGUN;

    

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
        if (boltgun == null)
        {
            Engineson.print("ERROR: PlayerShooting requires a Boltgun component!");
        }


    }

    public override void Update(float deltaTime)
    {
        
        
        //Engineson.print($"Shoot Timer: {shootTimer}");

        if (playerInput.IsChangingWeapon())
        {
            ChangeWeapon();
        }


        if (playerInput?.IsShooting() == true)
        {
            shootTimer -= deltaTime * 10;
            if (shootTimer <= 0)
            {
                Shoot();
                shootTimer = shootCooldown;
            }
                
            
        }

        if (playerInput?.IsReloading() == true)
        {
            Reload();
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
                    
                    break;
                case GunType.RAILGUN:
                    
                    break;
            }

            sound?.Play();

            
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

                break;
            case GunType.RAILGUN:

                break;
        }
    }

    private void ChangeWeapon()
    {
        switch (currentGun)
        {
            case GunType.BOLTGUN:
                shootCooldown = 1f / boltgun.shootCadence;
                shootTimer = shootCooldown;

                break;
            case GunType.SHOTGUN:

                break;
            case GunType.RAILGUN:

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

}