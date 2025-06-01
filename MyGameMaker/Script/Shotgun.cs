using System;
using System.Collections.Generic;
using System.Numerics;
using System.Runtime.InteropServices;
using HawkEngine;

public class Shotgun : BaseWeapon
{
    private const string shotgunShot = "Assets/Audio/SFX/Weapons/Shotgun/ShotgunShot.wav";
    private const string shotgunReload = "Assets/Audio/SFX/Weapons/Shotgun/ShotgunReload.wav";
    private bool strongShot = false;

    private PlayerController playerController;
    public PlayerData playerData;
    public Barrage barrage;
    public HookShot hookShot;
    private RedThirstManager redThirstManager;
    private PlayerInput playerInput;

    public float timeSinceLastShot = 0.0f;
    private List<Vector3> bulletDirections = new List<Vector3>();
    private List<HashSet<GameObject>> bulletHitEnemies = new List<HashSet<GameObject>>();
    private List<Vector3> bulletStartPositions = new List<Vector3>();
    private List<float> bulletLifetimes = new List<float>();
    private float hitRayLength = 1f;

    //private ShakeManager shakeManager;
    public float shakeIntensity = 0.40f;
    public float shakeDuration = 0.25f;
    public float shakeSpeed = 0.2f;

    private bool isReloading = false;
    private float reloadTimer = 0.0f;
    private float strongshotTimer = 0.0f;
    private float strongshotCooldown = 1.5f;

    private bool componentsInitialized = false;

    public override void Awake()
    {
    }

    public override void Start()
    {
        try
        {
            // Initialize weapon properties
            damage = 20.0f;
            shootCadence = 0.4f;
            magazineSize = 8;
            currentMagazineAmmo = magazineSize;
            maxAmmo = 50;
            currentTotalAmmo = 24;
            reloadTime = 0.75f;
            range = 20f;
            timeToLerp = 0.3f;
            ammoType = AmmoType.SHOTGUN;

            // Get required components safely
            transform = gameObject.GetComponent<Transform>();
            if (transform == null)
            {
                Engineson.print("ERROR: Transform component not found on Shotgun");
                return;
            }

            playerController = gameObject.GetComponent<PlayerController>();
            if (playerController == null)
            {
                Engineson.print("ERROR: PlayerController component not found on Shotgun");
                return;
            }

            playerData = playerController.playerData;
            if (playerData == null)
            {
                Engineson.print("ERROR: PlayerData not found on PlayerController");
                return;
            }

            playerInput = gameObject.GetComponent<PlayerInput>();
            if (playerInput == null)
            {
                Engineson.print("ERROR: PlayerInput component not found on Shotgun");
                return;
            }

            barrage = gameObject.GetComponent<Barrage>();
            if (barrage == null)
                Engineson.print("WARNING: Barrage component not found on Shotgun");

            hookShot = gameObject.GetComponent<HookShot>();
            if (hookShot == null)
                Engineson.print("WARNING: HookShot component not found on Shotgun");

            redThirstManager = gameObject.GetComponent<RedThirstManager>();
            if (redThirstManager == null)
                Engineson.print("WARNING: RedThirstManager component not found on Shotgun");

            componentsInitialized = true;

            Engineson.print("Shotgun initialized successfully");
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR initializing Shotgun: {e.Message}");
        }
    }

