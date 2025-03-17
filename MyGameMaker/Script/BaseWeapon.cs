using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public abstract class BaseWeapon : MonoBehaviour
{
    public float shootCadence;
    public int magazineSize;
    public int currentMagazineAmmo;
    public int maxAmmo;
    public int currentTotalAmmo;
    public float reloadTime;
    public Transform transform;
    public List<BulletData> bullets = new List<BulletData>();


    public enum AmmoType
    {
        BOLTGUN,
        SHOTGUN,
        RAILGUN
    }
    public AmmoType ammoType;

    public abstract void Shoot();

    public abstract void Reload();

    public abstract void UseAbility1();

    public abstract void UseAbility2();

    public abstract void CleanBullets();
}