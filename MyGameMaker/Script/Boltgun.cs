using System.Collections.Generic;
using System.Diagnostics;
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
        range = 20f;
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

        for (int i = 0; i < bulletsPos.Count; i++)
        {
            bulletsPos[i] = LerpVector3(bulletsPos[i], hitPoints[i], 0.1f);
            Engineson.print($"Bullet {i} position: {bulletsPos[i]}");
        }
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
            
            RayCast rayBullet = new RayCast();
            Vector3 bulletPosition = transform.GetPosition() + new Vector3(0, 1f, 0);

            rayBullet.PerformRaycast(bulletPosition, transform.forward, range);

            
            Vector3 bulletHitPoint = Vector3.Zero;

            if (rayBullet.hit.isHit)
            {
                bulletHitPoint = rayBullet.hit.point;
                Engineson.print($"Hit: {bulletHitPoint}");
            }
            else
            {
                bulletHitPoint = transform.GetPosition() + transform.forward * range;
                Engineson.print("Missed");
            }

            hitPoints.Add(bulletHitPoint);
            bulletsPos.Add(bulletPosition);
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
        for (int i = 0; i < bulletsPos.Count; i++)
        {
            if (Vector3.Distance(bulletsPos[i], hitPoints[i]) < 0.1f)
            {
                bulletsPos.RemoveAt(i);
                hitPoints.RemoveAt(i);
                i--;
            }
        }
    }

    public override void ResetCooldowns()
    {
       arcSnare.ResetCooldowns();
        grenadeLauncher.ResetCooldowns();
    }

}