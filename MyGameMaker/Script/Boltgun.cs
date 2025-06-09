using System;
using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public class Boltgun : BaseWeapon
{
    public GrenadeLauncher grenadeLauncher;
    public ArcSnare arcSnare;
    private PlayerController playerController;
    public PlayerData playerData;
    private PlayerInput playerInput;

    private const string boltgunShot = "Assets/Audio/SFX/Weapons/Boltgun/Boltgun_hit_enviroment.wav";
    private const string boltgunShotEnemy = "Assets/Audio/SFX/Weapons/Boltgun/Boltgun_hit_enemy.wav";
    private const string boltgunReload = "Assets/Audio/SFX/Weapons/Boltgun/BoltgunReload.wav";
    GameObject projectile;
    private RedThirstManager redThirstManager;
    private float timeSinceLastShot = 0.0f;

    private List<Vector3> bulletDirections = new List<Vector3>();
    List<HashSet<GameObject>> bulletHitEnemies = new List<HashSet<GameObject>>();
    private List<Vector3> bulletStartPositions = new List<Vector3>();

    private List<float> bulletLifetimes = new List<float>();
    private float bulletSpeed = 60f;
    private float maxLifetime = 4f;
    private float hitRayLength = 2f;

    //private ShakeManager shakeManager;
    public float shakeIntensity = 0.15f;
    public float shakeDuration = 0.2f;
    public float shakeSpeed = 0.2f;

    private bool isReloading = false;
    private float reloadTimer = 0.0f;

    public ParticleFX bulletcasingFX;

    //bullet spresd
    private float bulletSpreadAngle = 6.5f;
    private static readonly Random random = new Random();

    public override void Awake()
    {

    }
    public override void Start()
    {
        playerController = gameObject.GetComponent<PlayerController>();
        playerData = playerController.playerData;
        if (SceneManager.loadAmmo)
        {
            currentMagazineAmmo = playerData.boltgunCurrentAmmo;
            currentTotalAmmo = playerData.boltgunMaxAmmo;
        }
        else
        {
            currentMagazineAmmo = magazineSize;
            currentTotalAmmo = 180;
        }
        damage = 14.0f;
        shootCadence = 0.12f;
        magazineSize = 40;
        maxAmmo = 240;
        reloadTime = 0.5f;
        range = 80f;
        timeToLerp = 0.45f;
        ammoType = AmmoType.BOLTGUN;
        transform = gameObject.GetComponent<Transform>();
        grenadeLauncher = gameObject.GetComponent<GrenadeLauncher>();
        arcSnare = gameObject.GetComponent<ArcSnare>();
 
        redThirstManager = gameObject.GetComponent<RedThirstManager>();
        playerInput = gameObject.GetComponent<PlayerInput>();
        //shakeManager = GameObject.Find("ShakeManager")?.GetComponent<ShakeManager>();
        //if (shakeManager == null)
        //{
        //    Engineson.print("ERROR: ShakeManager not found");
        //}
        bulletcasingFX = GameObject.Find("BulletCasingFX").GetComponent<ParticleFX>();
        bulletcasingFX.ApplyPreset(44);
        bulletcasingFX.Stop();
    }

    public override void Update(float deltaTime)
    {
        if (timeSinceLastShot <= shootCadence + 0.5f)
        {
            timeSinceLastShot += deltaTime;
        }

        if (isReloading)
        {
            reloadTimer += deltaTime;
            if (reloadTimer >= reloadTime)
            {
                isReloading = false;
                reloadTimer = 0.0f;
            }
        }

        if (bulletsObjects.Count != bulletsPos.Count ||
            bulletsObjects.Count != bulletDirections.Count ||
            bulletsObjects.Count != bulletIntervals.Count ||
            bulletsObjects.Count != bulletLifetimes.Count ||
            bulletsObjects.Count != bulletHitEnemies.Count ||
            bulletsObjects.Count != bulletStartPositions.Count)
        {
            CleanBullets();
            return;
        }

        for (int i = bulletsObjects.Count - 1; i >= 0; i--)
        {
            if (bulletsObjects[i] == null)
            {
                RemoveBulletAtIndex(i);
                continue;
            }

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
            {
                hitObject = ray.hit.gameObject;
            }

            if (hitObject != null)
            {
                string tag = hitObject.tag;

                if (tag != "PowerUp" && tag != "Ammunition" && tag != "Player")
                {
                    if (!bulletHitEnemies[i].Contains(hitObject))
                    {
                        bulletHitEnemies[i].Add(hitObject);

                        float finalDamage = damage;

                        if (redThirstManager.IsInBlackRage())
                            finalDamage += redThirstManager.redThirstBonus;

                        switch (tag)
                        {
                            case "Melee":
                                if (hitObject.GetComponent<EnemyControllerMelee>() != null)
                                    hitObject.GetComponent<EnemyControllerMelee>().TakeDamage(finalDamage);
                                break;
                            case "Ranged":
                                if (hitObject.GetComponent<EnemyControllerRanged>() != null)
                                    hitObject.GetComponent<EnemyControllerRanged>().TakeDamage(finalDamage);
                                break;
                            case "Stalker":
                                if (hitObject.GetComponent<EnemyControllerStalker>() != null)
                                    hitObject.GetComponent<EnemyControllerStalker>().TakeDamage(finalDamage);
                                break;
                            case "Boss":
                                if (hitObject.GetComponent<EnemyControllerBoss>() != null)
                                    hitObject.GetComponent<EnemyControllerBoss>().TakeDamage(finalDamage);
                                break;
                            case "Warrior":
                                if (hitObject.GetComponent<EnemyControllerWarrior>() != null)
                                    hitObject.GetComponent<EnemyControllerWarrior>().TakeDamage(finalDamage);
                                break;
                            case "Destroyable":
                                if (hitObject.GetComponent<DestroyEnviormentObject>() != null)
                                    hitObject.GetComponent<DestroyEnviormentObject>().DestroyObject();
                                break;
                            case "ExplosiveBarrel":
                                hitObject.GetComponent<ExplosiveBarrel>()?.Explode();
                                break;
                        }
                    }

                    if (!playerData.isPiercing || (playerData.isPiercing && tag != "Melee" && tag != "Ranged" && tag != "Boss" && tag != "Warrior"))
                    {
                        shouldDestroy = true;
                    }
                }
            }

            bulletsPos[i] = newPos;

            if (bulletsObjects[i] != null)
            {
                Transform bulletTransform = bulletsObjects[i].GetComponent<Transform>();
                if (bulletTransform != null)
                {
                    bulletTransform.position = newPos;
                }
            }

            float distanceTraveled = Vector3.Distance(bulletStartPositions[i], newPos);
            if (distanceTraveled > range || shouldDestroy || bulletLifetimes[i] >= maxLifetime)
            {
                RemoveBulletAtIndex(i);
            }
        }
    }

    private void RemoveBulletAtIndex(int index)
    {
        if (index < 0 || index >= bulletsObjects.Count)
            return;

        if (bulletsObjects[index] != null)
        {
            Engineson.Destroy(bulletsObjects[index]);
        }

        bulletsObjects.RemoveAt(index);
        bulletsPos.RemoveAt(index);
        bulletDirections.RemoveAt(index);
        bulletIntervals.RemoveAt(index);
        bulletLifetimes.RemoveAt(index);
        bulletHitEnemies.RemoveAt(index);
        bulletStartPositions.RemoveAt(index);
    }

    public int GetCurrentAmmo()
    {
        return currentMagazineAmmo;
    }

    public int GetMaxAmmo()
    {
        return currentTotalAmmo;
    }

    public override void Shoot()
    {
        if (currentMagazineAmmo > 0 && timeSinceLastShot >= shootCadence && !isReloading)
        {
            timeSinceLastShot = 0f;

            if (!playerData.infiniteBullets)
                currentMagazineAmmo--;

            int audio = Audio.PlayOneShot(boltgunShot);
            Vector3 localOffset = new Vector3(-0.18f, 2.5f, 0.5f);

            Vector3 bulletStart = transform.position +
                                  (transform.right * localOffset.X) +
                                  (transform.up * localOffset.Y) +
                                  (transform.forward * localOffset.Z);
            bulletStart.Y += 0.5f;

            Vector3 direction;
            if (playerInput.GetCurrentLookDirection() != Vector3.Zero)
            {
                direction = playerInput.GetCurrentLookDirection();
            }
            else
            {
                direction = Vector3.Normalize(transform.forward);
            }

            float spreadYaw = ((float)random.NextDouble() - 0.5f) * bulletSpreadAngle;
            float spreadPitch = ((float)random.NextDouble() - 0.5f) * bulletSpreadAngle;

            Matrix4x4 spreadMatrix =
                Matrix4x4.CreateFromAxisAngle(transform.up, (float)Math.PI / 180f * spreadYaw) *
                Matrix4x4.CreateFromAxisAngle(transform.right, (float)Math.PI / 180f * spreadPitch);

            direction = Vector3.TransformNormal(direction, spreadMatrix);
            direction = Vector3.Normalize(direction);

            float yaw = (float)(Math.Atan2(direction.X, direction.Z) * (180.0 / Math.PI));
            float pitch = (float)(-Math.Asin(direction.Y) * (180.0 / Math.PI));

            GameObject projectile = Engineson.CreateGameObject("BoltgunProjectile", null);
            if (projectile == null)
            {
                Engineson.print("ERROR: Failed to create bullet projectile");
                return;
            }

            projectile.transform.SetScale(0.25f, 0.25f, 0.25f);
            projectile.transform.position = bulletStart;
            projectile.transform.SetRotation(pitch, yaw, 0f);
            projectile.AddComponent<ParticleFX>();
            ParticleFX particleFX = projectile.GetComponent<ParticleFX>();
            if (particleFX != null)
            {


                particleFX.ApplyPreset(51);
                particleFX.SetParticleStartRotation(yaw);               
                particleFX.EmitBurst(1);
                bulletcasingFX.EmitBurst(1);

            }

            bulletsObjects.Add(projectile);
            bulletsPos.Add(bulletStart);
            bulletDirections.Add(direction);
            bulletIntervals.Add(0);
            bulletLifetimes.Add(0);
            bulletHitEnemies.Add(new HashSet<GameObject>());
            bulletStartPositions.Add(bulletStart);

            if (playerController != null &&
                playerController.playerShooting != null &&
                playerController.playerShooting.rifleShotFX != null)
            {
                playerController.playerShooting.rifleShotFX.EmitBurst(1);
            }
        }
    }

    public override void Reload()
    {
        if (currentTotalAmmo > 0 && currentMagazineAmmo != magazineSize)
        {
            isReloading = true;
            int audioo = Audio.PlayOneShot(boltgunReload);
            if (currentMagazineAmmo + currentTotalAmmo >= magazineSize)
            {
                currentTotalAmmo -= magazineSize - currentMagazineAmmo;
                currentMagazineAmmo = magazineSize;
            }
            else
            {
                currentMagazineAmmo += currentTotalAmmo;
                currentTotalAmmo = 0;
            }

            Engineson.print("Boltgun reloaded");
            Engineson.print($"Current ammo: {currentTotalAmmo}");
        }
    }

    public override void UseAbility1()
    {
        Engineson.print("Boltgun ability 1 used");
        if (grenadeLauncher != null)
        {
            grenadeLauncher.TriggerAbility();
        }
    }

    public override void UseAbility2()
    {
        Engineson.print("Boltgun ability 2 used");
        if (arcSnare != null)
        {
            arcSnare.TriggerAbility();
        }
    }

    public override void CleanBullets()
    {
        for (int i = 0; i < bulletsObjects.Count; i++)
        {
            if (bulletsObjects[i] != null)
            {
                Engineson.Destroy(bulletsObjects[i]);
            }
        }

        bulletsObjects.Clear();
        bulletsPos.Clear();
        bulletDirections.Clear();
        bulletIntervals.Clear();
        bulletLifetimes.Clear();
        bulletHitEnemies.Clear();
        bulletStartPositions.Clear();

        Engineson.print("Cleaned all bullets");
    }

    public override void ResetCooldowns()
    {
        if (arcSnare != null)
        {
            arcSnare.ResetCooldowns();
        }

        if (grenadeLauncher != null)
        {
            grenadeLauncher.ResetCooldowns();
        }
    }
}