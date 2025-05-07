using System;
using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public class EnemyControllerStalker : EnemyController
{
    // Enemy Stats
    private float health = 350.0f;
    private float clawDamage = 25.0f;
    private float pounceDamage = 35.0f;
    private float distanceToPlayer;
    private bool hasDropped = false;

    // Hurtbox
    private float hurtboxActivationTime = 1.5f; // Tiempo que el jugador debe estar en la hurtbox para activarla
    private float hurtboxTimer = 0f;
    private Vector3 hurtboxSize = new Vector3(10.0f, 10.0f, 10.0f); // Tamaño de la hurtbox
    private Vector3 hurtboxOffset = new Vector3(5.0f, -3.0f, 0.0f); // Desplazamiento de la hurtbox hacia adelante
    private GameObject hurtboxObject;

    // Perfect Dodge
    private bool dodgewindow = false;
    private float dodgeActivationTime = 0.5f;
    private float dodgeTimer = 0f;
    private LictorAnimation anim;
    PlayerController pc;

    // Audio
    bool isCombatMusicPlaying = false;
    private const string MUSIC_COMBAT = "Assets/Audio/PlaceHolder_CombatMusic.wav";
    private const string SFX_DEATH = "Assets/Audio/Lictor/Death_2.wav";
    private const string SFX_FOOTSTEP = "Assets/Audio/SFX/Enemies/Hormagaunt/HormagauntFootstep_ready.wav";
    private const string SFX_ATTACK = "Assets/Audio/Lictor/Meele_Atk_SFX.wav";
    private const string SFX_HIT = "Assets/Audio/Lictor/Hit_3.wav";
    private const string SFX_POUNCE = "Assets/Audio/Lictor/Jump_FULL.wav";
    private bool hasPlayedDeathSound = false;

    // Invisibility
    private float invisibilityRange = 50.0f;
    private GameObject lictorMesh;

    // Pounce
    private float pounceRange = 30.0f;
    private float pounceTimer = 0f;
    private float pounceDuration = 1.5f;
    private float anticipationTimer = 0f;
    private float anticipationDuration = 2f;
    private bool hasPounce = true;
    private bool isPouncing = false;
    private bool hasMissed = true;

    // Death
    private float deathTimer = 0f;
    private float deathDuration = 3f;
    public override void Awake()
    {

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

        enemyTransform = gameObject.transform;
        if (enemyTransform == null)
        {
            Engineson.print("ERROR: Lictor transform not found!");
            return;
        }

        anim = gameObject.GetChild("LictorMesh").GetComponent<LictorAnimation>();
        if (anim == null)
        {
            Engineson.print("ERROR: LictorAnimation requires SkeletalANimation component");
            return;
        }

        //particles = gameObject.GetComponent<ParticleFX>();
        //particles.ApplyPreset(9);

        maxHealth = health;
        currentHealth = maxHealth;
        gameObject.tag = "Stalker";

        distToChase = 75f;
    }

    public override void Update(float deltaTime)
    {
        if (currentState != EnemyState.DEAD)
        {
            if (currentHealth <= 0)
            {
                currentState = EnemyState.DEAD;
                anim.SetDefeatAnimation();
                Audio.PlayOneShot(SFX_DEATH);
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
                    }

                    // Chasing
                    if (distanceToPlayer > minDistToChase)
                    {
                        currentState = EnemyState.CHASE;
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
                    Audio.Stop(SFX_FOOTSTEP);
                    hasStoppedFootsteps = true;
                }
                break;

            case EnemyState.CHASE:
                if (!isFootstepPlaying)
                {
                    Audio.Play(SFX_FOOTSTEP, true);
                    isFootstepPlaying = true;
                    hasStoppedFootsteps = false;
                }
                if (isCombatMusicPlaying == false)
                {
                    Audio.Play(MUSIC_COMBAT, true);
                    isCombatMusicPlaying = true;
                }

                Vector3 currentVelocity = rb.GetVelocity();
                moveDirection = Vector3.Normalize(playerTransform.position - gameObject.GetComponent<Transform>().position);
                Vector3 desiredVelocity = moveDirection * speedMovement;

                anim.SetWalkToPlayerAnimation();
                if (desiredVelocity.LengthSquared() > 0)
                {
                    desiredVelocity = Vector3.Normalize(desiredVelocity) * speedMovement;
                }

                Vector3 newVelocity = Vector3.Lerp(currentVelocity, desiredVelocity, acceleration * deltaTime);
                rb.SetVelocity(new Vector3(newVelocity.X, currentVelocity.Y, newVelocity.Z));

                // Invisibility
                if (distanceToPlayer < invisibilityRange && currentState != EnemyState.ATTACK && !isPouncing)
                {
                    Invisibility();
                }

                // Pounce
                if (distanceToPlayer < pounceRange && hasPounce && !isPouncing)
                {
                    lictorMesh.SetActive(true);
                    Pounce(deltaTime);
                }
                else
                {
                    anticipationTimer = 0;
                }
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
                    anim.SetPiercingAnimation();
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
                rb.SetVelocity(Vector3.Zero);

                stunTimer += deltaTime;
                if (stunTimer >= stunDuration)
                {
                    isStunned = false;
                    stunTimer = 0.0f;
                }
                break;

            case EnemyState.DEAD:
                if (!hasPlayedDeathSound)
                {
                    Audio.PlayOneShot(SFX_DEATH);
                    hasPlayedDeathSound = true;
                }
                if ((!hasDropped))
                {
                    GameObject.Find("DropManager").GetComponent<DropManager>().SpawnPrefab(this);
                }
                if (anim.isAnimFinished)
                {
                    Engineson.Destroy(lictorMesh);
                    Engineson.Destroy(gameObject);
                }
                hasDropped = true;
                collider.SetActive(false);
                break;

            default:
                break;
        }
    }

    public override void Attack()
    {
        //Engineson.print("Melee attack executed!");
        if (pc.redThirstManager.IsInBlackRage())
        {
            if (pc.redThirstManager.redThirstBonus < clawDamage)
            {
                pc.playerData.TakeDamage(clawDamage - pc.redThirstManager.redThirstBonus);
            }
            else
            {
                pc.playerData.TakeDamage(0);
            }
        }
        else
        {
            pc.playerData.TakeDamage(clawDamage);
        }

        Audio.PlayOneShot(SFX_ATTACK);
    }

    public override void TakeDamage(float damage)
    {
        if (currentHealth > 0)
        {
            currentHealth -= damage;
            anim.SetStunnedAnimation();
            //particles.ApplyPreset(19);
            //particles.EmitBurst(1);
            Audio.PlayOneShot(SFX_HIT);
        }
    }

    public void Invisibility()
    {
        lictorMesh.SetActive(false);
    }

    public void Pounce(float deltaTime)
    {
        anticipationTimer += deltaTime;
        if (anticipationTimer < anticipationDuration)
        {
            anim.SetCrossSlashAnimation();
            rb.SetVelocity(Vector3.Zero);
        }
        else if (anticipationTimer >= anticipationDuration)
        {
            hasPounce = false;
            isPouncing = true;
            Audio.PlayOneShot(SFX_POUNCE);
            Engineson.print("Pouncing");
            anim.SetLeapAnimation();
            rb.SetVelocity(rb.GetVelocity() * 120f);
        }
        else
        {
            Engineson.print("Not Pouncung anymore");
            pounceTimer = 0f;
            isPouncing = false;

            if (hasMissed)
            {
                Engineson.print("Missed");
                currentState = EnemyState.STUNNED;
            }
            else
            {
                Engineson.print("Not Missed");
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

    override public void OnCollisionEnter(GameObject other)
    {
        if (other.tag == "Player" && isPouncing)
        {
            hasMissed = false;

            Engineson.print(other.tag + " hit with Pounce");
            if (pc.redThirstManager.IsInBlackRage())
            {
                if (pc.redThirstManager.redThirstBonus < pounceDamage)
                {
                    pc.playerData.TakeDamage(pounceDamage - pc.redThirstManager.redThirstBonus);
                }
                else
                {
                    pc.playerData.TakeDamage(0);
                }
            }
            else
            {
                pc.playerData.TakeDamage(pounceDamage);
            }
        }
    }

    //For testing
    private void CreateHurtbox()
    {
        hurtboxObject = Engineson.CreateGameObject("Hurtbox", null);
        //hurtboxObject.AddComponent<MeshRenderer>();
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

