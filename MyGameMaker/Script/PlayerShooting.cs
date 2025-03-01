using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public class PlayerShooting : MonoBehaviour
{
    public float shootCooldown = 0.2f;
    public float projectileSpeed = 20.0f;
    public float projectileLifetime = 3.0f;
    public Vector3 bulletColor = new Vector3(0.0f, 1.0f, 1.0f);
    public float bulletMass = 0.5f;
    public float bulletScale = 0.3f;

    private PlayerInput playerInput;
    private Transform transform;
    private float shootTimer = 0f;
    private List<ProjectileInfo> activeProjectiles = new List<ProjectileInfo>();

    private class ProjectileInfo
    {
        public GameObject gameObject;
        public Rigidbody rigidbody;
        public float lifetime;
        public bool markedForDestruction;

        public ProjectileInfo(GameObject obj, Rigidbody rb)
        {
            gameObject = obj;
            rigidbody = rb;
            lifetime = 0f;
            markedForDestruction = false;
        }
    }

    public override void Start()
    {
        playerInput = gameObject.GetComponent<PlayerInput>();
        if (playerInput == null)
            Engineson.print("ERROR: PlayerShooting requires a PlayerInput component!");

        transform = gameObject.GetComponent<Transform>();
        if (transform == null)
            Engineson.print("ERROR: PlayerShooting requires a Transform component!");
    }

    public override void Update(float deltaTime)
    {
        shootTimer -= deltaTime;

        if (playerInput != null && playerInput.IsShooting() && shootTimer <= 0)
        {
            Shoot();
            shootTimer = shootCooldown;
        }

        UpdateProjectiles(deltaTime);
        CleanupProjectiles();
    }

    private void Shoot()
    {
        try
        {
            //TODO: Research why the mesh doesnt scales 
            GameObject projectile = Engineson.CreateGameObject("Projectile", null);
            if (projectile == null) return;

            // TODO: add specific mesh to the projectile
            projectile.AddComponent<MeshRenderer>();
            projectile.AddComponent<Collider>();
            Rigidbody rigidbody = projectile.AddComponent<Rigidbody>();

            Transform projTransform = projectile.GetComponent<Transform>();
            if (projTransform == null) return;

            Vector3 forward = transform.forward;
            Vector3 spawnPos = transform.position + forward * 1.0f;
            projTransform.position = spawnPos;
            //projTransform.SetScale(bulletScale, bulletScale, bulletScale);

            if (rigidbody != null)
            {
                rigidbody.SetMass(bulletMass);
                rigidbody.AddForce(forward * projectileSpeed);
                activeProjectiles.Add(new ProjectileInfo(projectile, rigidbody));
            }

            Engineson.print("Projectile fired!");
        }
        catch (System.Exception e)
        {
            Engineson.print($"Error creating projectile: {e.Message}");
        }
    }

    private void UpdateProjectiles(float deltaTime)
    {
        foreach (var proj in activeProjectiles)
        {
            if (proj.markedForDestruction) continue;

            proj.lifetime += deltaTime;

            if (proj.lifetime >= projectileLifetime)
                proj.markedForDestruction = true;
        }
    }

    private void CleanupProjectiles()
    {
        for (int i = activeProjectiles.Count - 1; i >= 0; i--)
        {
            var proj = activeProjectiles[i];
            if (proj.markedForDestruction)
            {
                try
                {
                    Engineson.Destroy(proj.gameObject);
                    activeProjectiles.RemoveAt(i);
                }
                catch (System.Exception e)
                {
                    Engineson.print($"Error destroying projectile: {e.Message}");
                    activeProjectiles.RemoveAt(i);
                }
            }
        }
    }
}
