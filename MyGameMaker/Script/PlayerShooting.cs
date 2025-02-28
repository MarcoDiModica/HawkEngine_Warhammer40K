using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Numerics;
using HawkEngine;

public class PlayerShooting
{
    private GameObject player;
    private float shootCooldown = 0.2f;
    private float projectileSpeed = 20.0f;
    private float projectileLifetime = 1.0f;
    private float shootTimer = 0f;
    private List<GameObject> activeProjectiles = new List<GameObject>();

    public PlayerShooting(GameObject player)
    {
        this.player = player;
    }

    public void UpdateShooting(float deltaTime)
    {
        shootTimer -= deltaTime;
        if (Input.GetKey(KeyCode.J) && shootTimer <= 0)
        {
            Shoot();
            shootTimer = shootCooldown;
        }
    }

    private void Shoot()
    {
        GameObject projectile = Engineson.CreateGameObject("Projectile", null);
        if (projectile != null)
        {
            Transform projTransform = projectile.GetComponent<Transform>();
            if (projTransform != null)
            {
                Vector3 forward = player.GetComponent<Transform>().forward;
                projTransform.position = player.GetComponent<Transform>().position + forward * 1.0f;
                projTransform.SetScale(0.3f, 0.3f, 0.3f);

                activeProjectiles.Add(projectile);
            }
        }
    }

    public void UpdateProjectiles(float deltaTime)
    {
        foreach (var proj in activeProjectiles)
        {
            proj.GetComponent<Transform>().position += proj.GetComponent<Transform>().forward * projectileSpeed * deltaTime;
        }
    }

    public void CleanupProjectiles()
    {
        activeProjectiles.RemoveAll(proj => proj == null);
    }
}
