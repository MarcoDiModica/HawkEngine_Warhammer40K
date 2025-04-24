using System;
using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public class EnemyControllerRanged : EnemyController
{
    // Perfect Dodge
    private bool dodgewindow = false;
    private float dodgeActivationTime = 0.5f;
    private float dodgeTimer = 0f;
    //private TermagauntAnimation animation
    private PlayerController pc;

    // Audio
    bool isCombatMusicPlaying = false;
    private Audio music;
    private string combatMusic = "Assets/Audio/PlaceHolder_CombatMusic.wav";

    // Enemy Stats
    private float health = 100.0f;
    private float projectileDamage = 15.0f;
    private float acidDamage = 5.0f;
    private float distanceToPlayer;

    // Shooting
    private float projectileRange = 100.0f;
    private List<BulletData> activeProjectiles = new List<BulletData>();
    public float shootCooldown = 2.0f;
    public float projectileSpeed = 90.0f;
    public float projectileLifetime = 0.5f;
    protected float shootTimer = 0f;

    public override void Awake()
    {
        music = gameObject.GetComponent<Audio>();
    }
    public override void Start()
    {
        pc = GameObject.Find("Player").GetComponent<PlayerController>();
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

        sound = gameObject.GetComponent<Audio>();
        if (sound == null)
        {
            Engineson.print("PlayerShooting: Audio component not found");
        }

        enemyTransform = gameObject.GetComponent<Transform>();
        if (enemyTransform == null)
        {
            Engineson.print("ERROR: PlayerMovement requires a Transform component!");
            return;
        }

        particles = gameObject.AddComponent<ParticleFX>();
        //particles.ApplyPreset();

        maxHealth = health;
        currentHealth = maxHealth;
        gameObject.tag = "Ranged";
    }

    public override void Update(float deltaTime)
    {
        if (currentState != EnemyState.DEAD)
        {
            if (currentHealth <= 0)
            {
                currentState = EnemyState.DEAD;
                //anim.SetDeathAnimation();
                sound.LoadAudio("Assets/Audio/SFX/Enemies/Hormagaunt/HormagauntDeath_ready.wav");
                sound?.Play();
                return;
            }

            if (currentState != EnemyState.STUNNED)
            {
                distanceToPlayer = Vector3.Distance(enemyTransform.position, playerTransform.position);

                if (distanceToPlayer < distToChase)
                {
                    // Shoot
                    if (distanceToPlayer < projectileRange)
                    {
                        currentState = EnemyState.ATTACK;
                    }
                    else
                    {
                        // Chase
                        if (distanceToPlayer > minDistToChase)
                        {
                            currentState = EnemyState.CHASE;
                        }
                    }

                    // Rotation
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

                        collider.SetRotation(newRotation);
                    }
                }
                else
                {
                    if (currentState != EnemyState.IDLE)
                    {
                        currentState = EnemyState.IDLE;
                        rb.SetVelocity(Vector3.Zero);
                        //anim.SetStandardIdleAnimation();
                    }
                }
            }
        }

        Engineson.print(gameObject.name + " STATE: " + currentState.ToString());

        switch (currentState)
        {
            case EnemyState.IDLE:
                isFootstepPlaying = false;
                if (!hasStoppedFootsteps)
                {
                    sound?.Stop();
                    hasStoppedFootsteps = true;
                }
                break;

            case EnemyState.CHASE:
                if (!isFootstepPlaying)
                {
                    sound?.LoadAudio("Assets/Audio/SFX/Enemies/Hormagaunt/HormagauntFootstep_ready.wav");
                    sound?.Play(true);
                    isFootstepPlaying = true;
                    hasStoppedFootsteps = false;
                }
                if (isCombatMusicPlaying == false)
                {
                    sound?.LoadAudio(combatMusic);
                    sound?.Play(true);
                    isCombatMusicPlaying = true;
                }

                Vector3 currentVelocity = rb.GetVelocity();
                moveDirection = Vector3.Normalize(playerTransform.position - gameObject.GetComponent<Transform>().position);
                Vector3 desiredVelocity = moveDirection * speedMovement;

                if (desiredVelocity.LengthSquared() > 0)
                {
                    desiredVelocity = Vector3.Normalize(desiredVelocity) * speedMovement;
                }

                Vector3 newVelocity = Vector3.Lerp(currentVelocity, desiredVelocity, acceleration * deltaTime);
                rb.SetVelocity(new Vector3(newVelocity.X, currentVelocity.Y, newVelocity.Z));
                break;

            case EnemyState.ATTACK:
                shootTimer += deltaTime;
                if (shootTimer >= shootCooldown)
                {
                    Attack();
                    sound?.Play();
                    shootTimer = 0;
                }
                break;

            case EnemyState.STUNNED:
                rb.SetVelocity(Vector3.Zero);

                stunTimer += deltaTime;
                if (stunTimer >= stunDuration)
                {
                    currentState = EnemyState.IDLE;
                    stunTimer = 0.0f;
                }
                break;

            case EnemyState.DEAD:
                collider.SetActive(false);
                break;

            default:
                break;
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
                BoxCollider projectileCollider = projectile.GetComponent<BoxCollider>();
                if (projTransform != null)
                {
                    Vector3 forward = moveDirection;
                    Vector3 spawnPos = enemyTransform.position + forward * 1.0f;
                    projTransform.position = spawnPos;
                    projTransform.SetScale(0.1f, 0.1f, 0.1f);

                    projectile.AddScript("BulletData");
                    projectile.GetComponent<BulletData>().Init(projTransform, forward, gameObject);
                    activeProjectiles.Add(projectile.GetComponent<BulletData>());
                    projectileCollider.SetPosition(projTransform.position);
                    Engineson.print("Projectile fired!");
                }
            }
        }
        catch (System.Exception e)
        {
            Engineson.print($"Error creating projectile: {e.Message}");
        }
    }

    public override void TakeDamage(float damage)
    {
        if (currentHealth > 0)
        {
            currentHealth -= damage;
            //anim.SetHitAnimation();
            //particles.ApplyPreset(19);
            //particles.EmitBurst(1);
            sound.LoadAudio("Assets/Audio/SFX/Enemies/Hormagaunt/HormagauntHit_ready.wav");
            sound?.Play();
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
