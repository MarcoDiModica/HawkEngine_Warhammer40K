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
    private List<float> bulletLifetimes = new List<float>();
    private List<Vector3> bulletDirections = new List<Vector3>();
    private List<HashSet<GameObject>> bulletHitEnemies = new List<HashSet<GameObject>>();
    private float hitRayLength = 0.5f;
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
            Vector3 newPos = currentPos + direction * speed * deltaTime;

            bool shouldDestroy = false;

            RayCast ray = new RayCast();
            ray.PerformRaycast(currentPos, direction, hitRayLength);

            if (ray.hit.isHit)
            {
                GameObject hitObject = ray.hit.gameObject;

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
                        {
                            shouldDestroy = true;
                        }
                    }
                }
            }

            bulletsPos[i] = newPos;
            bulletsObjects[i].GetComponent<Transform>().position = newPos;

            if (bulletLifetimes[i] > timeToLerp || shouldDestroy)
            {
                Engineson.Destroy(bulletsObjects[i]);
                bulletsObjects.RemoveAt(i);
                bulletsPos.RemoveAt(i);
                bulletDirections.RemoveAt(i);
                bulletIntervals.RemoveAt(i);
                bulletLifetimes.RemoveAt(i);
                bulletHitEnemies.RemoveAt(i);
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
            float spreadAngle = 45f;
            float angleStep = spreadAngle / (numProjectiles - 1);
            float startAngle = -spreadAngle / 2;

            for (int i = 0; i < numProjectiles; i++)
            {
                float angle = startAngle + angleStep * i;
                Vector3 direction = Vector3.Normalize(Vector3.Transform(transform.forward, Matrix4x4.CreateRotationY(angle * (3.14f / 180f))));

                Vector3 localOffset = new Vector3(0.0f, 2.5f, 0.5f);
                Vector3 bulletStart = transform.position +
                                      (transform.right * localOffset.X) +
                                      (transform.up * localOffset.Y) +
                                      (transform.forward * localOffset.Z);
                bulletStart.Y += 0.5f;

                Vector3 bulletRotation = transform.GetEulerAngles();
                GameObject projectile = Engineson.CreateGameObject("Projectile", null);
                projectile.AddComponent<MeshRenderer>();
                projectile.transform.SetScale(0.2f, 0.2f, 0.2f);
                projectile.transform.position = bulletStart;
                projectile.transform.SetRotation(bulletRotation.X, bulletRotation.Y, bulletRotation.Z);


                bulletsObjects.Add(projectile);
                bulletsPos.Add(bulletStart);
                bulletDirections.Add(direction);
                bulletIntervals.Add(0);
                bulletLifetimes.Add(0);
                bulletHitEnemies.Add(new HashSet<GameObject>());
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
