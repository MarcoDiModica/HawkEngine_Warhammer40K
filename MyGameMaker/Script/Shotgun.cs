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
        damage = 70.0f;
        shootCadence = 0.7f;
        magazineSize = 4;
        currentMagazineAmmo = magazineSize;
        maxAmmo = 50;
        currentTotalAmmo = 16;
        reloadTime = 2.5f;
        range = 20f;
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
        CleanBullets();
        timeSinceLastShot += deltaTime;

        for (int i = 0; i < bulletsPos.Count; i++)
        {
            bulletsPos[i] = LerpVector3(bulletsPos[i], hitPoints[i], 0.1f);
            
        }
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
                Vector3 forward = transform.forward;
                Vector3 spawnPos = transform.position + forward * 1.0f;

                float angle = startAngle + angleStep * i;
                Vector3 direction = Vector3.Transform(forward, Matrix4x4.CreateRotationY(angle * (3.14f / 180f))); // Convert degrees to radians

                RayCast rayBullet = new RayCast();
                Vector3 bulletPosition = transform.GetPosition() + new Vector3(0, 1f, 0);

                rayBullet.PerformRaycast(bulletPosition, direction, range);


                Vector3 bulletHitPoint = Vector3.Zero;

                if (rayBullet.hit.isHit)
                {
                    bulletHitPoint = rayBullet.hit.point;
                    collisionNames.Add(rayBullet.hit.gameObject.name);
                    Engineson.print($"Hit: {bulletHitPoint}");
                }
                else
                {
                    bulletHitPoint = transform.GetPosition() + transform.forward * range;
                    collisionNames.Add("Missed");
                    Engineson.print("Missed");
                }

                hitPoints.Add(bulletHitPoint);
                bulletsPos.Add(bulletPosition);
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
        for (int i = 0; i < bulletsPos.Count; i++)
        {
            if (Vector3.Distance(bulletsPos[i], hitPoints[i]) < 0.1f)
            {
                bulletsPos.RemoveAt(i);
                hitPoints.RemoveAt(i);
                if (collisionNames[i] == "Missed")
                {
                    collisionNames.RemoveAt(i);
                }
                else
                {
                    // Aquí se ejecuta la función de daño al enemigo
                    Engineson.print($"Bullet {i} hit: {collisionNames[i]}");
                    collisionNames.RemoveAt(i);
                }
                i--;
            }
        }
    }

    public override void ResetCooldowns()
    {
        hookShot.ResetCooldowns();
        barrage.ResetCooldowns();
    }
}
