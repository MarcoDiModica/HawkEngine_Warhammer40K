using HawkEngine;
using System;
using System.Collections.Generic;
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
    private RedThirstManager redThirstManager;

    private bool isCombatMusicPlaying = false;
    private const string MUSIC_COMBAT = "Assets/Audio/PlaceHolder_CombatMusic.wav";
    private const string SFX_DEATH = "Assets/Audio/SFX/Enemies/Hormagaunt/HormagauntDeath_ready.wav";
    private const string SFX_FOOTSTEP = "Assets/Audio/SFX/Enemies/Hormagaunt/HormagauntFootstep_ready.wav";
    private const string SFX_ATTACK = "Assets/Audio/SFX/Enemies/Hormagaunt/HormagauntMeleeAttack_ready.wav";
    private const string SFX_HIT = "Assets/Audio/SFX/Enemies/Hormagaunt/HormagauntHit_ready.wav";
    private const string SFX_LEAP = "Assets/Audio/SFX/Enemies/Hormagaunt/Hormagaunt_Leap_Atack.wav";

    private float health = 50.0f;
    private float clawDamage = 10.0f;
    private float leapDamage = 0.0f;
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

    private bool componentsInitialized = false;
    public bool isSpawning = false;
    public float spawnTimer = 0.0f;
    public float spawnDuration = 4.0f;

    private bool hasChangedVelocity = false;

    private ParticleFX leapParticles;

    private Vector3 GetDodgeDirection(Vector3 forward)
    {
        try
        {
            Vector3 left = Vector3.Normalize(new Vector3(-forward.Z, 0, forward.X));
            Vector3 right = Vector3.Normalize(new Vector3(forward.Z, 0, -forward.X));
            return (_rng.NextDouble() < 0.5) ? left : right;
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in GetDodgeDirection: {e.Message}");
            return Vector3.UnitX;
        }
    }

    public override void Awake()
    {
        try
        {
            isDead = false;
            isStunned = false;
            isAttacking = false;
            isShooting = false;

            smoothedVelocity = new List<Vector3>();
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in EnemyControllerMelee.Awake: {e.Message}");
        }
    }

    public override void Start()
    {
        try
        {
            GameObject playerObj = GameObject.Find("Player");
            if (playerObj == null)
            {
                Engineson.print("ERROR: Player GameObject couldn't be found!");
                return;
            }

            playerTransform = playerObj.GetComponent<Transform>();
            if (playerTransform == null)
            {
                Engineson.print("ERROR: Player Transform couldn't be found!");
                return;
            }

            pc = playerObj.GetComponent<PlayerController>();
            if (pc == null)
            {
                Engineson.print("ERROR: PlayerController component not found on Player!");
                return;
            }

            rb = gameObject.GetComponent<Rigidbody>();
            if (rb == null)
            {
                Engineson.print("ERROR: Rigidbody component not found!");
                return;
            }

            collider = gameObject.GetComponent<CapsuleCollider>();
            if (collider == null)
            {
                Engineson.print("ERROR: BoxCollider component not found!");
                return;
            }

            enemyTransform = gameObject.GetComponent<Transform>();
            if (enemyTransform == null)
            {
                Engineson.print("ERROR: Transform component not found!");
                return;
            }

            GameObject meshObject = gameObject.GetChild("HormagauntMesh");
            if (meshObject == null)
            {
                Engineson.print("ERROR: HormagauntMesh child object not found!");
                return;
            }

            anim = meshObject.GetComponent<HormagauntAnimation>();
            if (anim == null)
            {
                Engineson.print("ERROR: HormagauntAnimation component not found!");
                return;
            }

            renderer = meshObject.GetComponent<MeshRenderer>();
            if (renderer == null)
            {
                Engineson.print("WARNING: MeshRenderer component not found!");
            }

            particles = gameObject.AddComponent<ParticleFX>();
            if (particles != null)
            {
                particles.ApplyPreset(9);
            }

            maxHealth = health;
            currentHealth = maxHealth;

            gameObject.tag = "Melee";

            startPosition = enemyTransform.position;

            pathfinder = new Pathfinding();
            if (pathfinder != null)
            {
                pathfinder.UpdateGridOrigin(enemyTransform.position);
                lastPathfindingPosition = enemyTransform.position;
                chasePath = null;
                pathInitialized = false;
            }

            if (leapParticles == null)
            {
                leapParticles = gameObject.AddComponent<ParticleFX>();
                leapParticles.ApplyPreset(9);
            }

            currentState = EnemyState.IDLE;

            componentsInitialized = true;
            Engineson.print("EnemyControllerMelee initialized successfully");

            redThirstManager = playerObj.GetComponent<RedThirstManager>();
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in EnemyControllerMelee.Start: {e.Message}");
        }
        rb.SetMass(2);
    }

    public override void Update(float deltaTime)
    {
        if (SceneManager.isPaused)
        {
            if (!hasChangedVelocity && rb != null)
            {
                rb.SetVelocity(Vector3.Zero);
                hasChangedVelocity = true;
            }
            return;
        }
        else if (hasChangedVelocity)
        {
            hasChangedVelocity = false;
        }

        if (!componentsInitialized)
            return;

        try
        {
            if (!pathInitialized && pathfinder != null && enemyTransform != null && playerTransform != null)
            {
                pathfinder.UpdateGridOrigin(enemyTransform.position);
                pathInitialized = true;
            }

            if (isSpawning)
            {
                HandleSpawnState(deltaTime);
                return;
            }

            if (isSlowed)
            {
                HandleSlowedState(deltaTime);
            }

            if (currentState == EnemyState.DEAD)
            {
                HandleDeadState(deltaTime);
                return;
            }

            if (currentHealth <= 0 && currentState != EnemyState.DEAD)
            {
                SetDeadState();
                return;
            }

            if (currentState == EnemyState.STUNNED)
            {
                HandleStunnedState(deltaTime);
                return;
            }

            UpdatePlayerDetection();

            switch (currentState)
            {
                case EnemyState.IDLE:
                    HandleIdleState();
                    break;

                case EnemyState.CHASE:
                    HandleChaseState(deltaTime);
                    break;

                case EnemyState.ATTACK:
                    HandleAttackState(deltaTime);
                    break;

                case EnemyState.LEAP:
                    HandleLeapState(deltaTime);
                    break;
            }

            if (isFlashingColor)
            {
                flashTimer -= deltaTime;
                if (flashTimer <= 0.0f && renderer != null)
                {
                    renderer.SetColor(originalColor);
                    isFlashingColor = false;
                }
            }
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in EnemyControllerMelee.Update: {e.Message}");
        }
    }

    private void HandleIdleState()
    {
        try
        {
            isFootstepPlaying = false;
            if (!hasStoppedFootsteps)
            {
                Audio.Stop(SFX_FOOTSTEP);
                hasStoppedFootsteps = true;
            }
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in HandleIdleState: {e.Message}");
        }
    }

    private void HandleChaseState(float deltaTime)
    {
        try
        {
            if (!isFootstepPlaying)
            {
                Audio.Play(SFX_FOOTSTEP, true);
                isFootstepPlaying = true;
                hasStoppedFootsteps = false;
            }

            if (!isCombatMusicPlaying)
            {
                //Audio.Play(MUSIC_COMBAT, true);
                isCombatMusicPlaying = true;
            }

            if (enemyTransform != null && playerTransform != null && collider != null && rb != null)
            {
                Vector3 directDir = Vector3.Normalize(playerTransform.position - enemyTransform.position);
                moveDirection = directDir;
                Vector3 desiredVelocity;
                if (isSlowed)
                {
                    desiredVelocity = directDir * slowedSpeed;
                }
                else
                {
                    desiredVelocity = directDir * speedMovement;
                }
                Vector3 currentVelocity = rb.GetVelocity();
                Vector3 smoothedVel = SmoothVelocity(desiredVelocity, currentVelocity, deltaTime);
                rb.SetVelocity(smoothedVel);

                UpdateRotationToFaceDirection(moveDirection, deltaTime);
            }

            if (anim != null)
            {
                anim.SetRunningAnimation();
            }
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in HandleChaseState: {e.Message}");
        }
    }

    private void HandleAttackState(float deltaTime)
    {
        try
        {
            hurtboxTimer += deltaTime;
            if (dodgewindow)
            {
                dodgeTimer += deltaTime;
            }

            if (hurtboxTimer >= hurtboxActivationTime)
            {
                if (anim != null)
                {
                    anim.SetRandomAttackAnimation();
                }

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

            if (enemyTransform != null && playerTransform != null)
            {
                Vector3 directAttackDir = Vector3.Normalize(playerTransform.position - enemyTransform.position);
                moveDirection = directAttackDir;
                UpdateRotationToFaceDirection(moveDirection, deltaTime);
            }
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in HandleAttackState: {e.Message}");
        }
    }

    private void HandleStunnedState(float deltaTime)
    {
        try
        {
            if (rb != null)
            {
                rb.SetVelocity(Vector3.Zero);
            }

            stunTimer += deltaTime;
            if (stunTimer >= stunDuration)
            {
                currentState = EnemyState.IDLE;
                stunTimer = 0.0f;
            }
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in HandleStunnedState: {e.Message}");
        }
    }

    private void HandleSpawnState(float deltaTime)
    {
        collider.SetActive(false);
        try
        {

            spawnTimer += deltaTime;
            anim.SetClimbingAnimation();


            if (spawnTimer >= spawnDuration)
            {
                isSpawning = false;
                spawnTimer = 0;
                collider.SetActive(true);
            }
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in HandleLeapState: {e.Message}");
        }
    }
    private void HandleLeapState(float deltaTime)
    {
        try
        {
            hasLeap = false;

            leapTimer += deltaTime;

            if (rb != null && leapDirection != Vector3.Zero)
            {
                rb.SetVelocity(leapDirection * 1.8f);
            }

            try
            {
                if (leapParticles != null)
                {
                    //leapParticles.EmitBurst(1);
                }
            }
            catch (Exception particleEx)
            {
                Engineson.print($"WARNING: Leap particle effect failed: {particleEx.Message}");
            }

            if (leapTimer >= leapDuration)
            {
                isLeaping = false;
                hasLeap = true;
                lastLeap = 0f;
                currentState = EnemyState.CHASE;
            }
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in HandleLeapState: {e.Message}");
        }
    }

    private void HandleSlowedState(float deltaTime)
    {
        try
        {
            slowedTimer += deltaTime;
            if (slowedTimer >= slowedDuration)
            {
                isSlowed = false;
                slowedTimer = 0.0f;
            }
        }
        catch (Exception e)
        {
            Engineson.print($"Error in HandleSlowedState: {e.Message}");
        }
    }

    private void HandleDeadState(float deltaTime)
    {
        try
        {
            if (!hasDropped)
            {
                GameObject dropManager = GameObject.Find("DropManager");
                if (dropManager != null)
                {
                    DropManager dropComponent = dropManager.GetComponent<DropManager>();
                    if (dropComponent != null)
                    {
                        dropComponent.SpawnPrefab(this);
                    }
                }
                hasDropped = true;
            }

            if (isFlashingColor)
            {
                flashTimer -= deltaTime;
                if (isFootstepPlaying)
                {
                    Audio.Stop(SFX_FOOTSTEP);
                    Audio.Stop(MUSIC_COMBAT);
                    isFootstepPlaying = false;
                }

                if (anim != null && anim.isAnimFinished)
                {
                    bool once = false;
                    if (enemyTransform != null && !once)
                    {
                        once = true;
                    }

                    deathTimer += deltaTime;
                    if (deathTimer >= deathCooldown)
                    {
                        Engineson.Destroy(gameObject);
                    }
                }

                if (collider != null)
                {
                    collider.SetActive(false);
                }
            }
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in HandleDeadState: {e.Message}");
        }
    }

    private void SetDeadState()
    {
        try
        {
            currentState = EnemyState.DEAD;
            isDead = true;
            redThirstManager.AddRedThirstPoint(1);
            renderer?.SetColor(new Vector4(1, 1, 1, 1));

            if (anim != null)
            {
                anim.SetDeathAnimation();
            }

            Audio.PlayOneShot(SFX_DEATH);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetDeadState: {e.Message}");
        }
    }

    private void UpdatePlayerDetection()
    {
        try
        {
            if (enemyTransform == null || playerTransform == null)
                return;

            distanceToPlayer = Vector3.Distance(enemyTransform.position, playerTransform.position);

            if (distanceToPlayer < distToChase)
            {
                if (IsPlayerInHurtbox(playerTransform.position))
                {
                    currentState = EnemyState.ATTACK;
                }
                else if (distanceToPlayer <= maxLeapRange &&
                        distanceToPlayer >= minLeapRange &&
                        hasLeap &&
                        !isLeaping)
                {
                    PrepareLeap();
                }
                else if (distanceToPlayer > minDistToChase)
                {
                    currentState = EnemyState.CHASE;
                }
            }
            else if (currentState != EnemyState.IDLE)
            {
                currentState = EnemyState.IDLE;
                if (rb != null)
                {
                    rb.SetVelocity(Vector3.Zero);
                }
                if (anim != null)
                {
                    anim.SetStandardIdleAnimation();
                }
            }
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in UpdatePlayerDetection: {e.Message}");
        }
    }

    private void PrepareLeap()
    {
        try
        {
            currentState = EnemyState.LEAP;
            leapTimer = 0f;
            isLeaping = true;

            if (anim != null)
            {
                anim.SetWholeLeapAnimation();
            }

            if (playerTransform != null && enemyTransform != null)
            {
                leapDirection = Vector3.Normalize(playerTransform.position - enemyTransform.position);
            }
            else
            {
                leapDirection = Vector3.UnitZ;
            }
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in PrepareLeap: {e.Message}");
        }
    }

    private void UpdateRotationToFaceDirection(Vector3 direction, float deltaTime)
    {
        try
        {
            if (collider == null || enemyTransform == null || direction == Vector3.Zero)
                return;

            currentRotationAngle = enemyTransform.eulerAngles.Y;
            float targetAngle = (float)Math.Atan2(direction.X, direction.Z);
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
        catch (Exception e)
        {
            Engineson.print($"ERROR in UpdateRotationToFaceDirection: {e.Message}");
        }
    }

    private Vector3 SmoothVelocity(Vector3 desiredVelocity, Vector3 currentVelocity, float deltaTime)
    {
        try
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
        catch (Exception e)
        {
            Engineson.print($"ERROR in SmoothVelocity: {e.Message}");
            return currentVelocity;
        }
    }

    public override void ResetEnemyCheckPoint()
    {
        try
        {
            if (isDead)
                return;

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

            if (collider != null)
            {
                collider.SetActive(true);
                collider.SetPosition(startPosition);
            }
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in ResetEnemyCheckPoint: {e.Message}");
        }
    }

    public override void Attack()
    {
        try
        {
            Engineson.print("Melee attack executed!");

            if (pc != null && pc.playerData != null)
            {
                float effectiveDamage = clawDamage;

                if (pc.redThirstManager != null && pc.redThirstManager.redThirstBonus < clawDamage)
                {
                    effectiveDamage = clawDamage - pc.redThirstManager.redThirstBonus;
                    pc.playerData.TakeDamage(effectiveDamage);

                    pc.StartFlashColor(pc.flashColor, pc.flashDuration);
                }
                else if (pc.redThirstManager != null)
                {
                    pc.playerData.TakeDamage(0);
                }
                else
                {
                    pc.playerData.TakeDamage(clawDamage);

                    pc.StartFlashColor(pc.flashColor, pc.flashDuration);
                }

                Engineson.print("Player health: " + (pc.playerData.GetHealth()));
            }

            Audio.PlayOneShot(SFX_ATTACK);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in Attack: {e.Message}");
        }
    }

    public override void TakeDamage(float damage)
    {
        try
        {
            if (currentHealth <= 0)
                return;

            if (particles != null)
            {
                //EnemySquirting();
            }

            currentHealth -= damage;

            StartFlashColor(flashColor, flashDuration);

            anim?.SetHitAnimation();

            //Audio.PlayOneShot(SFX_HIT);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in TakeDamage: {e.Message}");
        }
    }

    private bool IsPlayerInHurtbox(Vector3 playerPos)
    {
        try
        {
            if (enemyTransform == null)
                return false;

            Vector3 hurtboxCenter = enemyTransform.position +
                                    (enemyTransform.forward * hurtboxOffset.X) +
                                    (Vector3.UnitY * hurtboxOffset.Y);
            Vector3 halfSize = hurtboxSize * 0.5f;

            return (playerPos.X >= hurtboxCenter.X - halfSize.X && playerPos.X <= hurtboxCenter.X + halfSize.X) &&
                   (playerPos.Y >= hurtboxCenter.Y - halfSize.Y && playerPos.Y <= hurtboxCenter.Y + halfSize.Y) &&
                   (playerPos.Z >= hurtboxCenter.Z - halfSize.Z && playerPos.Z <= hurtboxCenter.Z + halfSize.Z);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in IsPlayerInHurtbox: {e.Message}");
            return false;
        }
    }

    public override void OnCollisionEnter(GameObject other)
    {
        try
        {
            if (other == null || pc == null || !isLeaping || isDead) return;

            if (other.tag == "Player")
            {
                Engineson.print(other.tag + " hit with Leap");

                if (pc.playerData != null)
                {
                    if (pc.redThirstManager != null && pc.redThirstManager.IsInBlackRage())
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
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in OnCollisionEnter: {e.Message}");
        }
    }

    public void StartFlashColor(Vector4 color, float duration)
    {
        try
        {
            if (renderer != null)
            {
                renderer?.SetColor(color);
                isFlashingColor = true;
                flashTimer = duration;
            }
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in StartFlashColor: {e.Message}");
        }
    }

    private void CreateHurtbox()
    {
        try
        {
            if (enemyTransform == null)
                return;

            hurtboxObject = Engineson.CreateGameObject("Hurtbox", null);
            if (hurtboxObject == null)
            {
                Engineson.print("ERROR: Failed to create hurtbox object");
                return;
            }

            hurtboxObject.AddComponent<MeshRenderer>();
            Transform hurtboxTransform = hurtboxObject.AddComponent<Transform>();

            if (hurtboxTransform != null)
            {
                hurtboxTransform.position = enemyTransform.position +
                                           (enemyTransform.forward * hurtboxOffset.X) +
                                           (Vector3.UnitY * hurtboxOffset.Y);
                hurtboxTransform.SetScale(hurtboxSize.X, hurtboxSize.Y, hurtboxSize.Z);
            }

            BoxCollider hurtboxCollider = hurtboxObject.AddComponent<BoxCollider>();
            if (hurtboxCollider != null)
            {
                hurtboxCollider.SetTrigger(true);
            }

            hurtboxObject.tag = "EnemyAttack";
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in CreateHurtbox: {e.Message}");
        }
    }

    private void DestroyHurtbox()
    {
        try
        {
            if (hurtboxObject != null)
            {
                Engineson.Destroy(hurtboxObject);
                hurtboxObject = null;
            }
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in DestroyHurtbox: {e.Message}");
        }
    }
}