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
    private PlayerController pc;
    //stats
    private float health = 100.0f;
    private float damage = 20.0f;

    //audio
    bool isCombatMusicPlaying = false;
//     private AudioSource music;
//     private string combatMusic = "Assets/Audio/PlaceHolder_CombatMusic.wav";
//     private AudioClip combatSound;

    public override void Awake()
    {
        //music = gameObject.GetComponent<AudioSource>();
        startPosition = gameObject.GetComponent<Transform>().position;
    }
    public override void Start()
    {

        playerTransform = GameObject.Find("Player").GetComponent<Transform>();
        rb = gameObject.GetComponent<Rigidbody>();
        pc = GameObject.Find("Player").GetComponent<PlayerController>();
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

//         sound = gameObject.GetComponent<AudioSource>();
//         if (sound == null)
//         {
//             Engineson.print("PlayerShooting: Audio component not found");
//         }

        enemyTransform = gameObject.GetComponent<Transform>();
        if (enemyTransform == null)
        {
            Engineson.print("ERROR: PlayerMovement requires a Transform component!");
            return;
        }

        particles = gameObject.AddComponent<ParticleFX>();
        maxHealth = health;
        currentHealth = maxHealth;
        gameObject.tag = "Ranged";

//         combatSound = new AudioClip(combatMusic, "CombatMusic", true, false);
//         sound.LoadAudioClip(combatSound);
    }

    public override void Update(float deltaTime)
    {
        if (!isDead)
        {
            if (currentHealth <= 0)
            {
                Engineson.print("This man is dead man.");
                //Destroy(gameObject);
                isDead = true;
            }
            if (!isStunned)
            {
                Vector3 playerPos = playerTransform.position;

                if (Vector3.Distance(enemyTransform.position, playerPos) < distToChase)
                {
                    if (shootTimer <= 0)
                    {
                        Attack();
                        shootTimer = shootCooldown;
                    }

                    if (isCombatMusicPlaying == false)
                    {
                        //sound?.Play(combatSound);
                        isCombatMusicPlaying = true;
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
        if (isDead)
        {
            // Enemy Death
            collider.SetActive(false);
            if (pc.playerData.isPiercing == true)
            {
                pc.playerData.AddHealth(5.0f);
            }
        }
    }

    public override void ResetEnemyCheckPoint()
    {
        if (!isDead)
        {
            currentHealth = maxHealth;
            isStunned = false;
            rb.SetVelocity(Vector3.Zero);
            collider.SetActive(true);
            gameObject.GetComponent<Collider>().SetPosition(startPosition);
            //sound?.Stop();
            isCombatMusicPlaying = false;
        }
        
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
                    Vector3 spawnPos = enemyTransform.position + new Vector3(0,2.0f,0) + forward * 1.0f;
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
        currentHealth -= damage;
        particles.ApplyPreset(19);
        particles.EmitBurst(1);
        Engineson.print("Hit");
        //anim.SetHitAnimation();
        //sound.LoadAudio("Assets/Audio/SFX/Enemies/Hormagaunt/HormagauntHit_ready.wav");
        //sound?.Play();
    }
    override public void OnCollisionEnter(GameObject other)
    {
        if (other.tag == "BoltgunProjectile")
        {
            currentHealth -= 20.0f;
            Engineson.print("Boltgun hit!");
        }
        else if (other.tag == "ShotgunProjectile")
        {
            //cosas de la shotgun
        }
        else if (other.tag == "RailgunProjectile")
        {
            //Cosas de railgun
        }
        //Engineson.print("Player hit!");
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
