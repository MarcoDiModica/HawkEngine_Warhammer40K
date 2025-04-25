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
    PlayerController pc;

    // Audio
    bool isCombatMusicPlaying = false;
    private Audio music;
    private string combatMusic = "Assets/Audio/PlaceHolder_CombatMusic.wav";

    // Enemy Stats
    private float health = 100.0f;
    private float clawDamage = 10.0f;
    private float leapDamage = 15.0f;

    // Leap Attack
    public float maxLeapRange = 20.0f;
    public float minLeapRange = 10.0f;
    private float lastLeap = 0f;
    public float leapCooldown = 2.0f;
    private float leapDuration = 1.5f;
    private float leapTimer = 0f;
    private bool hasLeap = true;

    private bool isLeaping = false;

    // Pathfinding
    private float chaseReplanInterval = 0.5f;
    private float chaseTimer = 0f;

    public override void Awake() {

        music = gameObject.GetComponent<Audio>();
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
            Engineson.print("ERROR: Hormagaunt Movement requires a Collider component!");
            return;
        }

        sound = gameObject.GetComponent<Audio>();
        if (sound == null)
        {
            Engineson.print("ERROR: Audio component not found");
        }

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

        pc = GameObject.Find("Player").GetComponent<PlayerController>();
        maxHealth = health;
        currentHealth = maxHealth;
        gameObject.tag = "Melee";

        pathfinder = new Pathfinding();
        chasePath = null;
        pathInitialized = false;
    }

    public override void Update(float deltaTime)
    {
        if (currentState != EnemyState.DEAD)
        {
            if (currentHealth <= 0)
            {
                currentState = EnemyState.DEAD;
                anim.SetDeathAnimation();
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

                    // Chase
                    if (distanceToPlayer > minDistToChase)
                    {
                        currentState = EnemyState.CHASE;
                    }

                    // Leap
                    if (distanceToPlayer <= maxLeapRange && distanceToPlayer >= minLeapRange && hasLeap && !isLeaping)
                    {
                        leapTimer = 0f;
                        Leap();
                    }
                    else if (isLeaping)
                    {
                        leapTimer += deltaTime;
                        particles.EmitBurst(1);
                        if (leapTimer >= leapDuration)
                        {
                            isLeaping = false;
                            hasLeap = false;
                            lastLeap = 0.0f;
                        }
                    }
                    if (!hasLeap)
                    {
                        lastLeap += deltaTime;
                        if (lastLeap >= leapCooldown)
                        {
                            Engineson.print("LEAP RESTORED");
                            hasLeap = true;
                        }
                    }

                    // Enemy Rotation
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
                        anim.SetStandardIdleAnimation();
                    }
                }
            }
        }

        //Engineson.print(gameObject.name + " STATE: " + currentState.ToString());

        switch (currentState)
        {
            case EnemyState.IDLE:
                isRunning = false;
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

                //Vector3 currentVelocity = rb.GetVelocity();
                //moveDirection = Vector3.Normalize(playerTransform.position - gameObject.GetComponent<Transform>().position);
                //Vector3 desiredVelocity = moveDirection * speedMovement;

                //if (!isLeaping)
                //{
                //    anim.SetRunningAnimation();
                //    if (desiredVelocity.LengthSquared() > 0)
                //    {
                //        desiredVelocity = Vector3.Normalize(desiredVelocity) * speedMovement;
                //    }

                //    Vector3 newVelocity = Vector3.Lerp(currentVelocity, desiredVelocity, acceleration * deltaTime);
                //    rb.SetVelocity(new Vector3(newVelocity.X, currentVelocity.Y, newVelocity.Z));
                //}
                //isRunning = true;


                Vector3 myPos = enemyTransform.position;
                Vector3 tgtPos = playerTransform.position;

                chaseTimer += deltaTime;
                if (chaseTimer >= chaseReplanInterval)
                {
                    var newPath = pathfinder.FindPath(enemyTransform.position, playerTransform.position);
                    if (newPath != null && newPath.Count > 1)
                    {
                        (int curX, int curY) = Pathfinding.ToGrid(enemyTransform.position);
                        int found = newPath.FindIndex(v =>
                        {
                            var (gx, gy) = Pathfinding.ToGrid(v);
                            return gx == curX && gy == curY;
                        });
                        chasePath = newPath;
                        chaseIndex = (found >= 0) ? found + 1 : 1;
                    }
                    chaseTimer = 0f;
                    Engineson.print("" + chasePath.Count);
                }


                if (chasePath != null && chaseIndex < chasePath.Count)
                {
                    Vector3 wp = chasePath[chaseIndex];
                    Vector3 delta = wp - myPos;
                    float d = delta.Length();

                    if (d < Pathfinding.cellSize * 0.5f)
                    {
                        chaseIndex++;
                    }
                    else
                    {
                        Vector3 dir = (d > 1e-5f) ? delta / d : Vector3.Zero;

                        moveDirection = dir;

                        Vector3 desired = dir * speedMovement;
                        Vector3 cv = rb.GetVelocity();
                        float t = Math.Min(1f, acceleration * deltaTime);

                        rb.SetVelocity(Vector3.Lerp(cv, desired, t));
                        anim.SetRunningAnimation();
                        isRunning = true;
                    }
                }
                //else
                //{
                //    Vector3 delta = playerTransform.position - enemyTransform.position;
                //    float len = delta.Length();
                //    Vector3 dir = (len > 1e-5f) ? delta / len : Vector3.Zero;

                //    moveDirection = dir;

                //    Vector3 desired = dir * speedMovement;
                //    Vector3 cv = rb.GetVelocity();
                //    float t = Math.Min(1f, acceleration * deltaTime);

                //    rb.SetVelocity(Vector3.Lerp(cv, desired, t));
                //    anim.SetRunningAnimation();
                //    isRunning = true;
                //}

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
        Engineson.print("Melee attack executed!");
        if (pc.redThirstManager.IsInBlackRage())
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

        Engineson.print("Player health: " + (pc.playerData.GetHealth()));

        sound.LoadAudio("Assets/Audio/SFX/Enemies/Hormagaunt/HormagauntMeleeAttack_ready.wav");
        sound?.Play();
    }

    public override void TakeDamage(float damage)
    {
        if (currentHealth > 0)
        {
            currentHealth -= damage;
            anim.SetHitAnimation();
            particles.ApplyPreset(19);
            particles.EmitBurst(1);
            sound.LoadAudio("Assets/Audio/SFX/Enemies/Hormagaunt/HormagauntHit_ready.wav");
            sound?.Play();
        }
    }

    public void Leap()
    {
        if (!isLeaping)
        {
            isLeaping = true;
            anim.SetWholeLeapAnimation();
            rb.SetVelocity(rb.GetVelocity() * 1.8f);
            leapTimer = 0.0f;
        }

        if(leapTimer >= leapDuration)
        {
            Engineson.print("Leap ended");
            isLeaping = false;
            hasLeap = false;
            lastLeap = 0.0f;
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
        if (other.tag == "BoltgunProjectile")
        {
            currentHealth -= 20.0f;
            Engineson.print("Hit");
            anim.SetHitAnimation();
            sound.LoadAudio("Assets/Audio/SFX/Enemies/Hormagaunt/HormagauntHit_ready.wav");
            sound?.Play();

            Engineson.print("Boltgun hit!");
        }
        else if (other.tag == "ShotgunProjectile")
        {
            //cosas de la shotgun
            anim.SetHitAnimation();
            sound.LoadAudio("Assets/Audio/SFX/Enemies/Hormagaunt/HormagauntHit_ready.wav");
            sound?.Play();

        }
        else if (other.tag == "RailgunProjectile")
        {
            //Cosas de railgun
            currentHealth -= 100.0f;
            anim.SetHitAnimation();
            sound.LoadAudio("Assets/Audio/SFX/Enemies/Hormagaunt/HormagauntHit_ready.wav");
            sound?.Play();
        }
        if (other.tag == "Player" && isLeaping)
        {
            Engineson.print("Player hit while Leaping!");
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
            Engineson.print("Player health: " + (pc.playerData.GetHealth()));
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
