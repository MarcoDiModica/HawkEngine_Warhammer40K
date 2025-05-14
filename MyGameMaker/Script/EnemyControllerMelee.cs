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
    private PlayerController pc;

    bool isCombatMusicPlaying = false;
    private const string MUSIC_COMBAT = "Assets/Audio/PlaceHolder_CombatMusic.wav";
    private const string SFX_DEATH = "Assets/Audio/SFX/Enemies/Hormagaunt/HormagauntDeath_ready.wav";
    private const string SFX_FOOTSTEP = "Assets/Audio/SFX/Enemies/Hormagaunt/HormagauntFootstep_ready.wav";
    private const string SFX_ATTACK = "Assets/Audio/SFX/Enemies/Hormagaunt/HormagauntMeleeAttack_ready.wav";
    private const string SFX_HIT = "Assets/Audio/SFX/Enemies/Hormagaunt/HormagauntHit_ready.wav";

    private float health = 50.0f;
    private float clawDamage = 5.0f;
    private float leapDamage = 7.0f;
    private float distanceToPlayer;
    private bool hasDropped = false;

    public float maxLeapRange = 20.0f;
    public float minLeapRange = 10.0f;
    private float leapTimer = 0f;
    private float leapDuration = 1.5f;
    private float anticipationTimer = 0f;
    private float anticipationDuration = 1f;
    private float lastLeap = 0f;
    public float leapCooldown = 2.0f;
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

    private float obstacleDetectDist = 5.0f; 
    private bool isAvoidingObstacle = false;
    private Vector3 avoidDirection = Vector3.Zero;
    private float avoidRotationSpeed = 5f;
    private float avoidTimer = 0f;
    private float avoidTimeLimit = 1.0f;
    private static readonly Random _rng = new Random();

    private float deathTimer = 0f;
    private float deathCooldown = 2f;


    private Vector3 GetDodgeDirection(Vector3 forward)
    {
        Vector3 left = Vector3.Normalize(new Vector3(-forward.Z, 0, forward.X));
        Vector3 right = Vector3.Normalize(new Vector3(forward.Z, 0, -forward.X));
        return (_rng.NextDouble() < 0.5) ? left : right;
    }

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

        enemyTransform = gameObject.GetComponent<Transform>();
        if (enemyTransform == null)
        {
            Engineson.print("ERROR: Hormagaunt Movement requires a Transform component!");
            return;
        }

        anim = gameObject.GetChild("HormagauntMesh").GetComponent<HormagauntAnimation>();
        if (anim == null)
        {
            Engineson.print("ERROR: HormagauntAnimation requires a SkeletalAnimation component!");
            return;
        }

        renderer = gameObject.GetChild("HormagauntMesh").GetComponent<MeshRenderer>();

        particles = gameObject.AddComponent<ParticleFX>();
        particles.ApplyPreset(9);
        

        maxHealth = health;
        currentHealth = maxHealth;
        gameObject.tag = "Melee";

        startPosition = enemyTransform.position;

        pathfinder = new Pathfinding();
        pathfinder.UpdateGridOrigin(enemyTransform.position);
        lastPathfindingPosition = enemyTransform.position;
        chasePath = null;
        pathInitialized = false;
        currentState = EnemyState.IDLE;
    }

    public override void Update(float deltaTime)
    {
       // Engineson.print("Enemy update");
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
                Audio.PlayOneShot(SFX_DEATH);
                return;
            }

            if (currentState != EnemyState.STUNNED)
            {
                distanceToPlayer = Vector3.Distance(enemyTransform.position, playerTransform.position);

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

                    if (distanceToPlayer <= maxLeapRange && distanceToPlayer >= minLeapRange && hasLeap && !isLeaping)
                    {
                        currentState = EnemyState.LEAP;
                        leapTimer = 0f;
                        isLeaping = true;
                        anim.SetWholeLeapAnimation();
                        leapDirection = Vector3.Normalize(playerTransform.position - enemyTransform.position);
                    }

                    if (distanceToPlayer > minDistToChase)
                    {
                        currentState = EnemyState.CHASE;
                    }



                    if (moveDirection != Vector3.Zero)
                    {
                        //currentRotationAngle = GetComponent<Transform>().eulerAngles.Y;
                        //float targetAngle = (float)Math.Atan2(moveDirection.X, moveDirection.Z);
                        //float targetAngleDegrees = targetAngle * (180.0f / (float)Math.PI);

                        //while (targetAngleDegrees - currentRotationAngle > 180.0f) targetAngleDegrees -= 360.0f;
                        //while (targetAngleDegrees - currentRotationAngle < -180.0f) targetAngleDegrees += 360.0f;

                        //currentRotationAngle = Lerp(currentRotationAngle, targetAngleDegrees, rotationSpeed * deltaTime);

                        //Vector3 eulerRotation = new Vector3(0, currentRotationAngle, 0);
                        //Quaternion newRotation = Quaternion.CreateFromYawPitchRoll(
                        //    eulerRotation.Y * ((float)Math.PI / 180.0f),
                        //    eulerRotation.X * ((float)Math.PI / 180.0f),
                        //    eulerRotation.Z * ((float)Math.PI / 180.0f)
                        //);

                        //collider.SetRotation(newRotation);
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
                    //Audio.Play(MUSIC_COMBAT, true);
                    isCombatMusicPlaying = true;
                }

                //chaseTimer += deltaTime;
                //if (chaseTimer >= chaseReplanInterval && enemyTransform != null && playerTransform != null)
                //{
                //    chasePath = pathfinder.FindPath(enemyTransform.position, playerTransform.position);
                //    if (chasePath != null && chasePath.Count > 1)
                //    {
                //        pathPointIndex = 1;
                //        smoothedVelocity.Clear();
                //    }
                //    chaseTimer = 0f;
                //}

                //if (chasePath != null && pathPointIndex < chasePath.Count)
                //{
                //    Vector3 myPos = enemyTransform.position;
                //    Vector3 targetPosition = chasePath[pathPointIndex];
                //    Vector3 toTarget = targetPosition - myPos;
                //    float dist = toTarget.Length();

                //    if (dist < 0.5f)
                //    {
                //        pathPointIndex++;
                //        break;
                //    }

                //    Vector3 forward = Vector3.Normalize(toTarget);
                //    GameObject hitObject = null;

                //    {
                //        RayCast ray = new RayCast();
                //        ray.PerformRaycast(myPos, forward, obstacleDetectDist);
                //        if (ray.hit.isHit)
                //            hitObject = ray.hit.gameObject;
                //    }

                //    if (!isAvoidingObstacle)
                //    {
                //        if (hitObject != null && hitObject.tag == "Collider")
                //        {
                //            isAvoidingObstacle = true;
                //            avoidDirection = GetDodgeDirection(forward);
                //            avoidTimer = 0f;
                //        }
                //        else
                //        {
                //            moveDirection = forward;
                //        }
                //    }
                //    else
                //    {
                //        avoidTimer += deltaTime;

                //        if (avoidTimer >= avoidTimeLimit)
                //        {
                //            moveDirection = avoidDirection;
                //            isAvoidingObstacle = false;
                //        }
                //        else
                //        {
                //            bool forwardBlocked = false;
                //            RayCast ray = new RayCast();
                //            ray.PerformRaycast(myPos, forward, obstacleDetectDist);
                //            forwardBlocked = ray.hit.isHit && ray.hit.gameObject.tag == "Collider";

                //            if (!forwardBlocked)
                //            {
                //                isAvoidingObstacle = false;
                //                moveDirection = forward;
                //            }
                //            else
                //            {
                //                RayCast checkDodge = new RayCast();
                //                checkDodge.PerformRaycast(myPos, avoidDirection, obstacleDetectDist);
                //                if (checkDodge.hit.isHit && checkDodge.hit.gameObject.tag == "Collider")
                //                {
                //                    avoidDirection = -avoidDirection;
                //                    moveDirection = avoidDirection;
                //                }
                //            }
                //        }
                //    }
                //    float currentYAngle = enemyTransform.eulerAngles.Y;
                //    float targetYAngle = (float)(Math.Atan2(moveDirection.X, moveDirection.Z) * 180.0 / Math.PI);
                //    float deltaAngle = targetYAngle - currentYAngle;
                //    //while (deltaAngle > 180f) deltaAngle -= 360f;
                //    //while (deltaAngle < -180f) deltaAngle += 360f;
                //    float newY = currentYAngle + deltaAngle * Math.Min(1f, avoidRotationSpeed * deltaTime);
                //    collider.SetRotation(
                //        Quaternion.CreateFromYawPitchRoll(newY * ((float)Math.PI / 180f), 0, 0)
                //    );

                //    Vector3 desiredVel = moveDirection * speedMovement;
                //    Vector3 currVel = rb.GetVelocity();
                //    Vector3 smoothVel = SmoothVelocity(desiredVel, currVel, deltaTime);
                //    rb.SetVelocity(smoothVel);

                //    anim.SetRunningAnimation();
                //}
                //else if (chasePath == null || chasePath.Count == 0)
                //{
                Vector3 directDir = Vector3.Normalize(playerTransform.position - enemyTransform.position);
                moveDirection = directDir;

                Vector3 desiredVelocity = directDir * speedMovement;
                Vector3 currentVelocity = rb.GetVelocity();

                Vector3 smoothedVel = SmoothVelocity(desiredVelocity, currentVelocity, deltaTime);
                rb.SetVelocity(smoothedVel);
                //}

                currentRotationAngle = enemyTransform.eulerAngles.Y;
                float targetAngle = (float)Math.Atan2(moveDirection.X, moveDirection.Z);
                float targetAngleDegrees = targetAngle * (180.0f / (float)Math.PI);

                //while (targetAngleDegrees - currentRotationAngle > 180.0f) targetAngleDegrees -= 360.0f;
                //while (targetAngleDegrees - currentRotationAngle < -180.0f) targetAngleDegrees += 360.0f;

                currentRotationAngle = Lerp(currentRotationAngle, targetAngleDegrees, rotationSpeed * deltaTime);

                Vector3 eulerRotation = new Vector3(0, currentRotationAngle, 0);
                Quaternion newRotation = Quaternion.CreateFromYawPitchRoll(
                    eulerRotation.Y * ((float)Math.PI / 180.0f),
                    eulerRotation.X * ((float)Math.PI / 180.0f),
                    eulerRotation.Z * ((float)Math.PI / 180.0f)
                );

                collider.SetRotation(newRotation);

                anim.SetRunningAnimation();
                //  }

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

                Vector3 directAttackDir = Vector3.Normalize(playerTransform.position - enemyTransform.position);
                moveDirection = directAttackDir;

                currentRotationAngle = enemyTransform.eulerAngles.Y;
                float targetAttackAngle = (float)Math.Atan2(moveDirection.X, moveDirection.Z);
                float targetAttackAngleDegrees = targetAttackAngle * (180.0f / (float)Math.PI);

                currentRotationAngle = Lerp(currentRotationAngle, targetAttackAngleDegrees, rotationSpeed * deltaTime);

                Vector3 eulerRotationAttack = new Vector3(0, currentRotationAngle, 0);
                Quaternion newAttackRotation = Quaternion.CreateFromYawPitchRoll(
                    eulerRotationAttack.Y * ((float)Math.PI / 180.0f),
                    eulerRotationAttack.X * ((float)Math.PI / 180.0f),
                    eulerRotationAttack.Z * ((float)Math.PI / 180.0f)
                );

                collider.SetRotation(newAttackRotation);

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
                
                if ((!hasDropped))
                {
                    GameObject.Find("DropManager").GetComponent<DropManager>().SpawnPrefab(this);
                }
                if (isFootstepPlaying)
                {
                    Audio.Stop(SFX_FOOTSTEP);
                    Audio.Stop(MUSIC_COMBAT);
                    isFootstepPlaying = false;
                }
                if (anim.isAnimFinished)
                {
                    deathTimer += deltaTime;
                    if (deathTimer >= deathCooldown)
                    {
                        Engineson.Destroy(gameObject);
                    }
                }
                hasDropped = true;
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
                    //currentState = EnemyState.CHASE;
                    lastLeap = 0f;
                }
                
                break;
            default:
                break;
        }
        if (isFlashingColor)
        {
            flashTimer -= deltaTime;
            if (flashTimer <= 0.0f)
            {
                if (renderer != null)
                {
                    renderer.SetColor(originalColor);
                }
                isFlashingColor = false;
            }
        }
       // Engineson.print("Enemy end update");
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

    public override void ResetEnemyCheckPoint()
    {
        if (!isDead)
        {
            currentHealth = maxHealth;
            currentState = EnemyState.IDLE;
            isStunned = false;
            isAttacking = false;
            isShooting = false;
            isFootstepPlaying = false;
            hasStoppedFootsteps = false;
            hasDropped = false;
            hasLeap = true;
            leapTimer = 0f;
            leapDirection = Vector3.Zero;
            collider.SetActive(true);
            gameObject.GetComponent<Collider>().SetPosition(startPosition);
        }
        
    }

    public override void Attack()
    {
        Engineson.print("Melee attack executed!");
        if (pc.redThirstManager.redThirstBonus < clawDamage)
        {
            pc.playerData.TakeDamage(clawDamage - pc.redThirstManager.redThirstBonus);
            pc.StartFlashColor(pc.flashColor, pc.flashDuration);
        }
        else
        {
            pc.playerData.TakeDamage(0);
        }
        Engineson.print("Player health: " + (pc.playerData.GetHealth()));

        Audio.PlayOneShot(SFX_ATTACK);
    }

    public override void TakeDamage(float damage)
    {
        if (currentHealth > 0)
        {
            currentHealth -= damage;
            StartFlashColor(flashColor, flashDuration);
            anim.SetHitAnimation();
            particles.ApplyPreset(19);
            particles.EmitBurst(1);
            Audio.PlayOneShot(SFX_HIT);
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
                    pc.StartFlashColor(pc.flashColor, pc.flashDuration);
                }
                else
                {
                    pc.playerData.TakeDamage(0);
                }
            }
            else
            {
                pc.playerData.TakeDamage(leapDamage);
                pc.StartFlashColor(pc.flashColor, pc.flashDuration);
            }
            Engineson.print(other.tag + " health: " + (pc.playerData.GetHealth()));
        }
    }

    public void StartFlashColor(Vector4 color, float duration)
    {
        if (renderer != null)
        {
            renderer.SetColor(color);
            isFlashingColor = true;
            flashTimer = duration;
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