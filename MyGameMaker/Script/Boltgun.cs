using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public class Boltgun : BaseWeapon
{
    public override void Start()
    {
        shootCadence = 3f;
        magazineSize = 30;
        currentMagazineAmmo = magazineSize;
        maxAmmo = 240;
        currentTotalAmmo = 120;
        reloadTime = 1.5f;
        ammoType = AmmoType.BOLTGUN;
        transform = gameObject.GetComponent<Transform>();
    }

    public override void Shoot()
    {
        if (currentMagazineAmmo > 0)
        {
            currentMagazineAmmo--;
            // Shoot logic
            GameObject projectile = Engineson.CreateGameObject("Projectile", null);

            // TODO: add custom mesh to the projectile
            projectile.AddComponent<MeshRenderer>();
            projectile.AddComponent<Collider>();

            if (projectile != null)
            {
                Transform projTransform = projectile.GetComponent<Transform>();
                if (projTransform != null)
                {
                    Vector3 forward = transform.forward;
                    Vector3 spawnPos = transform.position + forward * 1.0f;
                    projTransform.position = spawnPos;
                    projTransform.SetScale(0.1f, 0.1f, 0.1f);

                    projectile.AddScript("BulletData");
                    projectile.GetComponent<BulletData>().Init(projectile, projTransform, forward);

                    Engineson.print("Projectile fired!");
                }
            }
        }
        
    }

    public override void Reload()
    {
        if (currentTotalAmmo > 0)
        {
            currentTotalAmmo -= magazineSize - currentMagazineAmmo;
            currentMagazineAmmo = magazineSize;
            Engineson.print("Boltgun reloaded");
            Engineson.print($"Current ammo: {currentTotalAmmo}");
        }
    }

    public override void UseAbility1()
    {
        // Ability 1 logic
        Engineson.print("Boltgun ability 1 used");
    }

    public override void UseAbility2()
    {
        // Ability 2 logic
        Engineson.print("Boltgun ability 2 used");
    }

    
}