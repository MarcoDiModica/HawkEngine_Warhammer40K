using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public class Shotgun : BaseWeapon
{
    private Audio sound;
    private string shotgunShot = "Assets/Audio/SFX/Weapons/Shotgun/ShotgunShot.wav";
    private string shotgunReload = "Assets/Audio/SFX/Weapons/Shotgun/ShotgunReload.wav";
    public override void Start()
    {
        shootCadence = 1f;
        magazineSize = 4;
        currentMagazineAmmo = magazineSize;
        maxAmmo = 50;
        currentTotalAmmo = 16;
        reloadTime = 2.5f;
        ammoType = AmmoType.SHOTGUN;
        transform = gameObject.GetComponent<Transform>();
        sound = gameObject.GetComponent<Audio>();
    }

    public override void Update(float deltaTime)
    {
        //CleanBullets();
    }

    public override void Shoot()
    {
        if (currentMagazineAmmo > 0)
        {
            currentMagazineAmmo--;
            sound?.LoadAudio(shotgunShot);
            sound?.Play();
            // Shoot logic
            int numProjectiles = 5;
            float spreadAngle = 45f;
            float angleStep = spreadAngle / (numProjectiles - 1);
            float startAngle = -spreadAngle / 2;

            for (int i = 0; i < numProjectiles; i++)
            {
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

                        float angle = startAngle + angleStep * i;
                        Vector3 direction = Vector3.Transform(forward, Matrix4x4.CreateRotationY(angle * (3.14f / 180f))); // Convert degrees to radians
                        projTransform.LookAt(direction);
                        projTransform.SetScale(0.1f, 0.1f, 0.1f);

                        projectile.AddScript("BulletData");
                        projectile.GetComponent<BulletData>().Init(projTransform, direction);
                        bullets.Add(projectile.GetComponent<BulletData>());

                        Engineson.print("Projectile fired!");
                    }
                }
            }
        }
    }


    public override void Reload()
    {
        if (currentTotalAmmo > 0)
        {
            sound?.LoadAudio(shotgunReload);
            sound?.Play();
            currentTotalAmmo -= magazineSize - currentMagazineAmmo;
            currentMagazineAmmo = magazineSize;
        }
    }

    public override void UseAbility1()
    {
        
    }

    public override void UseAbility2()
    {

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
                    bullets.RemoveAt(i);
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
