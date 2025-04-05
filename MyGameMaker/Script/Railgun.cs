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
    ToggleMode toggleMode;
    EnergyBall energyBall;
    LaserBeam laserBeam;
    public PlayerData playerData;

    private Audio sound;
    private string railgunReload = "Assets/Audio/SFX/Weapons/Railgun/RailgunCharge.wav";
    private string railgunShot = "Assets/Audio/SFX/Weapons/Railgun/RailgunShot.wav";

    private float timeSinceLastShot = 0.0f;
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
        ammoType = AmmoType.RAILGUN;
        transform = gameObject.GetComponent<Transform>();
        sound = gameObject.GetComponent<Audio>();
        playerController = gameObject.GetComponent<PlayerController>();
        playerData = playerController.playerData;
        toggleMode = gameObject.GetComponent<ToggleMode>();
        energyBall = gameObject.GetComponent<EnergyBall>();
        laserBeam = gameObject.GetComponent<LaserBeam>();
    }

    public override void Update(float deltaTime)
    {
        timeSinceLastShot += deltaTime;

        if (railgunMode == RailgunMode.SEMIAUTOMATIC)
        {

            damage = 100.0f;
            shootCadence = 0.66f;
            magazineSize = 4;
        }
        else
        {
            damage = 50.0f;
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

        for (int i = bulletsPos.Count - 1; i >= 0; i--)
        {
            bulletsPos[i] = LerpVector3(bulletsPos[i], hitPoints[i], 0.1f);
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
                    Engineson.Destroy(bulletsObjects[i]);
                    bulletsObjects.RemoveAt(i);
                    collisionNames.RemoveAt(i);
                }
            }
        }

        //CleanBullets();
    }

    public override void Shoot()
    {
        isReloading = false;
        if (currentMagazineAmmo > 0 && isCooling == false && isRecharged && timeSinceLastShot >= shootCadence)
        {
            timeSinceLastShot = 0f;
            if (!playerData.infiniteBullets)
            {
                currentMagazineAmmo--;
            }
            sound?.LoadAudio(railgunShot);
            sound?.Play();
            // Shoot logic
            RayCast rayBullet = new RayCast();
            Vector3 bulletPosition = transform.GetPosition() + new Vector3(0, 2.5f, 0);

            rayBullet.PerformRaycast(bulletPosition, transform.forward, range);

            var projectile = Engineson.CreateGameObject("Projectile", null);

            // TODO: add custom mesh to the projectile
            projectile.AddComponent<MeshRenderer>();
            projectile.GetComponent<Transform>().SetScale(0.5f, 0.5f, 0.5f);

            bulletsObjects.Add(projectile);

            Vector3 bulletHitPoint = Vector3.Zero;

            if (rayBullet.hit.isHit)
            {
                bulletHitPoint = rayBullet.hit.point;
                collisionNames.Add(rayBullet.hit.gameObject.name);

                Engineson.print($"Hit: {bulletHitPoint}");
            }
            else
            {
                bulletHitPoint = transform.GetPosition() + new Vector3(0, 2.5f, 0) + transform.forward * range;
                collisionNames.Add("Missed");
                Engineson.print("Missed");
            }

            hitPoints.Add(bulletHitPoint);
            bulletsPos.Add(bulletPosition);
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
        sound?.LoadAudio(railgunReload);
        sound?.Play();
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
