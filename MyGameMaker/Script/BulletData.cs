using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public class BulletData : MonoBehaviour
{
    public GameObject gameObject;
    public Transform transform;
    public float lifetime;
    public Vector3 direction;
    public bool markedForDestruction;
    public float projectileLifetime = 0.5f;
    public float projectileSpeed = 90.0f;

    public void Init(GameObject obj, Transform trans, Vector3 dir)
    {
        gameObject = obj;
        transform = trans;
        direction = dir;
        lifetime = 0f;
        markedForDestruction = false;
    }

    public override void Update(float deltaTime)
    {

        lifetime += deltaTime;

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

        if (markedForDestruction)
        {
            if (gameObject != null)
            {
                Engineson.Destroy(gameObject);
            }
        }
    }
}