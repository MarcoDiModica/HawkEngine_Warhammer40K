using System;
using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public class EnemyControllerStalker : EnemyController
{
    // Hurtbox
    private float hurtboxActivationTime = 1.5f; // Tiempo que el jugador debe estar en la hurtbox para activarla
    private float hurtboxTimer = 0f;
    private Vector3 hurtboxSize = new Vector3(3.0f, 2.0f, 3.0f); // Tamaño de la hurtbox
    private Vector3 hurtboxOffset = new Vector3(4.0f, 0.0f, 0.0f); // Desplazamiento de la hurtbox hacia adelante
    private GameObject hurtboxObject;

    // Perfect Dodge
    private bool dodgewindow = false;
    private float dodgeActivationTime = 0.5f;
    private float dodgeTimer = 0f;
    private HormagauntAnimation anim;
    PlayerController pc;

    // Audio
    bool isCombatMusicPlaying = false;
    private Audio music;
    private string combatMusic = "Assets/Audio/PlaceHolder_CombatMusic.wav";

    // Enemy Stats
    private float health = 350.0f;
    private float clawDamage = 25.0f;
    private float pounceDamage = 35.0f;

        // Invisibility
    private float invisibilityRange = 35.0f;
    private GameObject lictorMesh;
    private bool isInvisible = false;

        // Pounce+
    private float pounceRange = 15.0f;
    private float pounceTimer = 0f;
    private float pounceDuration = 1.0f;
    private bool hasPounced = false;
    private bool isPouncing = false;

    public override void Awake()
    {
        music = gameObject.GetComponent<Audio>();
    }

    public override void Start()
    {
        playerTransform = GameObject.Find("Player").GetComponent<Transform>();
        rb = gameObject.GetComponent<Rigidbody>();
        if (playerTransform == null)
        {
            Engineson.print("ERROR: Player not found!");
        }

        lictorMesh = GameObject.Find("LictorMesh");
        if (lictorMesh == null)
        {
            Engineson.print("ERROR: LictorMesh not found!");
            return;
        }

        collider = gameObject.GetComponent<BoxCollider>();
        if (collider == null)
        { 
            Engineson.print("ERROR: Lictor Collider not found!"); 
            return; 
        }

        sound = gameObject.GetComponent<Audio>();
        if (sound == null)
        {
            Engineson.print("ERROR: Lictor Sound not found!");
        }

        enemyTransform = gameObject.transform;
        if (enemyTransform == null)
        {
            Engineson.print("ERROR: Lictor transform not found!");
            return;
        }

        //anim = GameObject.Find("LictorMesh").GetComponent<LictorAnimation>();
        //if (anim == null)
        //{
        //    Engineson.print("ERROR: LictorAnimation requires SkeletalANimation component");
        //    return;
        //}

        //particles = gameObject.GetComponent<ParticleFX>();
        //particles.ApplyPreset(9);

        pc = GameObject.Find("Player").GetComponent<PlayerController>();
        maxHealth = health;
        currentHealth = maxHealth;
        gameObject.tag = "Stalker";
    }

    public override void Update(float deltaTime)
    {
        if (currentState != EnemyState.DEAD)
        {
            if (currentHealth <= 0)
            {
                currentState = EnemyState.DEAD;
                sound.LoadAudio("Assets/Audio/SFX/Enemies/Hormagaunt/HormagauntDeath_ready.wav");
                sound?.Play();
                return;
            }

            if (currentState != EnemyState.STUNNED)
            {
                float distanceToPlayer = Vector3.Distance(enemyTransform.position, playerTransform.position);

                if (distanceToPlayer < distToChase)
                {
                    // Attack
                    if (IsPlayerInHurtbox(playerTransform.position))
                    {
                        currentState = EnemyState.ATTACK;
                    }

                    // Chasing
                    if (distanceToPlayer > minDistToChase)
                    {
                        currentState = EnemyState.CHASE;
                    }

                    // Invisibility
                    if (distanceToPlayer < invisibilityRange && currentState != EnemyState.ATTACK)
                    {
                        Invisibility();

                        if (distanceToPlayer < pounceRange && !hasPounced && !isPouncing)
                        {
                            Pounce(deltaTime);
                        }
                    }
                    else
                    {
                        lictorMesh.SetActive(true);
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

                break;

            case EnemyState.STUNNED:
                stunTimer += deltaTime;
                rb.SetVelocity(Vector3.Zero);
                if (stunTimer >= stunDuration)
                {
                    isStunned = false;
                    stunTimer = 0.0f;
                }
                break;

            case EnemyState.DEAD:
                collider.SetActive(false);

                break;

            default:
                break;
        }
    }

    public override void Attack()
    {
        pc.playerData.TakeDamage(clawDamage);
        sound.LoadAudio("Assets/Audio/SFX/Enemies/Hormagaunt/HormagauntMeleeAttack_ready.wav");
        sound?.Play();
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

    public void Invisibility()
    {
        lictorMesh.SetActive(false);
    }

    public void Pounce(float deltaTime)
    {
        if (!hasPounced)
        {
            isPouncing = true;
            hasPounced = true;
            pounceTimer += deltaTime;
            rb.SetVelocity(rb.GetVelocity() * 2.5f);

            if (pounceTimer >= pounceDuration)
            {
                isPouncing = false;
                pounceTimer = 0f;
                hasPounced = false;
            }
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

    //For testing
    private void CreateHurtbox()
    {
        hurtboxObject = Engineson.CreateGameObject("Hurtbox", null);
        hurtboxObject.AddComponent<MeshRenderer>();
        var hurtboxTransform = hurtboxObject.AddComponent<Transform>();
        hurtboxTransform.position = enemyTransform.position + (enemyTransform.forward * hurtboxOffset.X) + (Vector3.UnitY * hurtboxOffset.Y);
        hurtboxTransform.SetScale(hurtboxSize.X, hurtboxSize.Y, hurtboxSize.Z);
        var hurtboxCollider = hurtboxObject.AddComponent<BoxCollider>();
        hurtboxCollider.SetTrigger(true);
        hurtboxObject.tag = "EnemyAttack";
        //Attack();
    }

    private void DestroyHurtbox()
    {
        if (hurtboxObject != null)
        {
            Engineson.Destroy(hurtboxObject);
        }
    }
}

