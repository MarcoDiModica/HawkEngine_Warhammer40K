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
        timeToLerp = 2;
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
        timeSinceLastShot += deltaTime;

        for (int i = bulletsPos.Count - 1; i >= 0; i--)
        {
            bulletIntervals[i] += deltaTime;
            bulletsPos[i] = LerpVector3(bulletsPos[i], hitPoints[i], bulletIntervals[i] / timeToLerp);
            bulletsObjects[i].GetComponent<Transform>().position = bulletsPos[i];

            if (Vector3.Distance(bulletsPos[i], hitPoints[i]) < 0.5f)
            {
                bulletsPos.RemoveAt(i);
                hitPoints.RemoveAt(i);
                if (collisionNames[i] == "Missed")
                {
                    Engineson.Destroy(bulletsObjects[i]);
                    bulletsObjects.RemoveAt(i);
                    collisionNames.RemoveAt(i);
                    bulletIntervals.RemoveAt(i);
                }
                else
                {
                    Engineson.print($"Bullet {i} hit: {collisionNames[i]}");
                    var enemy = GameObject.Find(collisionNames[i]);
                    if (enemy.tag == "Melee")
                    {
                        enemy.GetComponent<EnemyControllerMelee>().TakeDamage(damage); //placeholder damage
                    }
                    if (enemy.tag == "Ranged")
                    {
                        enemy.GetComponent<EnemyControllerRanged>().TakeDamage(damage); //placeholder damage
                    }
                    if (enemy.tag == "Stalker")
                    {
                        enemy.GetComponent<EnemyControllerStalker>().TakeDamage(damage); //placeholder damage
                    }
                    if (enemy.tag == "Boss")
                    {
                        enemy.GetComponent<EnemyControllerBoss>().TakeDamage(damage); //placeholder damage
                    }
                    if (enemy.tag == "Destroyable")
                    {
                        enemy.GetComponent<DestroyEnviormentObject>().DestroyObject();
                    }
                    Engineson.Destroy(bulletsObjects[i]);
                    bulletsObjects.RemoveAt(i);
                    collisionNames.RemoveAt(i);
                    bulletIntervals.RemoveAt(i);
                }
            }
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
                Vector3 bulletPosition = transform.GetPosition() + new Vector3(0, 2.5f, 0);

                rayBullet.PerformRaycast(bulletPosition, direction, range);

                var projectile = Engineson.CreateGameObject("Projectile", null);

                // TODO: add custom mesh to the projectile
                projectile.AddComponent<MeshRenderer>();
                projectile.GetComponent<Transform>().SetScale(0.5f, 0.5f, 0.5f);

                bulletsObjects.Add(projectile);
                bulletIntervals.Add(0);


                Vector3 bulletHitPoint = Vector3.Zero;

                if (rayBullet.hit.isHit)
                {
                    bulletHitPoint = rayBullet.hit.point;
                    collisionNames.Add(rayBullet.hit.gameObject.name);
                   
                    Engineson.print($"Hit: {bulletHitPoint}");
                }
                else
                {
                    bulletHitPoint = bulletPosition + direction * range;
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
        
    }

    public override void ResetCooldowns()
    {
        hookShot.ResetCooldowns();
        barrage.ResetCooldowns();
    }
}
