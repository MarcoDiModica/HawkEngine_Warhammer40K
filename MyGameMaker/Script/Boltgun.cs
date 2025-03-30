using System.Collections.Generic;
using System.Numerics;
using System.Runtime.CompilerServices;
using HawkEngine;

public class Boltgun : BaseWeapon
{

    GrenadeLauncher grenadeLauncher;
    ArcSnare arcSnare;
    private PlayerController playerController;
    public PlayerData playerData;
    private Audio sound;
    private string boltgunShot = "Assets/Audio/SFX/Weapons/Boltgun/BoltgunShot.wav";
    private string boltgunReload = "Assets/Audio/SFX/Weapons/Boltgun/BoltgunReload.wav";

    private float timeSinceLastShot = 0.0f;

    public override void Awake()
    {
        
    }
    public override void Start()
    {
        shootCadence = 0.1f;
        magazineSize = 30;
        currentMagazineAmmo = magazineSize;
        maxAmmo = 240;
        currentTotalAmmo = 120;
        reloadTime = 1.5f;
        ammoType = AmmoType.BOLTGUN;
        transform = gameObject.GetComponent<Transform>();
        grenadeLauncher = gameObject.GetComponent<GrenadeLauncher>();
        arcSnare = gameObject.GetComponent<ArcSnare>();
        sound = gameObject.GetComponent<Audio>();
        playerController = gameObject.GetComponent<PlayerController>();
        playerData = playerController.playerData;
    }

    public override void Update(float deltaTime)
    {
        CleanBullets();
        timeSinceLastShot += deltaTime;
        //Engineson.print(deltaTime.ToString());
        //Engineson.print(timeSinceLastShot.ToString());
    }

    public override void Shoot()
    {

        if (currentMagazineAmmo > 0 && timeSinceLastShot >= shootCadence)
        {
            timeSinceLastShot = 0f;

            if (!playerData.infiniteBullets)
            {
                currentMagazineAmmo--;
            }

            sound?.LoadAudio(boltgunShot);
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
        // Ability 2 logic
        Engineson.print("Boltgun ability 2 used");
        arcSnare.TriggerAbility();
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
                    bullets .RemoveAt(i);
                }
                catch (System.Exception e)
                {
                    Engineson.print($"Error destroying projectile: {e.Message}");
                    bullets.RemoveAt(i);
                }
            }
        }
    }

}