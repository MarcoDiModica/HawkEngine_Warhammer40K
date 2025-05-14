using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public class Railgun : BaseWeapon
{

    public bool isCooling = false;
    public bool isRecharged = true;
    public bool isReloading = false;
    private float coolingTime = 3f;
    private float coolTimer = 0f;
    private float reloadTimer = 0f;
    private PlayerController playerController;
    public ToggleMode toggleMode;
    public EnergyBall energyBall;
    LaserBeam laserBeam;
    public PlayerData playerData;

    private RedThirstManager redThirstManager;

    private const string  railgunReload = "Assets/Audio/SFX/Weapons/Railgun/Energy_Ball_dissapear.wav";
    private const string railgunShot = "Assets/Audio/SFX/Weapons/Railgun/Energy_ball_hit.wav";

    private float timeSinceLastShot = 0.0f;

    private List<Vector3> bulletDirections = new List<Vector3>();
    private List<HashSet<GameObject>> bulletHitEnemies = new List<HashSet<GameObject>>();
    private List<Vector3> bulletStartPositions = new List<Vector3>();
    private List<float> bulletLifetimes = new List<float>();
    private float bulletSpeed = 90f;
    private float maxLifetime = 1.5f;

    //private ShakeManager shakeManager;
    public float shakeIntensity = 0.4f;
    public float shakeDuration = 0.25f;
    public float shakeSpeed = 0.2f;
    public enum RailgunMode
    {
        SEMIAUTOMATIC,
        AUTOMATIC
    }

    public RailgunMode railgunMode = RailgunMode.SEMIAUTOMATIC;

    public override void Awake()
    {

    }
    public override void Start()
    {
        damage = 100.0f;
        shootCadence = 0.66f;
        magazineSize = 4;
        currentMagazineAmmo = magazineSize;
        maxAmmo = 0;
        currentTotalAmmo = 0;
        reloadTime = 2f;
        range = 25f;
        timeToLerp = 2;
        ammoType = AmmoType.RAILGUN;
        transform = gameObject.GetComponent<Transform>();
        playerController = gameObject.GetComponent<PlayerController>();
        playerData = playerController.playerData;
        toggleMode = gameObject.GetComponent<ToggleMode>();
        energyBall = gameObject.GetComponent<EnergyBall>();
        laserBeam = gameObject.GetComponent<LaserBeam>();
        redThirstManager = gameObject.GetComponent<RedThirstManager>();
        //shakeManager = GameObject.Find("ShakeManager")?.GetComponent<ShakeManager>();
        //if (shakeManager == null)
        //{
        //    Engineson.print("ERROR: ShakeManager not found");
        //}

    }

    public override void Update(float deltaTime)
    {
        if (timeSinceLastShot <= shootCadence + 0.5f)
        {
            timeSinceLastShot += deltaTime;
        }

        if (railgunMode == RailgunMode.AUTOMATIC)
        {

            damage = 50.0f;
            shootCadence = 0.66f;
            magazineSize = 4;
        }
        else
        {
            damage = 100.0f;
            shootCadence = 2f;
            magazineSize = 10;
        }

        if (isCooling)
        {
            coolTimer += deltaTime;
            if (coolTimer >= coolingTime)
            {
                Cooling();
            }
        }

        if (isReloading)
        {
            reloadTimer += deltaTime;
            if (reloadTimer >= reloadTime)
            {
                Reload();
            }
        }

        for (int i = bulletsObjects.Count - 1; i >= 0; i--)
        {
            bulletLifetimes[i] += deltaTime;

            Vector3 currentPos = bulletsPos[i];
            Vector3 direction = bulletDirections[i];
            Vector3 displacement = direction * bulletSpeed * deltaTime;
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
                            case "Stalker":
                                hitObject.GetComponent<EnemyControllerStalker>()?.TakeDamage(finalDamage);
                                break;
                            case "Boss":
                                hitObject.GetComponent<EnemyControllerBoss>()?.TakeDamage(finalDamage);
                                break;
                            case "Warrior":
                                hitObject.GetComponent<EnemyControllerWarrior>()?.TakeDamage(finalDamage);
                                break;
                            case "Destroyable":
                                hitObject.GetComponent<DestroyEnviormentObject>()?.DestroyObject();
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
            bulletsObjects[i].GetComponent<Transform>().position = newPos;

            float distanceTraveled = Vector3.Distance(bulletStartPositions[i], newPos);
            if (distanceTraveled > range || shouldDestroy || bulletLifetimes[i] > maxLifetime)
            {
                Engineson.Destroy(bulletsObjects[i]);
                bulletsObjects.RemoveAt(i);
                bulletsPos.RemoveAt(i);
                bulletDirections.RemoveAt(i);
                bulletLifetimes.RemoveAt(i);
                bulletHitEnemies.RemoveAt(i);
                bulletStartPositions.RemoveAt(i);
            }
        };
    }

    public override void Shoot()
    {
        isReloading = false;

        if (currentMagazineAmmo > 0 && !isCooling && isRecharged && timeSinceLastShot >= shootCadence)
        {
            //shakeManager.ApplyShake(shakeIntensity, shakeDuration, shakeSpeed);

            timeSinceLastShot = 0f;
            if (!playerData.infiniteBullets)
                currentMagazineAmmo--;

            int audio = Audio.PlayOneShot(railgunShot);

            Vector3 localOffset = new Vector3(-0.9f, 2.5f, 2f);
            Vector3 bulletStart = transform.position +
                                  (transform.right * localOffset.X) +
                                  (transform.up * localOffset.Y) +
                                  (transform.forward * localOffset.Z);

            bulletStart.Y += 0.5f;
            Vector3 direction = Vector3.Normalize(transform.forward);

            float yaw = (float)(System.Math.Atan2(direction.X, direction.Z) * (180.0 / System.Math.PI));
            float pitch = (float)(-System.Math.Asin(direction.Y) * (180.0 / System.Math.PI));

            GameObject projectile = Engineson.CreateGameObject("RailProjectile", null);
            //projectile.AddComponent<MeshRenderer>();
            projectile.transform.SetScale(0.25f, 0.25f, 1.5f);
            projectile.transform.position = bulletStart;
            projectile.transform.SetRotation(pitch, yaw, 0f);
            projectile.AddComponent<ParticleFX>();
            if (railgunMode == RailgunMode.AUTOMATIC)
            {
                projectile.GetComponent<ParticleFX>().ApplyPreset(13);
                projectile.GetComponent<ParticleFX>().EmitBurst(1);
            }
            else
            {
                projectile.AddComponent<ParticleFX>().ApplyPreset(14);
                projectile.GetComponent<ParticleFX>().EmitBurst(1);
            }
         
            bulletsObjects.Add(projectile);
            bulletsPos.Add(bulletStart);
            bulletDirections.Add(direction);
            bulletStartPositions.Add(bulletStart);
            bulletHitEnemies.Add(new HashSet<GameObject>());
            bulletLifetimes.Add(0f);
          
        }

        if (currentMagazineAmmo <= 0)
        {
            isCooling = true;
            isRecharged = false;
        }
    }

    public void Cooling()
    {
        isCooling = false;
        isRecharged = true;
        coolTimer = 0f;
        currentMagazineAmmo = magazineSize;
        int audioo = Audio.PlayOneShot(railgunReload);
    }

    public override void Reload()
    {
        isReloading = false;
        reloadTimer = 0f;
        currentMagazineAmmo++;
        if (currentMagazineAmmo >= magazineSize)
        {
            currentMagazineAmmo = magazineSize;
        }
    }

    public override void UseAbility1()
    {
        toggleMode.TriggerAbility();
    }

    public override void UseAbility2()
    {
        if (railgunMode == RailgunMode.AUTOMATIC)
        {
            laserBeam.TriggerAbility();
        }
        else
        {
            energyBall.TriggerAbility();
        }
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

    public void ChangeMode()
    {
        if (railgunMode == RailgunMode.SEMIAUTOMATIC)
        {
            railgunMode = RailgunMode.AUTOMATIC;
        }
        else
        {
            railgunMode = RailgunMode.SEMIAUTOMATIC;
        }
    }

    public override void ResetCooldowns()
    {
    }
}
