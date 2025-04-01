using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public class Shotgun : BaseWeapon
{
    private Audio sound;
    private string shotgunShot = "Assets/Audio/SFX/Weapons/Shotgun/ShotgunShot.wav";
    private string shotgunReload = "Assets/Audio/SFX/Weapons/Shotgun/ShotgunReload.wav";


    private PlayerController playerController;
    public PlayerData playerData;
    Barrage barrage;
    HookShot hookShot;

    private float timeSinceLastShot = 0.0f;

    public override void Awake()
    {

    }
    public override void Start()
    {
        shootCadence = 0.7f;
        magazineSize = 4;
        currentMagazineAmmo = magazineSize;
        maxAmmo = 50;
        currentTotalAmmo = 16;
        reloadTime = 2.5f;
        ammoType = AmmoType.SHOTGUN;
        transform = gameObject.GetComponent<Transform>();
        sound = gameObject.GetComponent<Audio>();
        playerController = gameObject.GetComponent<PlayerController>();
        playerData = playerController.playerData;
        barrage = gameObject.GetComponent<Barrage>();
        hookShot = gameObject.GetComponent<HookShot>();
        
    }

    public override void Update(float deltaTime)
    {
        //CleanBullets();
        timeSinceLastShot += deltaTime;
    }

    public override void Shoot()
    {
        if (currentMagazineAmmo > 0 && timeSinceLastShot >= shootCadence)
        {
            timeSinceLastShot = 0f; // Reiniciar contador

            if (!playerData.infiniteBullets)
            {
                currentMagazineAmmo--;
            }

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
                projectile.AddComponent<BoxCollider>();

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
                        projectile.GetComponent<BulletData>().Init(projTransform, direction, gameObject);
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

            if (currentTotalAmmo >= magazineSize)
            {
                currentMagazineAmmo = magazineSize;
                currentTotalAmmo = currentTotalAmmo - magazineSize;
            }
            else
            {
                currentMagazineAmmo = currentTotalAmmo;
                currentTotalAmmo = 0;
            }
            currentTotalAmmo -= magazineSize - currentMagazineAmmo;
            Engineson.print("Shotgun reloaded");
            Engineson.print($"Current ammo: {currentTotalAmmo}");
        }
    }

    public override void UseAbility1()
    {
        hookShot.TriggerAbility();
    }

    public override void UseAbility2()
    {
        barrage.TriggerAbility();
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

    public override void ResetCooldowns()
    {
        hookShot.ResetCooldowns();
        barrage.ResetCooldowns();
    }
}
