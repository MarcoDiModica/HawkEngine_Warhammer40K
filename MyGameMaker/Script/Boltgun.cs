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
    private Audio sound;
    private string boltgunShot = "Assets/Audio/SFX/Weapons/Boltgun/BoltgunShot.wav";
    private string boltgunReload = "Assets/Audio/SFX/Weapons/Boltgun/BoltgunReload.wav";
    GameObject projectile;
    private RedThirstManager redThirstManager;
    private float timeSinceLastShot = 0.0f;

    private List<Vector3> bulletDirections = new List<Vector3>();
    List<HashSet<GameObject>> bulletHitEnemies = new List<HashSet<GameObject>>();

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
        range = 20f;
        timeToLerp = 0.5f;
        ammoType = AmmoType.BOLTGUN;
        transform = gameObject.GetComponent<Transform>();
        grenadeLauncher = gameObject.GetComponent<GrenadeLauncher>();
        arcSnare = gameObject.GetComponent<ArcSnare>();
        sound = gameObject.GetComponent<Audio>();
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
            Vector3 direction = Vector3.Normalize(transform.forward);
            bulletDirections.Add(direction);

            float speed = range / timeToLerp;
            Vector3 newPos = currentPos + bulletsObjects[i].transform.forward * speed * deltaTime;

            bool shouldDestroy = false; //  inicializamos en falso

            RayCast ray = new RayCast();
            ray.PerformRaycast(currentPos, bulletDirections[i], hitRayLength);

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
                            redThirstManager.OnWeaponUsed();

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

                        // Determinar si la bala debe destruirse:
                        if (!playerData.isPiercing)
                        {
                            shouldDestroy = true;
                        }
                        else
                        {
                            // Si es perforante pero impacta algo que no es un enemigo, también se destruye
                            if (tag != "Melee" && tag != "Ranged" && tag != "Boss" && tag != "Destroyable")
                            {
                                shouldDestroy = true;
                            }
                        }
                    }

                }
            }

            // Actualizar posición
            bulletsPos[i] = newPos;
            bulletsObjects[i].GetComponent<Transform>().position = newPos;

            // Si debe destruirse o ha pasado su tiempo de vida
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

            sound?.LoadAudio(boltgunShot);
            sound?.Play();

            Vector3 localOffset = new Vector3(0.0f, 2.5f, 0.5f); // Y = altura, Z = hacia adelante, X = lateral si se desea

            Vector3 bulletStart = transform.position +
                                  (transform.right * localOffset.X) +
                                  (transform.up * localOffset.Y) +
                                  (transform.forward * localOffset.Z);
            bulletStart.Y += 0.5f;
            Vector3 direction = transform.forward;
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

    public override void Reload()
    {
        if (currentTotalAmmo > 0)
        {
            sound?.LoadAudio(boltgunReload);
            sound?.Play();
           
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