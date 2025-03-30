using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public class BulletData : MonoBehaviour
{
    public GameObject owner;
    public Transform transform;
    public float lifetime;
    public Vector3 direction;
    public bool markedForDestruction;
    public float projectileLifetime;
    public float projectileSpeed = 0.5f;

    public void Init(Transform trans, Vector3 dir, GameObject own)
    {
        owner = own;
        transform = trans;
        direction = dir;
        lifetime = 0f;
        projectileLifetime = 3;
        markedForDestruction = false;
    }

    public override void Awake()
    {
        
    }
    public override void Start()
    {

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