    public override void Update(float deltaTime)
    {
        if (!componentsInitialized)
            return;

        try
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

            if (strongShot)
            {
                strongshotTimer += deltaTime;
                if (strongshotTimer >= strongshotCooldown)
                {
                    Engineson.print("Damage reset");
                    damage = damage / 2;
                    strongShot = false;
                    strongshotTimer = 0.0f;
                }
            }

            if (bulletsObjects.Count != bulletsPos.Count ||
                bulletsObjects.Count != bulletDirections.Count ||
                bulletsObjects.Count != bulletIntervals.Count ||
                bulletsObjects.Count != bulletLifetimes.Count ||
                bulletsObjects.Count != bulletHitEnemies.Count ||
                bulletsObjects.Count != bulletStartPositions.Count)
            {
                Engineson.print("ERROR: Bullet collections out of sync, cleaning bullets");
                CleanBullets();
                return;
            }

            for (int i = bulletsObjects.Count - 1; i >= 0; i--)
            {
                if (i >= bulletsObjects.Count || bulletsObjects[i] == null)
                {
                    continue;
                }

                try
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
                                if (redThirstManager != null)
                                {
                                    redThirstManager.OnShotgunUsed();
                                    if (redThirstManager.IsInBlackRage())
                                        finalDamage += redThirstManager.redThirstBonus;
                                }

                                Engineson.print("Hit for this amount:" + finalDamage);

                                switch (tag)
                                {
                                    case "Melee":
                                        EnemyControllerMelee melee = hitObject.GetComponent<EnemyControllerMelee>();
                                        if (melee != null) melee.TakeDamage(finalDamage);
                                        if (strongShot) melee.getStunned();
                                        break;
                                    case "Ranged":
                                        EnemyControllerRanged ranged = hitObject.GetComponent<EnemyControllerRanged>();
                                        if (ranged != null) ranged.TakeDamage(finalDamage);
                                        if (strongShot) ranged.getStunned();
                                        break;
                                    case "Stalker":
                                        EnemyControllerStalker stalker = hitObject.GetComponent<EnemyControllerStalker>();
                                        if (stalker != null) stalker.TakeDamage(finalDamage);
                                        if (strongShot) stalker.getStunned();
                                        break;
                                    case "Boss":
                                        EnemyControllerBoss boss = hitObject.GetComponent<EnemyControllerBoss>();
                                        if (boss != null) boss.TakeDamage(finalDamage);
                                        break;
                                    case "Warrior":
                                        EnemyControllerWarrior warrior = hitObject.GetComponent<EnemyControllerWarrior>();
                                        if (warrior != null) warrior.TakeDamage(finalDamage);
                                        if (strongShot) warrior.getStunned();
                                        break;
                                    case "Destroyable":
                                        DestroyEnviormentObject destroyable = hitObject.GetComponent<DestroyEnviormentObject>();
                                        if (destroyable != null) destroyable.DestroyObject();
                                        break;
                                }
                            }

                            if (playerData != null &&
                                (!playerData.isPiercing || (playerData.isPiercing && tag != "Melee" && tag != "Ranged" && tag != "Boss" && tag != "Warrior")))
                            {
                                shouldDestroy = true;
                            }
                        }
                    }

                    bulletsPos[i] = newPos;

                    Transform bulletTransform = bulletsObjects[i].GetComponent<Transform>();
                    if (bulletTransform != null)
                    {
                        bulletTransform.position = newPos;
                    }

                    float distanceTraveled = Vector3.Distance(bulletStartPositions[i], newPos);
                    if (distanceTraveled > range || shouldDestroy || bulletLifetimes[i] > 2.0f)
                    {
                        RemoveBulletAtIndex(i);
                    }
                }
                catch (Exception e)
                {
                    Engineson.print($"ERROR updating bullet {i}: {e.Message}");
                    RemoveBulletAtIndex(i);
                }
            }
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in Shotgun.Update: {e.Message}");
        }
    }

    private void RemoveBulletAtIndex(int index)
    {
        if (index < 0 || index >= bulletsObjects.Count)
            return;

        try
        {
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
        catch (Exception e)
        {
            Engineson.print($"ERROR removing bullet at index {index}: {e.Message}");
            CleanBullets();
        }
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
        if (!componentsInitialized)
            return;

        try
        {
            if (currentMagazineAmmo > 0 && timeSinceLastShot >= shootCadence && !isReloading)
            {
                //shakeManager.ApplyShake(shakeIntensity, shakeDuration, shakeSpeed);

                timeSinceLastShot = 0f;

                if (playerData != null && !playerData.infiniteBullets)
                    currentMagazineAmmo--;

                int audio = Audio.PlayOneShot(shotgunShot);
                int numProjectiles = 5;
                float maxSpreadAngle = 5f;

                Random random = new Random();

                for (int i = 0; i < numProjectiles; i++)
                {

                    Vector3 baseDirection;
                    if (playerInput.GetCurrentLookDirection() != Vector3.Zero)
                    {
                        baseDirection = playerInput.GetCurrentLookDirection();
                    }
                    else
                    {
                        baseDirection = Vector3.Normalize(transform.forward);
                    }

                    float randomYaw = (float)(random.NextDouble() * 2 * maxSpreadAngle - maxSpreadAngle);   // izquierda/derecha
                    float randomPitch = (float)(random.NextDouble() * 2 * maxSpreadAngle - maxSpreadAngle); // arriba/abajo

                    float yawRad = randomYaw * (float)(Math.PI / 180f);
                    float pitchRad = randomPitch * (float)(Math.PI / 180f);

                    Matrix4x4 rotationMatrix = Matrix4x4.CreateFromYawPitchRoll(yawRad, pitchRad, 0);
                    Vector3 direction = Vector3.Normalize(Vector3.Transform(baseDirection, rotationMatrix));

                    Vector3 localOffset = new Vector3(-0.18f, 2.5f, 2f);
                    Vector3 bulletStart = transform.position +
                                          (transform.right * localOffset.X) +
                                          (transform.up * localOffset.Y) +
                                          (transform.forward * localOffset.Z);
                    bulletStart.Y += 0.5f;

                    float yaw = (float)(Math.Atan2(direction.X, direction.Z) * (180.0 / Math.PI));
                    float pitch = (float)(-Math.Asin(direction.Y) * (180.0 / Math.PI));

                    GameObject projectile = Engineson.CreateGameObject("ShotgunProjectile", null);
                    if (projectile == null)
                    {
                        Engineson.print("ERROR: Failed to create shotgun projectile");
                        continue;
                    }

                    Transform projectileTransform = projectile.GetComponent<Transform>();
                    if (projectileTransform != null)
                    {
                        projectileTransform.SetScale(0.25f, 0.25f, 0.25f);
                        projectileTransform.position = bulletStart;
                        projectileTransform.SetRotation(pitch, yaw, 0f);
                    }

                    ParticleFX particleFX = projectile.AddComponent<ParticleFX>();
                    if (particleFX != null)
                    {
                        particleFX.ApplyPreset(14);
                        particleFX.EmitBurst(1);
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
                        playerController.playerShooting.shotgunShotFX != null)
                    {
                        playerController.playerShooting.shotgunShotFX.EmitBurst(1);
                    }
                }
            }
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in Shotgun.Shoot: {e.Message}");
        }
    }

    public override void Reload()
    {
        if (!componentsInitialized)
            return;

        try
        {
            if (currentTotalAmmo > 0 && currentMagazineAmmo != magazineSize)
            {
                isReloading = true;
                int audioo = Audio.PlayOneShot(shotgunReload);
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

                Engineson.print("Shotgun reloaded");
                Engineson.print($"Current ammo: {currentTotalAmmo}");
            }
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in Shotgun.Reload: {e.Message}");
        }
    }

    public override void UseAbility1()
    {
        if (!componentsInitialized)
            return;

        try
        {
            if (!strongShot)
            {
                damage = damage * 2;
                strongShot = true;
            }
            Shoot();
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in Shotgun.UseAbility1: {e.Message}");
        }
    }

    public override void UseAbility2()
    {
        hookShot.TriggerAbility();
    }

    public override void CleanBullets()
    {
        try
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

            Engineson.print("All shotgun bullets cleaned");
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in Shotgun.CleanBullets: {e.Message}");
        }
    }

    public override void ResetCooldowns()
    {
        if (!componentsInitialized)
            return;

        try
        {
            if (barrage != null)
            {
                barrage.ResetCooldowns();
            }
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in Shotgun.ResetCooldowns: {e.Message}");
        }
    }
}