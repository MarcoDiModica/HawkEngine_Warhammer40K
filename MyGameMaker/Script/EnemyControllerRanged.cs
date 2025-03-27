using System;
using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public class EnemyControllerRanged : EnemyController
{
    private float shootTimer = 0f;
    private List<ProjectileInfo> activeProjectiles = new List<ProjectileInfo>();

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
        // Initialize components specific to EnemyControllerRanged
        playerTransform = GetComponent<Transform>();
        rb = GetComponent<Rigidbody>();
        collider = GetComponent<BoxCollider>();
        enemyTransform = GetComponent<Transform>();
        soundAttack = GetComponent<Audio>();
        shootTimer = 0f;
    }

    public override void Update(float deltaTime)
    {
        // Implement update logic specific to EnemyControllerRanged
    }

    public override void Attack()
    {
        // Implement attack logic specific to EnemyControllerRanged
    }

    private void Shoot()
    {
        try
        {
            GameObject projectile = Engineson.CreateGameObject("Projectile", null);
            Engineson.print("Projectile created!" + enemyTransform.forward);
            // TODO: add custom mesh to the projectile
            projectile.AddComponent<MeshRenderer>();
            projectile.AddComponent<BoxCollider>();

            //sound?.Play();

            if (projectile != null)
            {
                Transform projTransform = projectile.GetComponent<Transform>();
                if (projTransform != null)
                {
                    Vector3 forward = moveDirection;
                    Vector3 spawnPos = enemyTransform.position + forward * 1.0f;
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
