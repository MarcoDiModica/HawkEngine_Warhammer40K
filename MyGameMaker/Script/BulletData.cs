using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public class BulletData : MonoBehaviour
{
    public Transform transform;
    public float lifetime;
    public Vector3 direction;
    public bool markedForDestruction;
    public float projectileLifetime;
    public float projectileSpeed = 90.0f;

    public void Init(Transform trans, Vector3 dir)
    {

        transform = trans;
        direction = dir;
        lifetime = 0f;
        projectileLifetime = 3;
        markedForDestruction = false;
    }

    public override void Update(float deltaTime)
    {

        lifetime += deltaTime * 10;

        if (lifetime >= projectileLifetime)
        {
            markedForDestruction = true;
        }

        try
        {
            if (transform != null)
            {
                transform.position += direction * projectileSpeed * deltaTime;
            }
        }
        catch (System.Exception e)
        {
            markedForDestruction = true;
            Engineson.print($"Error updating projectile: {e.Message}");
        }
    }
}