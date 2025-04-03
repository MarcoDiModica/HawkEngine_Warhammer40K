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
    public float range;
    public Transform transform;
    public List<BulletData> bullets = new List<BulletData>();
    public List<Vector3> bulletsPos = new List<Vector3>();
    public List<Vector3> hitPoints = new List<Vector3>();

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

    public abstract void ResetCooldowns();
    public abstract void CleanBullets();

    public Vector3 LerpVector3(Vector3 start, Vector3 end, float t)
    {
        t = Clamp01(t);
        return new Vector3(
            start.X + (end.X - start.X) * t,
            start.Y + (end.Y - start.Y) * t,
            start.Z + (end.Z - start.Z) * t
        );
    }

    public float Clamp01(float value)
    {
        if (value < 0) return 0;
        if (value > 1) return 1;
        return value;
    }
}