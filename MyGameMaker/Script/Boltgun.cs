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
    GameObject projectile;

    private float timeSinceLastShot = 0.0f;

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
        timeToLerp = 1;
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
        
        timeSinceLastShot += deltaTime;

        for (int i = bulletsPos.Count - 1; i >= 0; i--)
        {
            bulletIntervals[i] += deltaTime;
            bulletsPos[i] = LerpVector3(bulletsPos[i], hitPoints[i], bulletIntervals[i] / timeToLerp);
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
                    bulletIntervals.RemoveAt(i);
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
                        //enemy.GetComponent<EnemyControllerStalker>().TakeDamage(damage); //placeholder damage
                    }
                    if (enemy.tag == "Boss")
                    {
                        enemy.GetComponent<EnemyControllerBoss>().TakeDamage(damage); //placeholder damage
                    }
                    if (enemy.tag == "Destroyable")
                    {
                        enemy.GetComponent<DestroyEnviormentObject>().DestroyObject(); 
                    }

                    Engineson.Destroy(bulletsObjects[i]);
                    bulletsObjects.RemoveAt(i);
                    collisionNames.RemoveAt(i);
                    bulletIntervals.RemoveAt(i);
                }
            }
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
            Vector3 bulletPosition = transform.GetPosition() + new Vector3(0, 2.5f, 0);

            rayBullet.PerformRaycast(bulletPosition, transform.forward, range);

            var projectile = Engineson.CreateGameObject("Projectile", null);

            // TODO: add custom mesh to the projectile
            projectile.AddComponent<MeshRenderer>();
            projectile.GetComponent<Transform>().SetScale(0.2f, 0.2f, 0.2f);

            //var bulletFX = Engineson.CreateGameObject("BulletFX", null);
            //bulletFX.AddComponent<ParticleFX>().ApplyPreset(12);
            //bulletFX.GetComponent<ParticleFX>().EmitBurst(5);

            //projectile.AddChild(bulletFX);
            bulletsObjects.Add(projectile);
            bulletIntervals.Add(0);

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