using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public class PlayerShooting : MonoBehaviour
{
    public float shootCooldown = 0.01f;
    public float projectileSpeed = 90.0f;
    public float projectileLifetime = 0.5f;
    private PlayerInput playerInput;
    private Transform transform;
    private float shootTimer = 0f;
    private List<ProjectileInfo> activeProjectiles = new List<ProjectileInfo>();
    
    private Audio sound;

    private class ProjectileInfo
    {
        public GameObject gameObject;
        public Transform transform;
        public float lifetime;
        public Vector3 direction;
        public bool markedForDestruction;

        public ProjectileInfo(GameObject obj, Transform trans, Vector3 dir)
        {
            gameObject = obj;
            transform = trans;
            direction = dir;
            lifetime = 0f;
            markedForDestruction = false;
        }
    }

    public override void Start()
    {
        playerInput = gameObject.GetComponent<PlayerInput>();
        if (playerInput == null)
        {
            Engineson.print("ERROR: PlayerShooting requires a PlayerInput component!");
        }

        transform = gameObject.GetComponent<Transform>();
        if (transform == null)
        {
            Engineson.print("ERROR: PlayerShooting requires a Transform component!");
        }

        sound = gameObject.GetComponent<Audio>();
        if (sound == null)
        {
            Engineson.print("PlayerShooting: Audio component not found");
        }
    }

    public override void Update(float deltaTime)
    {
        shootTimer -= deltaTime;

        if (playerInput?.IsShooting() == true && shootTimer <= 0)
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
            GameObject projectile = Engineson.CreateGameObject("Projectile", null);

            // TODO: add custom mesh to the projectile
            projectile.AddComponent<MeshRenderer>();
            projectile.AddComponent<Collider>();

            sound?.Play();

            if (projectile != null)
            {
                Transform projTransform = projectile.GetComponent<Transform>();
                if (projTransform != null)
                {
                    Vector3 forward = transform.forward;
                    Vector3 spawnPos = transform.position + forward * 1.0f;
                    projTransform.position = spawnPos;
                    projTransform.SetScale(0.1f, 0.1f, 0.1f);

                    ProjectileInfo projInfo = new ProjectileInfo(projectile, projTransform, forward);
                    activeProjectiles.Add(projInfo);

                    Engineson.print("Projectile fired!");
                }
            }
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
            {
                proj.markedForDestruction = true;
                continue;
            }

            try
            {
                if (proj.transform != null)
                {
                    proj.transform.position += proj.direction * projectileSpeed * deltaTime;
                }
            }
            catch (System.Exception e)
            {
                proj.markedForDestruction = true;
                Engineson.print($"Error updating projectile: {e.Message}");
            }
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