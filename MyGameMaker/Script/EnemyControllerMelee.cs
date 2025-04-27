using HawkEngine;
using System;
using System.Net.Http.Headers;
using System.Numerics;

public class EnemyControllerMelee : EnemyController
{
    // Hurtbox
    private float hurtboxActivationTime = 1.5f; // Tiempo que el jugador debe estar en la hurtbox para activarla
    private float hurtboxTimer = 0f;
    private Vector3 hurtboxSize = new Vector3(3.0f, 2.0f, 3.0f); // Tama�o de la hurtbox
    private Vector3 hurtboxOffset = new Vector3(4.0f, 0.0f, 0.0f); // Desplazamiento de la hurtbox hacia adelante
    private GameObject hurtboxObject;

    // Perfect Dodge
    private bool dodgewindow = false;
    private float dodgeActivationTime = 0.5f;
    private float dodgeTimer = 0f;
    private HormagauntAnimation anim;
    private PlayerController pc;

    // Audio
    bool isCombatMusicPlaying = false;
    private const string MUSIC_COMBAT = "Assets/Audio/PlaceHolder_CombatMusic.wav";
    private const string SFX_DEATH = "Assets/Audio/SFX/Enemies/Hormagaunt/HormagauntDeath_ready.wav";
    private const string SFX_FOOTSTEP = "Assets/Audio/SFX/Enemies/Hormagaunt/HormagauntFootstep_ready.wav";
    private const string SFX_ATTACK = "Assets/Audio/SFX/Enemies/Hormagaunt/HormagauntMeleeAttack_ready.wav";
    private const string SFX_HIT = "Assets/Audio/SFX/Enemies/Hormagaunt/HormagauntHit_ready.wav";

    // Enemy Stats
    private float health = 100.0f;
    private float clawDamage = 10.0f;
    private float leapDamage = 15.0f;
    private float distanceToPlayer;

    // Leap Attack
    public float maxLeapRange = 20.0f;
    public float minLeapRange = 10.0f;
    private float leapTimer = 0f;
    private float leapDuration = 1.5f;
    private float anticipationTimer = 0f;
    private float anticipationDuration = 1f;
    private float lastLeap = 0f;
    public float leapCooldown = 2.0f;
    private bool hasLeap = true;
    private bool isLeaping = false;

    public override void Awake() 
    {
        //music = gameObject.GetComponent<Audio>();
    }

    public override void Start()
    {
        playerTransform = GameObject.Find("Player").GetComponent<Transform>();
        rb = gameObject.GetComponent<Rigidbody>();
        if (playerTransform == null)
        {
            Engineson.print("ERROR: Player couldn't be found!");
        }
        pc = GameObject.Find("Player").GetComponent<PlayerController>();
        if (pc == null)
        {
            Engineson.print("ERROR: PlayerController component not found on Player!");
            return;
        }
        collider = gameObject.GetComponent<BoxCollider>();
        if (collider == null)
        {
            Engineson.print("ERROR: Hormagaunt Movement requires a Collider component!");
            return;
        }

//         sound = gameObject.GetComponent<AudioSource>();
//         if (sound == null)
//         {
//             Engineson.print("ERROR: Audio component not found");
//         }

        enemyTransform = gameObject.GetComponent<Transform>();
        if (enemyTransform == null)
        {
            Engineson.print("ERROR: Hormagaunt Movement requires a Transform component!");
            return;
        }

        anim = GameObject.Find("HormagauntMesh").GetComponent<HormagauntAnimation>();
        if (anim == null)
        {
            Engineson.print("ERROR: HormagauntAnimation requires a SkeletalAnimation component!");
            return;
        }

        particles = gameObject.AddComponent<ParticleFX>();
        particles.ApplyPreset(9);
        Audio.MasterVolume = 0.8f;
        Audio.MusicVolume = 0.6f;
        Audio.SfxVolume = 1.0f;

        maxHealth = health;
        currentHealth = maxHealth;
        gameObject.tag = "Melee";
        isDead = false;
    }

    public override void Update(float deltaTime)
    {
        if (currentState != EnemyState.DEAD)
        {
            if (currentHealth <= 0)
            {
                currentState = EnemyState.DEAD;
                anim.SetDeathAnimation();
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

                    // Chase
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
                        anim.SetStandardIdleAnimation();
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

                if (!isLeaping)
                {
                    anim.SetRunningAnimation();
                    if (desiredVelocity.LengthSquared() > 0)
                    {
                        desiredVelocity = Vector3.Normalize(desiredVelocity) * speedMovement;
                    }

                    Vector3 newVelocity = Vector3.Lerp(currentVelocity, desiredVelocity, acceleration * deltaTime);
                    rb.SetVelocity(new Vector3(newVelocity.X, currentVelocity.Y, newVelocity.Z));
                }

                // Leap
                if (distanceToPlayer <= maxLeapRange && distanceToPlayer >= minLeapRange && currentState != EnemyState.ATTACK && hasLeap && !isLeaping)
                {
                    leapTimer = 0.0f;
                    Leap(deltaTime);
                }
                else if (!hasLeap)
                {
                    lastLeap += deltaTime;
                    if (lastLeap >= leapCooldown)
                    {
                        Engineson.print("LEAP RESTORED");
                        hasLeap = true;
                    }
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
                    anim.SetRandomAttackAnimation();
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
    }

    public override void Attack()
    {
        Engineson.print("Melee attack executed!");
        pc.playerData.TakeDamage(clawDamage);
        Engineson.print("Player health: " + (pc.playerData.GetHealth()));

        //sound?.Play(meleeAttackSound);
    }

    public void Leap()
    {
        if (!isLeaping)
        {
            if(pc.redThirstManager.redThirstBonus < clawDamage)
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
            anim.SetHitAnimation();
            particles.ApplyPreset(19);
            particles.EmitBurst(1);
            Audio.PlayOneShot(SFX_HIT);
        }
    }

    public void Leap(float deltaTime)
    {
        anticipationTimer += deltaTime;
        if (anticipationTimer < anticipationDuration)
        {
            //anim.SetAnticipationAnimation();
            rb.SetVelocity(Vector3.Zero);
        }
        else if (anticipationTimer >= anticipationDuration)
        {
            if (distanceToPlayer > distToChase)
            {
                return;
            }
            else
            {
                hasLeap = false;
                anim.SetWholeLeapAnimation();
                particles.EmitBurst(1);

                leapTimer += deltaTime;
                if (leapTimer < leapDuration)
                {
                    isLeaping = true;
                    rb.SetVelocity(rb.GetVelocity() * 10f);
                }
                else if (leapTimer >= leapDuration)
                {
                    leapTimer = 0f;
                    isLeaping = false;
                    lastLeap = 0.0f;
                }
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
        if (other.tag == "Player" && isLeaping)
        {
            Engineson.print(other.tag + "hit with Leap");
            if (pc.redThirstManager.IsInBlackRage())
            {
                if (pc.redThirstManager.redThirstBonus < leapDamage)
                {
                    pc.playerData.TakeDamage(leapDamage - pc.redThirstManager.redThirstBonus);
                }
                else
                {
                    pc.playerData.TakeDamage(0);
                }
            }
            else
            {
                pc.playerData.TakeDamage(leapDamage);
            }
            Engineson.print(other.tag + " health: " + (pc.playerData.GetHealth()));
        }
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
