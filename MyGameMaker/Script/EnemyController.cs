using System;
using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public class EnemyController : MonoBehaviour
{
    //movement variables
    private Transform playerTranform;
    private float distToChase = 20.0f;
    private float minDistToChase = 5.0f;
    private float speedMovement = 5.0f;
    //private Rigidbody rb;
    public float acceleration = 15.0f;
    Vector3 moveDirection;
    private float currentRotationAngle;
    public float rotationSpeed = 300.0f;
    //private Collider collider;
    private Transform enemyTransform;
    private Audio soundAtack;

    //shoting variables
    public float shootCooldown = 2.0f;
    public float projectileSpeed = 90.0f;
    public float projectileLifetime = 0.5f;
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

        playerTranform = GameObject.Find("Player").GetComponent<Transform>();
        //rb = gameObject.GetComponent<Rigidbody>();

        if (playerTranform == null)
        {
            Engineson.print("ERROR: Player couldn't be found!");
        }

        //collider = gameObject.GetComponent<Collider>();
        //if (collider == null)
        //{
        //    Engineson.print("ERROR: PlayerMovement requires a Collider component!");
        //    return;
        //}

        soundAtack = gameObject.GetComponent<Audio>();
        if (soundAtack == null)
        {
            Engineson.print("PlayerShooting: Audio component not found");
        }

        enemyTransform = gameObject.GetComponent<Transform>();
        if (enemyTransform == null)
        {
            Engineson.print("ERROR: PlayerMovement requires a Transform component!");
            return;
        }
    }

    public override void Update(float deltaTime)
    {
        Vector3 playerPos = playerTranform.position;

        if (Vector3.Distance(enemyTransform.position, playerPos) < distToChase)
        {
            if (shootTimer <= 0)
            {
                Shoot();
                soundAtack?.Play();
                shootTimer = shootCooldown;
            }
            else
            {
                shootTimer -= deltaTime;
            }

            if (Vector3.Distance(enemyTransform.position, playerPos) > minDistToChase)
            {
                //Vector3 currentVelocity = rb.GetVelocity();
                moveDirection = Vector3.Normalize(playerPos - gameObject.GetComponent<Transform>().position);
                Vector3 desiredVelocity = moveDirection * speedMovement;

                if (desiredVelocity.LengthSquared() > 0)
                {
                    desiredVelocity = Vector3.Normalize(desiredVelocity) * speedMovement;
                }

                //Vector3 newVelocity = Vector3.Lerp(currentVelocity, desiredVelocity, acceleration * deltaTime);
                //rb.SetVelocity(new Vector3(newVelocity.X, currentVelocity.Y, newVelocity.Z));

                enemyTransform.position += desiredVelocity * deltaTime;
            }
        }
        else
        {
            //rb.SetVelocity(Vector3.Zero);
        }

        if (moveDirection != Vector3.Zero)
        {
            currentRotationAngle = GetComponent<Transform>().eulerAngles.Y;
            float targetAngle = (float)Math.Atan2(moveDirection.X, moveDirection.Z);
            float targetAngleDegrees = targetAngle * (180.0f / (float)Math.PI);

            while (targetAngleDegrees - currentRotationAngle > 180.0f) targetAngleDegrees -= 360.0f;
            while (targetAngleDegrees - currentRotationAngle < -180.0f) targetAngleDegrees += 360.0f;

            currentRotationAngle = Lerp(currentRotationAngle, targetAngleDegrees, rotationSpeed * deltaTime);

            Vector3 eulerRotation = new Vector3(0, currentRotationAngle, 0);
            Quaternion newRotation = Quaternion.CreateFromYawPitchRoll(
                eulerRotation.Y * ((float)Math.PI / 180.0f),
                eulerRotation.X * ((float)Math.PI / 180.0f),
                eulerRotation.Z * ((float)Math.PI / 180.0f)
            );

            enemyTransform.SetRotationQuat(newRotation);
        }

        UpdateProjectiles(deltaTime);
        CleanupProjectiles();
    }
    private float Lerp(float start, float end, float t)
    {
        return start + (end - start) * Math.Min(1, Math.Max(0, t));
    }

    private void Shoot()
    {
        try
        {
            GameObject projectile = Engineson.CreateGameObject("Projectile", null);
            Engineson.print("Projectile created!" + enemyTransform.forward);
            // TODO: add custom mesh to the projectile
            projectile.AddComponent<MeshRenderer>();
            projectile.AddComponent<Collider>();

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