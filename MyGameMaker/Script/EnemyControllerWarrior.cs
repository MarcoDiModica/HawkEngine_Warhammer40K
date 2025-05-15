using System;
using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public class EnemyControllerWarrior : EnemyController
{
    // Enemy Stats
    private float health = 100.0f;
    private float projectileDamage = 15.0f;
    private float swordDamage = 25.0f;
    private float distanceToPlayer;
    private bool hasDropped = false;

    // Hurtbox
    private float hurtboxActivationTime = 1.5f; // Tiempo que el jugador debe estar en la hurtbox para activarla
    private float hurtboxTimer = 0f;
    private Vector3 hurtboxSize = new Vector3(10.0f, 10.0f, 10.0f); // Tamaño de la hurtbox
    private Vector3 hurtboxOffset = new Vector3(5.0f, -3.0f, 0.0f); // Desplazamiento de la hurtbox hacia adelante
    private GameObject hurtboxObject;

    // Shooting
    private float projectileRange = 30;
    private List<BulletData> activeProjectiles = new List<BulletData>();
    public float shootCooldown = 2.0f;
    public float projectileSpeed = 90.0f;
    public float projectileLifetime = 0.5f;
    protected float shootTimer = 0f;

    // Perfect Dodge
    private bool dodgewindow = false;
    private float dodgeActivationTime = 0.5f;
    private float dodgeTimer = 0f;
    //private TyranidWarriorAnimation animation
    private PlayerController pc;

    // Audio
    bool isCombatMusicPlaying = false;
    //private Audio music;
    private string combatMusic = "Assets/Audio/PlaceHolder_CombatMusic.wav";
    private string WalkSound = "Assets/Audio/TyranidWarrior/Tyranid_WAR_Walk.wav";
    private string MeleeAttackSound = "Assets/Audio/TyranidWarrior/Tyranid_WAR_Melee_Atack_1.wav";
    private string RangedAttackSound = "Assets/Audio/TyranidWarrior/Tyranid_WAR_Ranged_Atack_1.wav";
    private string DeathSound = "Assets/Audio/TyranidWarrior/TyranidWAR_Death.wav";
    private string GrowlSound = "Assets/Audio/TyranidWarrior/Tyranid_WAR_Grwol_1.wav";
    private string HitSound = "Assets/Audio/TyranidWarrior/Tyranid_WAR_Hit_1.wav";

    private bool hasPlayedDeathSound = false;
    private WarriorAnimation anim;
    public override void Awake()
    {
        //music = gameObject.GetComponent<Audio>();
        startPosition = gameObject.GetComponent<Transform>().position;
    }

    public override void Start()
    {
        pc = GameObject.Find("Player").GetComponent<PlayerController>();
        playerTransform = GameObject.Find("Player").GetComponent<Transform>();
        rb = gameObject.GetComponent<Rigidbody>();
        if (playerTransform == null)
        {
            Engineson.print("ERROR: Player not found!");
        }

        collider = gameObject.GetComponent<BoxCollider>();
        if (collider == null)
        {
            Engineson.print("ERROR: Tyranid Warrior Collider not found!");
            return;
        }

        //sound = gameObject.GetComponent<Audio>();
        //if (sound == null)
        //{
        //    Engineson.print("ERROR: Tyranid Warrior Sound not found!");
        //}

        enemyTransform = gameObject.transform;
        if (enemyTransform == null)
        {
            Engineson.print("ERROR: Tyranid Warrior transform not found!");
            return;
        }

        anim = gameObject.GetChild("TyranidWarriorMesh").GetComponent<WarriorAnimation>();
        if (anim == null)
        {
            Engineson.print("ERROR: TyranidWarriorAnimation requires SkeletalANimation component");
            return;
        }

        //particles = gameObject.GetComponent<ParticleFX>();
        //particles.ApplyPreset(9);

        maxHealth = health;
        currentHealth = maxHealth;
        gameObject.tag = "Warrior";
    }

    public override void Update(float deltaTime)
    {
        if (currentState != EnemyState.DEAD)
        {
            if (currentHealth <= 0)
            {
                currentState = EnemyState.DEAD;
                //anim.SetDeathAnimation();
                //sound.LoadAudio("Assets/Audio/SFX/Enemies/Hormagaunt/HormagauntDeath_ready.wav");
                //sound?.Play();
                return;
            }

            if (currentState != EnemyState.STUNNED)
            {
                distanceToPlayer = Vector3.Distance(enemyTransform.position, playerTransform.position);

                if (distanceToPlayer < distToChase)
                {
                    // Attack
                    if (IsPlayerInHurtbox(playerTransform.position))
                    {
                        currentState = EnemyState.ATTACK;
                        isAttacking = true;
                    }
                    // Shooting
                    else if (distanceToPlayer < projectileRange)
                    {
                        currentState = EnemyState.ATTACK;
                        isShooting = true;
                    }
                    // Chase
                    else
                    {
                        currentState = EnemyState.CHASE;
                    }
                }
                else
                {
                    if (currentState != EnemyState.IDLE)
                    {
                        currentState = EnemyState.IDLE;
                        rb.SetVelocity(Vector3.Zero);
                        anim.SetIdleAnimation();
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
                    
                    //sound?.Stop();
                    hasStoppedFootsteps = true;
                }
                break;

            case EnemyState.CHASE:
                if (!isFootstepPlaying)
                {
                    //sound?.LoadAudio("Assets/Audio/SFX/Enemies/Hormagaunt/HormagauntFootstep_ready.wav");
                    //sound?.Play(true);
                    Audio.PlayOneShot(GrowlSound);
                    Audio.SchedulePlay(WalkSound,0.5f);
                    isFootstepPlaying = true;
                    hasStoppedFootsteps = false;
                }
                if (isCombatMusicPlaying == false)
                {
                    //sound?.LoadAudio(combatMusic);
                    //sound?.Play(true);
                    isCombatMusicPlaying = true;
                }

                Vector3 currentVelocity = rb.GetVelocity();
                moveDirection = Vector3.Normalize(playerTransform.position - gameObject.GetComponent<Transform>().position);
                Vector3 desiredVelocity = moveDirection * speedMovement;

                //anim.SetRunningAnimation();
                if (desiredVelocity.LengthSquared() > 0)
                {
                    desiredVelocity = Vector3.Normalize(desiredVelocity) * speedMovement;
                }

                Vector3 newVelocity = Vector3.Lerp(currentVelocity, desiredVelocity, acceleration * deltaTime);
                rb.SetVelocity(new Vector3(newVelocity.X, currentVelocity.Y, newVelocity.Z));
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
                anim.SetRunAnimation();
                break;

            case EnemyState.ATTACK:
                if (isAttacking)
                {
                    hurtboxTimer += deltaTime;
                    if (dodgewindow)
                    {
                        dodgeTimer += deltaTime;
                    }
                    if (hurtboxTimer >= hurtboxActivationTime)
                    {
                        //CreateHurtbox();
                        //anim.SetRandomAttackAnimation();
                        hurtboxTimer = 0f;
                        dodgeTimer = 0f;
                        dodgewindow = true;
                    }
                    else if (dodgeTimer >= dodgeActivationTime && dodgewindow)
                    {
                        Attack();

                        //DestroyHurtbox();
                        hurtboxTimer = 0f;
                        dodgeTimer = 0f;
                        dodgewindow = false;
                        isAttacking = false;
                    }
                }
                else if (isShooting)
                {
                    shootTimer += deltaTime;
                    if (shootTimer >= shootCooldown)
                    {
                        Attack();
                        //sound?.Play();
                        shootTimer = 0;
                    }
                }
                currentRotationAngle = GetComponent<Transform>().eulerAngles.Y;
                float targetAttackAngle = (float)Math.Atan2(moveDirection.X, moveDirection.Z);
                float targetAttackAngleDegrees = targetAttackAngle * (180.0f / (float)Math.PI);

                while (targetAttackAngleDegrees - currentRotationAngle > 180.0f) targetAttackAngleDegrees -= 360.0f;
                while (targetAttackAngleDegrees - currentRotationAngle < -180.0f) targetAttackAngleDegrees += 360.0f;

                currentRotationAngle = Lerp(currentRotationAngle, targetAttackAngleDegrees, rotationSpeed * deltaTime);

                Vector3 eulerAttackRotation = new Vector3(0, currentRotationAngle, 0);
                Quaternion newAttackRotation = Quaternion.CreateFromYawPitchRoll(
                    eulerAttackRotation.Y * ((float)Math.PI / 180.0f),
                    eulerAttackRotation.X * ((float)Math.PI / 180.0f),
                    eulerAttackRotation.Z * ((float)Math.PI / 180.0f)
                );

                collider.SetRotation(newAttackRotation);
                break;
            case EnemyState.STUNNED:
                break;
            case EnemyState.DEAD:
                if (!hasPlayedDeathSound) 
                {
                    Audio.PlayOneShot(DeathSound);
                    hasPlayedDeathSound = true; 
                }
                if ((!hasDropped))
                {
                    GameObject.Find("DropManager").GetComponent<DropManager>().SpawnPrefab(this);
                }
                anim.SetDeathAnimation();
                hasDropped = true;
                collider.SetActive(false);
                break;
            default:
                break;
        }

        UpdateProjectiles(deltaTime);
        CleanupProjectiles();
    }

    public override void ResetEnemyCheckPoint()
    {
        if (!isDead)
        {
            currentHealth = maxHealth;
            gameObject.GetComponent<Collider>().SetPosition(startPosition);
            rb.SetVelocity(Vector3.Zero);
            currentState = EnemyState.IDLE;
            isAttacking = false;
            isShooting = false;
            hasPlayedDeathSound = false;
            hasDropped = false;
        }
        
        //anim.SetStandardIdleAnimation();
    }
    public override void Attack()
    {
        if (isAttacking)
        {
            Audio.PlayOneShot(MeleeAttackSound);
            //Engineson.print("Melee attack executed!");
            if (pc.redThirstManager.IsInBlackRage())
            {
                if (pc.redThirstManager.redThirstBonus < swordDamage)
                {
                    pc.playerData.TakeDamage(swordDamage - pc.redThirstManager.redThirstBonus);
                    pc.StartFlashColor(pc.flashColor, pc.flashDuration);
                }
                else
                {
                    pc.playerData.TakeDamage(0);
                }
            }
            else
            {
                pc.playerData.TakeDamage(swordDamage);
                pc.StartFlashColor(pc.flashColor, pc.flashDuration);
            }
            anim.SetMeleeAnimation();
            //sound.LoadAudio("Assets/Audio/SFX/Enemies/Hormagaunt/HormagauntMeleeAttack_ready.wav");
            //sound?.Play();
        }
        else if (isShooting)
        {
            try
            {
                Audio.PlayOneShot(RangedAttackSound);
                GameObject projectile = Engineson.CreateGameObject("Projectile", null);
                Engineson.print("Projectile created!" + enemyTransform.forward);
                // TODO: add custom mesh to the projectile
                projectile.AddComponent<MeshRenderer>();
                projectile.AddComponent<BoxCollider>();
                projectile.AddComponent<ParticleFX>();
                projectile.GetComponent<ParticleFX>().ApplyPreset(10);
                projectile.GetComponent<ParticleFX>().EmitBurst(1);
                //sound?.Play();
                projectile.tag = "EnemyAttack";
                anim.SetShootAnimation();
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
    }

    public override void TakeDamage(float damage)
    {
        if (currentHealth > 0)
        {
            Audio.PlayOneShot(HitSound);
            currentHealth -= damage;
            anim.SetHurtAnimation();
        }
    }

    private bool IsPlayerInHurtbox(Vector3 playerPos)
    {
        Vector3 hurtboxCenter = enemyTransform.position + (enemyTransform.forward * hurtboxOffset.X) + (Vector3.UnitY * hurtboxOffset.Y);
        Vector3 halfSize = hurtboxSize * 0.5f;

        return (playerPos.X >= hurtboxCenter.X - halfSize.X && playerPos.X <= hurtboxCenter.X + halfSize.X) &&
               (playerPos.Y >= hurtboxCenter.Y - halfSize.Y && playerPos.Y <= hurtboxCenter.Y + halfSize.Y) &&
               (playerPos.Z >= hurtboxCenter.Z - halfSize.Z && playerPos.Z <= hurtboxCenter.Z + halfSize.Z);
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