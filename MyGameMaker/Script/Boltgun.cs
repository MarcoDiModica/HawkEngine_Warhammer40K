using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Numerics;
using System.Runtime.CompilerServices;
using HawkEngine;

public class Boltgun : BaseWeapon
{

    public GrenadeLauncher grenadeLauncher;
    public ArcSnare arcSnare;
    private PlayerController playerController;
    public PlayerData playerData;
    //private Audio sound;
   // private string boltgunShot = "Assets/Audio/SFX/Weapons/Boltgun/BoltgunShot.wav";
    //private string boltgunReload = "Assets/Audio/SFX/Weapons/Boltgun/BoltgunReload.wav";
    GameObject projectile;
    private RedThirstManager redThirstManager;
    private float timeSinceLastShot = 0.0f;

    private List<Vector3> bulletDirections = new List<Vector3>();
    List<HashSet<GameObject>> bulletHitEnemies = new List<HashSet<GameObject>>();
    private List<Vector3> bulletStartPositions = new List<Vector3>();

    private List<float> bulletLifetimes = new List<float>();
    private float bulletSpeed = 60f;
    private float maxLifetime = 2f;
    private float hitRayLength = 1f;
    public override void Awake()
    {
        
    }
    public override void Start()
    {
        damage = 20.0f; 
        shootCadence = 0.1f;
        magazineSize = 30;
        currentMagazineAmmo = magazineSize;
        maxAmmo = 240;
        currentTotalAmmo = 120;
        reloadTime = 1.5f;
        range = 30f;
        timeToLerp = 0.1f;
        ammoType = AmmoType.BOLTGUN;
        transform = gameObject.GetComponent<Transform>();
        grenadeLauncher = gameObject.GetComponent<GrenadeLauncher>();
        arcSnare = gameObject.GetComponent<ArcSnare>();
      //  sound = gameObject.GetComponent<Audio>();
        playerController = gameObject.GetComponent<PlayerController>();
        playerData = playerController.playerData;
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

          //  sound?.LoadAudio(boltgunShot);
          //  sound?.Play();

            Vector3 localOffset = new Vector3(0.0f, 2.5f, 0.5f); // Y = altura, Z = hacia adelante, X = lateral si se desea

            Vector3 bulletStart = transform.position +
                                  (transform.right * localOffset.X) +
                                  (transform.up * localOffset.Y) +
                                  (transform.forward * localOffset.Z);
            bulletStart.Y += 0.5f;
            Vector3 direction = Vector3.Normalize(transform.forward);

            // Calcular rotaci�n desde la direcci�n (LookAt-like)
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
            playerController.playerShooting.rifleShotFX.EmitBurst(1);
        }

    }

    public override void Reload()
    {
        if (currentTotalAmmo > 0)
        {
           // sound?.LoadAudio(boltgunReload);
           // sound?.Play();
           
            if(currentTotalAmmo >= magazineSize)
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
            Engineson.print("Boltgun reloaded");
            Engineson.print($"Current ammo: {currentTotalAmmo}");
        }
    }

    public override void UseAbility1()
    {

        Engineson.print("Boltgun ability 1 used");
        grenadeLauncher.TriggerAbility();
    }

    public override void UseAbility2()
    {
        
        Engineson.print("Boltgun ability 2 used");
        arcSnare.TriggerAbility();
    }

    public override void CleanBullets()
    {
        
    }

    public override void ResetCooldowns()
    {
       arcSnare.ResetCooldowns();
        grenadeLauncher.ResetCooldowns();
    }

}