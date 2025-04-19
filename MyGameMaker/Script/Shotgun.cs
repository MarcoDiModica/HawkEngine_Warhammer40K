using System;
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
    public Barrage barrage;
    public HookShot hookShot;
    private RedThirstManager redThirstManager;

    private float timeSinceLastShot = 0.0f;
    private List<Vector3> bulletDirections = new List<Vector3>();
    private List<HashSet<GameObject>> bulletHitEnemies = new List<HashSet<GameObject>>();
    private List<Vector3> bulletStartPositions = new List<Vector3>();
    private List<float> bulletLifetimes = new List<float>();
    private float hitRayLength = 1f;
    public override void Awake()
    {

    }
    public override void Start()
    {
        damage = 12.0f;
        shootCadence = 0.7f;
        magazineSize = 4;
        currentMagazineAmmo = magazineSize;
        maxAmmo = 50;
        currentTotalAmmo = 16;
        reloadTime = 2.5f;
        range = 20f;
        timeToLerp = 0.3f;
        ammoType = AmmoType.SHOTGUN;
        transform = gameObject.GetComponent<Transform>();
        sound = gameObject.GetComponent<Audio>();
        playerController = gameObject.GetComponent<PlayerController>();
        playerData = playerController.playerData;
        barrage = gameObject.GetComponent<Barrage>();
        hookShot = gameObject.GetComponent<HookShot>();
        redThirstManager = gameObject.GetComponent<RedThirstManager>();
    }

    public override void Update(float deltaTime)
    {
        timeSinceLastShot += deltaTime;

        for (int i = bulletsObjects.Count - 1; i >= 0; i--)
        {
            bulletIntervals[i] += deltaTime;
            bulletLifetimes[i] += deltaTime;

            Vector3 currentPos = bulletsPos[i];
            Vector3 direction = bulletDirections[i];
            float speed = range / timeToLerp;
            Vector3 displacement = direction * speed * deltaTime;
            Vector3 newPos = currentPos + displacement;

            bool shouldDestroy = false;
            GameObject hitObject = null;

            RayCast ray = new RayCast();
            ray.PerformRaycast(currentPos, direction, displacement.Length());

            if (ray.hit.isHit)
                hitObject = ray.hit.gameObject;

            if (hitObject != null)
            {
                string tag = hitObject.tag;
                if (tag != "PowerUp" && tag != "Ammunition" && tag != "Player")
                {
                    if (!bulletHitEnemies[i].Contains(hitObject))
                    {
                        bulletHitEnemies[i].Add(hitObject);

                        float finalDamage = damage;
                        redThirstManager.OnShotgunUsed();

                        if (redThirstManager.IsInBlackRage())
                            finalDamage += redThirstManager.redThirstBonus;

                        switch (tag)
                        {
                            case "Melee":
                                hitObject.GetComponent<EnemyControllerMelee>()?.TakeDamage(finalDamage);
                                break;
                            case "Ranged":
                                hitObject.GetComponent<EnemyControllerRanged>()?.TakeDamage(finalDamage);
                                break;
                            case "Boss":
                                hitObject.GetComponent<EnemyControllerBoss>()?.TakeDamage(finalDamage);
                                break;
                            case "Destroyable":
                                hitObject.GetComponent<DestroyEnviormentObject>()?.DestroyObject();
                                break;
                        }
                    }

                    if (!playerData.isPiercing || (playerData.isPiercing && tag != "Melee" && tag != "Ranged" && tag != "Boss"))
                        shouldDestroy = true;
                }
            }

            bulletsPos[i] = newPos;
            bulletsObjects[i].GetComponent<Transform>().position = newPos;

            float distanceTraveled = Vector3.Distance(bulletStartPositions[i], newPos);
            if (distanceTraveled > range || shouldDestroy)
            {
                Engineson.Destroy(bulletsObjects[i]);
                bulletsObjects.RemoveAt(i);
                bulletsPos.RemoveAt(i);
                bulletDirections.RemoveAt(i);
                bulletIntervals.RemoveAt(i);
                bulletLifetimes.RemoveAt(i);
                bulletHitEnemies.RemoveAt(i);
                bulletStartPositions.RemoveAt(i);
            }
        }
    }

    public override void Shoot()
    {
        if (currentMagazineAmmo > 0 && timeSinceLastShot >= shootCadence)
        {
            timeSinceLastShot = 0f;

            if (!playerData.infiniteBullets)
                currentMagazineAmmo--;

            sound?.LoadAudio(shotgunShot);
            sound?.Play();

            int numProjectiles = 5;
            float maxSpreadAngle = 5f;

            Random random = new Random();

            for (int i = 0; i < numProjectiles; i++)
            {
                Vector3 baseDirection = transform.forward;

                // Obtener valores aleatorios entre -maxSpreadAngle y +maxSpreadAngle
                float randomYaw = (float)(random.NextDouble() * 2 * maxSpreadAngle - maxSpreadAngle);   // izquierda/derecha
                float randomPitch = (float)(random.NextDouble() * 2 * maxSpreadAngle - maxSpreadAngle); // arriba/abajo

                // Convertir variaciones a radianes
                float yawRad = randomYaw * (float)(Math.PI / 180f);
                float pitchRad = randomPitch * (float)(Math.PI / 180f);

                // Aplicar rotación a la dirección base
                Matrix4x4 rotationMatrix = Matrix4x4.CreateFromYawPitchRoll(yawRad, pitchRad, 0);
                Vector3 direction = Vector3.Normalize(Vector3.Transform(baseDirection, rotationMatrix));

                // Posición inicial del proyectil
                Vector3 localOffset = new Vector3(0.0f, 2.5f, 0.5f);
                Vector3 bulletStart = transform.position +
                                      (transform.right * localOffset.X) +
                                      (transform.up * localOffset.Y) +
                                      (transform.forward * localOffset.Z);
                bulletStart.Y += 0.5f;

                // Rotación del proyectil
                float yaw = (float)(Math.Atan2(direction.X, direction.Z) * (180.0 / Math.PI));
                float pitch = (float)(-Math.Asin(direction.Y) * (180.0 / Math.PI));

                GameObject projectile = Engineson.CreateGameObject("Projectile", null);
                projectile.AddComponent<MeshRenderer>();
                projectile.transform.SetScale(0.2f, 0.2f, 0.2f);
                projectile.transform.position = bulletStart;
                projectile.transform.SetRotation(pitch, yaw, 0f);

                bulletsObjects.Add(projectile);
                bulletsPos.Add(bulletStart);
                bulletDirections.Add(direction);
                bulletIntervals.Add(0);
                bulletLifetimes.Add(0);
                bulletHitEnemies.Add(new HashSet<GameObject>());
                bulletStartPositions.Add(bulletStart);
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
