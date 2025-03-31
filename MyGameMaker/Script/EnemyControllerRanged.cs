using System;
using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public class EnemyControllerRanged : EnemyController
{
    private List<BulletData> activeProjectiles = new List<BulletData>();
    public float shootCooldown = 2.0f;
    public float projectileSpeed = 90.0f;
    public float projectileLifetime = 0.5f;
    protected float shootTimer = 0f;

    //stats
    private float health = 100.0f;
    private float damage = 20.0f;



    public override void Awake()
    {
        
    }
    public override void Start()
    {

        playerTransform = GameObject.Find("Player").GetComponent<Transform>();
        rb = gameObject.GetComponent<Rigidbody>();

        if (playerTransform == null)
        {
            Engineson.print("ERROR: Player couldn't be found!");
        }

        collider = gameObject.GetComponent<BoxCollider>();
        if (collider == null)
        {
            Engineson.print("ERROR: PlayerMovement requires a Collider component!");
            return;
        }

        soundAttack = gameObject.GetComponent<Audio>();
        if (soundAttack == null)
        {
            Engineson.print("PlayerShooting: Audio component not found");
        }

        enemyTransform = gameObject.GetComponent<Transform>();
        if (enemyTransform == null)
        {
            Engineson.print("ERROR: PlayerMovement requires a Transform component!");
            return;
        }

        maxHealth = 100.0f;
        currentHealth = maxHealth;
    }

    public override void Update(float deltaTime)
    {
        if (!isStunned)
        {
            Vector3 playerPos = playerTransform.position;

            if (Vector3.Distance(enemyTransform.position, playerPos) < distToChase)
            {
                if (shootTimer <= 0)
                {
                    Attack();
                    soundAttack?.Play();
                    shootTimer = shootCooldown;
                }
                else
                {
                    shootTimer -= deltaTime;
                }

                if (Vector3.Distance(enemyTransform.position, playerPos) > minDistToChase)
                {
                    Vector3 currentVelocity = rb.GetVelocity();
                    moveDirection = Vector3.Normalize(playerPos - gameObject.GetComponent<Transform>().position);
                    Vector3 desiredVelocity = moveDirection * speedMovement;

                    if (desiredVelocity.LengthSquared() > 0)
                    {
                        desiredVelocity = Vector3.Normalize(desiredVelocity) * speedMovement;
                    }

                    Vector3 newVelocity = Vector3.Lerp(currentVelocity, desiredVelocity, acceleration * deltaTime);
                    rb.SetVelocity(new Vector3(newVelocity.X, currentVelocity.Y, newVelocity.Z));

                    //enemyTransform.position += desiredVelocity * deltaTime;
                }
            }
            else
            {
                rb.SetVelocity(Vector3.Zero);
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

                // enemyTransform.SetRotationQuat(newRotation);
                collider.SetRotation(newRotation);
            }

        }
        else if (isStunned)
        {
            stunTimer += deltaTime;
            rb.SetVelocity(Vector3.Zero);
            if (stunTimer >= stunDuration)
            {
                isStunned = false;
                stunTimer = 0.0f;
            }
        }

        UpdateProjectiles(deltaTime);
        CleanupProjectiles();
    }

    public override void Attack()
    {
        try
        {
            GameObject projectile = Engineson.CreateGameObject("Projectile", null);
            Engineson.print("Projectile created!" + enemyTransform.forward);
            // TODO: add custom mesh to the projectile
            projectile.AddComponent<MeshRenderer>();
            projectile.AddComponent<BoxCollider>();

            //sound?.Play();
            projectile.tag = "EnemyAttack";

            if (projectile != null)
            {
                Transform projTransform = projectile.GetComponent<Transform>();
                if (projTransform != null)
                {
                    Vector3 forward = moveDirection;
                    Vector3 spawnPos = enemyTransform.position + forward * 1.0f;
                    projTransform.position = spawnPos;
                    projTransform.SetScale(0.1f, 0.1f, 0.1f);

                    projectile.AddScript("BulletData");
                    projectile.GetComponent<BulletData>().Init(projTransform, forward, gameObject);
                    activeProjectiles.Add(projectile.GetComponent<BulletData>());

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
