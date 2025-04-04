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

        CleanBullets();
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
            GameObject projectile = Engineson.CreateGameObject("Projectile", null);

            // TODO: add custom mesh to the projectile
            projectile.AddComponent<MeshRenderer>();
            projectile.AddComponent<BoxCollider>();

            if (projectile != null)
            {
                Transform projTransform = projectile.GetComponent<Transform>();
                if (projTransform != null)
                {
                    Vector3 forward = transform.forward;
                    Vector3 spawnPos = transform.position + forward * 1.0f;
                    projTransform.position = spawnPos;
                    projTransform.SetScale(0.1f, 0.1f, 0.1f);

                    projectile.AddScript("BulletData");
                    projectile.GetComponent<BulletData>().Init(projTransform, forward, gameObject);
                    bullets.Add(projectile.GetComponent<BulletData>());

                    Engineson.print("Projectile fired!");
                }
            }
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
