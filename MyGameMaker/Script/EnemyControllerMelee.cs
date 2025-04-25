using HawkEngine;
using System;
using System.Collections.Generic;
using System.Net.Http.Headers;
using System.Numerics;

public class EnemyControllerMelee : EnemyController
{
    private float hurtboxActivationTime = 1.5f;
    private float hurtboxTimer = 0f;
    private Vector3 hurtboxSize = new Vector3(3.0f, 2.0f, 3.0f);
    private Vector3 hurtboxOffset = new Vector3(4.0f, 0.0f, 0.0f);
    private GameObject hurtboxObject;

    private bool dodgewindow = false;
    private float dodgeActivationTime = 0.5f;
    private float dodgeTimer = 0f;
    private HormagauntAnimation anim;
    PlayerController pc;

    bool isCombatMusicPlaying = false;
    private Audio music;
    private string combatMusic = "Assets/Audio/PlaceHolder_CombatMusic.wav";

    private float health = 100.0f;
    private float clawDamage = 10.0f;
    private float leapDamage = 15.0f;

    public float maxLeapRange = 20.0f;
    public float minLeapRange = 10.0f;
    private float lastLeap = 0f;
    public float leapCooldown = 2.0f;
    private float leapDuration = 1.5f;
    private float leapTimer = 0f;
    private bool hasLeap = true;
    private Vector3 leapDirection;

    private bool isLeaping = false;

    private float chaseReplanInterval = 0.5f;
    private float chaseTimer = 0f;

