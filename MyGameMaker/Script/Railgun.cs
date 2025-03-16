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

    public enum RailgunMode
    {
        SEMIAUTOMATIC,
        AUTOMATIC
    }

    public RailgunMode railgunMode = RailgunMode.SEMIAUTOMATIC;

    public override void Start()
    {
        shootCadence = 0.66f;
        magazineSize = 4;
        currentMagazineAmmo = magazineSize;
        maxAmmo = 0;
        currentTotalAmmo = 0;
        reloadTime = 2f;
        ammoType = AmmoType.RAILGUN;
        transform = gameObject.GetComponent<Transform>();
    }

    public override void Update(float deltaTime)
    {

        if (railgunMode == RailgunMode.SEMIAUTOMATIC)
        {
            shootCadence = 0.66f;
            magazineSize = 4;
        }
        else
        {
            shootCadence = 2f;
            magazineSize = 10;
        }

        if (isCooling)
        {
            coolTimer += deltaTime * 10;
            if (coolTimer >= coolingTime)
            {
                Cooling();
            }
        }

        if (isReloading)
        {
            reloadTimer += deltaTime * 10;
            if (reloadTimer >= reloadTime)
            {
                Reload();
            }
        }
    }

    public override void Shoot()
    {
        isReloading = false;
        if (currentMagazineAmmo > 0 && isCooling == false && isRecharged)
        {
            currentMagazineAmmo--;
            // Shoot logic
            GameObject projectile = Engineson.CreateGameObject("Projectile", null);

            // TODO: add custom mesh to the projectile
            projectile.AddComponent<MeshRenderer>();
            projectile.AddComponent<Collider>();

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
                    projectile.GetComponent<BulletData>().Init(projectile, projTransform, forward);

                    Engineson.print("Projectile fired!");
                }
            }
        }
        else if (currentMagazineAmmo <= 0)
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

    }

    public override void UseAbility2()
    {

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
}
