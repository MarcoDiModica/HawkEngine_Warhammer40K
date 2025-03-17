using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public class Boltgun : BaseWeapon
{

    GrenadeLauncher grenadeLauncher;

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
        grenadeLauncher = gameObject.GetComponent<GrenadeLauncher>();
    }

    public override void Update(float deltaTime)
    {
        CleanBullets();
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
                    projectile.GetComponent<BulletData>().Init(projTransform, forward);
                    bullets.Add(projectile.GetComponent<BulletData>());

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
        grenadeLauncher.TriggerAbility();
    }

    public override void UseAbility2()
    {
        // Ability 2 logic
        Engineson.print("Boltgun ability 2 used");
    }

    public override void CleanBullets()
    {
        for (int i = bullets.Count - 1; i >= 0; i--)
        {
            var proj = bullets[i];
            if (proj.markedForDestruction)
            {
                try
                {
                    Engineson.Destroy(proj.gameObject);
                    bullets .RemoveAt(i);
                }
                catch (System.Exception e)
                {
                    Engineson.print($"Error destroying projectile: {e.Message}");
                    bullets.RemoveAt(i);
                }
            }
        }
    }

}