    private float pathUpdateDistance = 5.0f;
    private Vector3 lastPathfindingPosition;
    private int pathPointIndex = 0;
    private float pathSmoothingFactor = 0.2f;
    private List<Vector3> smoothedVelocity = new List<Vector3>();

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
        pathfinder.UpdateGridOrigin(enemyTransform.position);
        lastPathfindingPosition = enemyTransform.position;
        chasePath = null;
        pathInitialized = false;
    }

    public override void Update(float deltaTime)
    {
        if (!pathInitialized && pathfinder != null && enemyTransform != null && playerTransform != null)
        {
            pathfinder.UpdateGridOrigin(enemyTransform.position);
            pathInitialized = true;
        }

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

                if (Vector3.Distance(enemyTransform.position, lastPathfindingPosition) > pathUpdateDistance)
                {
                    pathfinder.UpdateGridOrigin(enemyTransform.position);
                    lastPathfindingPosition = enemyTransform.position;
                }

                if (distanceToPlayer < distToChase)
                {
                    if (IsPlayerInHurtbox(playerTransform.position))
                    {
                        currentState = EnemyState.ATTACK;
                    }

                    if (distanceToPlayer > minDistToChase)
                    {
                        currentState = EnemyState.CHASE;
                    }

                    if (distanceToPlayer <= maxLeapRange && distanceToPlayer >= minLeapRange && hasLeap && !isLeaping)
                    {
                        currentState = EnemyState.LEAP;
                        leapTimer = 0f;
                        isLeaping = true;
                        anim.SetWholeLeapAnimation();
                        leapDirection = Vector3.Normalize(playerTransform.position - enemyTransform.position);
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

                chaseTimer += deltaTime;
                if (chaseTimer >= chaseReplanInterval && enemyTransform != null && playerTransform != null)
                {
                    chasePath = pathfinder.FindPath(enemyTransform.position, playerTransform.position);
                    if (chasePath != null && chasePath.Count > 1)
                    {
                        pathPointIndex = 1;
                        smoothedVelocity.Clear();
                    }
                    chaseTimer = 0f;
                }

                if (chasePath != null && pathPointIndex < chasePath.Count)
                {
                    int furthestVisibleIndex = pathPointIndex;
                    Vector3 myPos = enemyTransform.position;

                    for (int i = pathPointIndex + 1; i < Math.Min(chasePath.Count, pathPointIndex + 4); i++)
                    {
                        Vector3 pointPos = chasePath[i];
                        Vector3 dirToPoint = pointPos - myPos;
                        float distToPoint = dirToPoint.Length();

                        if (distToPoint > 0.1f)
                        {
                            // si es visible el target
                        }
                    }

                    Vector3 targetPosition = chasePath[furthestVisibleIndex];
                    Vector3 delta = targetPosition - myPos;
                    float distance = delta.Length();

                    if (distance < 0.5f)
                    {
                        pathPointIndex = furthestVisibleIndex + 1;

                        if (pathPointIndex >= chasePath.Count && chasePath.Count > 0)
                        {
                            float distanceToPlayerNow = Vector3.Distance(myPos, playerTransform.position);
                            if (distanceToPlayerNow > 1.5f)
                            {
                                chasePath = pathfinder.FindPath(myPos, playerTransform.position);
                                if (chasePath != null && chasePath.Count > 1)
                                {
                                    pathPointIndex = 1;
                                    smoothedVelocity.Clear();
                                }
                            }
                        }
                    }
                    else
                    {
                        Vector3 moveDir = Vector3.Normalize(delta);
                        moveDirection = moveDir;

                        Vector3 desiredVelocity = moveDir * speedMovement;
                        Vector3 currentVelocity = rb.GetVelocity();

                        Vector3 smoothedVel = SmoothVelocity(desiredVelocity, currentVelocity, deltaTime);
                        rb.SetVelocity(smoothedVel);

                        anim.SetRunningAnimation();
                        isRunning = true;
                    }
                }
                else if (chasePath == null || chasePath.Count == 0)
                {
                    Vector3 directDir = Vector3.Normalize(playerTransform.position - enemyTransform.position);
                    moveDirection = directDir;

                    Vector3 desiredVelocity = directDir * speedMovement;
                    Vector3 currentVelocity = rb.GetVelocity();

                    Vector3 smoothedVel = SmoothVelocity(desiredVelocity, currentVelocity, deltaTime);
                    rb.SetVelocity(smoothedVel);

                    anim.SetRunningAnimation();
                    isRunning = true;
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
                    anim.SetRandomAttackAnimation();
                    hurtboxTimer = 0f;
                    dodgeTimer = 0f;
                    dodgewindow = true;
                }
                else if (dodgeTimer >= dodgeActivationTime && dodgewindow)
                {
                    Attack();

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

            case EnemyState.LEAP:
                hasLeap = false;
                leapTimer += deltaTime;
                rb.SetVelocity(leapDirection * 1.8f);
                if (leapTimer >= leapDuration)
                {
                    isLeaping = false;
                    hasLeap = true;
                    currentState = EnemyState.CHASE;
                    lastLeap = 0f;
                }
                break;
            default:
                break;
        }
    }

    private Vector3 SmoothVelocity(Vector3 desiredVelocity, Vector3 currentVelocity, float deltaTime)
    {
        if (smoothedVelocity == null)
        {
            smoothedVelocity = new List<Vector3>();
        }

        while (smoothedVelocity.Count < 3)
        {
            smoothedVelocity.Add(currentVelocity);
        }

        while (smoothedVelocity.Count > 3)
        {
            smoothedVelocity.RemoveAt(0);
        }

        Vector3 newVelocity = Vector3.Lerp(currentVelocity, desiredVelocity, pathSmoothingFactor);

        smoothedVelocity.RemoveAt(0);
        smoothedVelocity.Add(newVelocity);

        Vector3 avgVelocity = Vector3.Zero;
        foreach (var vel in smoothedVelocity)
        {
            avgVelocity += vel;
        }

        return avgVelocity / smoothedVelocity.Count;
    }

    public override void Attack()
    {
        Engineson.print("Melee attack executed!");
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

        if (leapTimer >= leapDuration)
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
            anim.SetHitAnimation();
            sound.LoadAudio("Assets/Audio/SFX/Enemies/Hormagaunt/HormagauntHit_ready.wav");
            sound?.Play();
        }
        else if (other.tag == "RailgunProjectile")
        {
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
    }

    private void DestroyHurtbox()
    {
        if (hurtboxObject != null)
        {
            Engineson.Destroy(hurtboxObject);
        }
    }
}