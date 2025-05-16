using System;
using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public class EnemyControllerWarrior : EnemyController
{
    private float health = 100.0f;
    private float projectileDamage = 15.0f;
    private float swordDamage = 25.0f;
    private float distanceToPlayer;
    private bool hasDropped = false;

    private float hurtboxActivationTime = 1.5f;
    private float hurtboxTimer = 0f;
    private Vector3 hurtboxSize = new Vector3(10.0f, 10.0f, 10.0f);
    private Vector3 hurtboxOffset = new Vector3(5.0f, -3.0f, 0.0f);
    private GameObject hurtboxObject;

    private float projectileRange = 30;
    private List<BulletData> activeProjectiles = new List<BulletData>();
    public float shootCooldown = 2.0f;
    public float projectileSpeed = 90.0f;
    public float projectileLifetime = 0.5f;
    protected float shootTimer = 0f;

    private bool dodgewindow = false;
    private float dodgeActivationTime = 0.5f;
    private float dodgeTimer = 0f;
    private PlayerController pc;
    private Transform transform;

    bool isCombatMusicPlaying = false;
    private string combatMusic = "Assets/Audio/PlaceHolder_CombatMusic.wav";
    private string WalkSound = "Assets/Audio/TyranidWarrior/Tyranid_WAR_Walk.wav";
    private string MeleeAttackSound = "Assets/Audio/TyranidWarrior/Tyranid_WAR_Melee_Atack_1.wav";
    private string RangedAttackSound = "Assets/Audio/TyranidWarrior/Tyranid_WAR_Ranged_Atack_1.wav";
    private string DeathSound = "Assets/Audio/TyranidWarrior/TyranidWAR_Death.wav";
    private string GrowlSound = "Assets/Audio/TyranidWarrior/Tyranid_WAR_Grwol_1.wav";
    private string HitSound = "Assets/Audio/TyranidWarrior/Tyranid_WAR_Hit_1.wav";

    private bool hasPlayedDeathSound = false;
    private WarriorAnimation anim;
    private List<Vector3> bulletDirections = new List<Vector3>();
    List<HashSet<GameObject>> bulletHitEnemies = new List<HashSet<GameObject>>();
    private List<Vector3> bulletStartPositions = new List<Vector3>();
    private List<float> bulletLifetimes = new List<float>();
    public List<BulletData> bullets = new List<BulletData>();
    public List<Vector3> bulletsPos = new List<Vector3>();
    public List<Vector3> hitPoints = new List<Vector3>();
    public List<string> collisionNames = new List<string>();
    public List<GameObject> bulletsObjects = new List<GameObject>();
    public List<float> bulletIntervals = new List<float>();
    public int magazineSize;
    public int currentMagazineAmmo;
    public int maxAmmo;
    public int currentTotalAmmo;
    public float range;
    public float timeToLerp = 0.1f;

    private bool componentsInitialized = false;

    public override void Awake()
    {
        try
        {
            Transform enemyTransform = gameObject.GetComponent<Transform>();
            if (enemyTransform != null)
            {
                startPosition = enemyTransform.position;
            }
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in EnemyControllerWarrior.Awake: {e.Message}");
        }
    }

    public override void Start()
    {
        try
        {
            GameObject playerObj = GameObject.Find("Player");
            if (playerObj == null)
            {
                Engineson.print("ERROR: Player not found!");
                return;
            }

            pc = playerObj.GetComponent<PlayerController>();
            playerTransform = playerObj.GetComponent<Transform>();

            if (playerTransform == null)
            {
                Engineson.print("ERROR: Player Transform not found!");
                return;
            }

            rb = gameObject.GetComponent<Rigidbody>();
            if (rb == null)
            {
                Engineson.print("ERROR: Rigidbody not found!");
                return;
            }

            collider = gameObject.GetComponent<BoxCollider>();
            if (collider == null)
            {
                Engineson.print("ERROR: Tyranid Warrior Collider not found!");
                return;
            }

            transform = gameObject.GetComponent<Transform>();
            if (transform == null)
            {
                Engineson.print("ERROR: Transform component not found!");
                return;
            }

            enemyTransform = transform;

            GameObject meshObject = gameObject.GetChild("TyranidWarriorMesh");
            if (meshObject != null)
            {
                anim = meshObject.GetComponent<WarriorAnimation>();
                if (anim == null)
                {
                    Engineson.print("ERROR: WarriorAnimation component not found!");
                    return;
                }
            }
            else
            {
                Engineson.print("ERROR: TyranidWarriorMesh not found!");
                return;
            }

            maxHealth = health;
            currentHealth = maxHealth;
            gameObject.tag = "Warrior";
            projectileDamage = 20.0f;
            range = 100f;
            timeToLerp = 0.1f;

            componentsInitialized = true;
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in EnemyControllerWarrior.Start: {e.Message}");
        }
    }

    public override void Update(float deltaTime)
    {
        if (!componentsInitialized)
            return;

        try
        {
            if (currentState == EnemyState.DEAD)
            {
                HandleDeadState();
                return;
            }

            if (currentHealth <= 0)
            {
                currentState = EnemyState.DEAD;
                return;
            }

            if (currentState == EnemyState.STUNNED)
            {
                HandleStunnedState(deltaTime);
                return;
            }

            UpdateBullets(deltaTime);
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

                default:
                    break;
            }

            CleanupProjectiles();
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in EnemyControllerWarrior.Update: {e.Message}");
        }
    }

    private void HandleIdleState()
    {
        try
        {
            isFootstepPlaying = false;
            if (!hasStoppedFootsteps)
            {
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
                Audio.PlayOneShot(GrowlSound);
                Audio.SchedulePlay(WalkSound, 0.5f);
                isFootstepPlaying = true;
                hasStoppedFootsteps = false;
            }

            if (!isCombatMusicPlaying)
            {
                isCombatMusicPlaying = true;
            }

            Vector3 currentVelocity = rb.GetVelocity();
            moveDirection = Vector3.Normalize(playerTransform.position - transform.position);
            Vector3 desiredVelocity = moveDirection * speedMovement;

            if (anim != null)
            {
                anim.SetRunAnimation();
            }

            if (desiredVelocity.LengthSquared() > 0)
            {
                desiredVelocity = Vector3.Normalize(desiredVelocity) * speedMovement;
            }

            Vector3 newVelocity = Vector3.Lerp(currentVelocity, desiredVelocity, acceleration * deltaTime);
            rb.SetVelocity(new Vector3(newVelocity.X, currentVelocity.Y, newVelocity.Z));

            UpdateRotation(deltaTime);
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
            if (isAttacking)
            {
                hurtboxTimer += deltaTime;
                if (dodgewindow)
                {
                    dodgeTimer += deltaTime;
                }

                if (hurtboxTimer >= hurtboxActivationTime)
                {
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
            }
            else if (isShooting)
            {
                shootTimer += deltaTime;
                if (shootTimer >= shootCooldown)
                {
                    Attack();
                    shootTimer = 0;
                }
            }

            UpdateRotation(deltaTime);
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
            stunTimer += deltaTime;
            if (stunTimer >= stunDuration)
            {
                currentState = EnemyState.IDLE;
                isStunned = false;
                stunTimer = 0.0f;
            }
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in HandleStunnedState: {e.Message}");
        }
    }

    private void HandleDeadState()
    {
        try
        {
            if (!hasPlayedDeathSound)
            {
                Audio.PlayOneShot(DeathSound);
                hasPlayedDeathSound = true;
            }

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

            if (anim != null)
            {
                anim.SetDeathAnimation();
            }

            if (collider != null)
            {
                collider.SetActive(false);
            }

            isDead = true;
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in HandleDeadState: {e.Message}");
        }
    }

    private void UpdateRotation(float deltaTime)
    {
        try
        {
            if (transform == null || collider == null || moveDirection == Vector3.Zero)
                return;

            currentRotationAngle = transform.eulerAngles.Y;
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
        catch (Exception e)
        {
            Engineson.print($"ERROR in UpdateRotation: {e.Message}");
        }
    }

    private void UpdatePlayerDetection()
    {
        try
        {
            if (playerTransform == null || enemyTransform == null)
                return;

            distanceToPlayer = Vector3.Distance(enemyTransform.position, playerTransform.position);

            if (distanceToPlayer < distToChase)
            {
                if (IsPlayerInHurtbox(playerTransform.position))
                {
                    currentState = EnemyState.ATTACK;
                    isAttacking = true;
                }
                else if (distanceToPlayer < projectileRange)
                {
                    if (rb != null)
                    {
                        rb.SetVelocity(Vector3.Zero);
                    }
                    currentState = EnemyState.ATTACK;
                    isShooting = true;
                }
                else
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
                    anim.SetIdleAnimation();
                }
            }
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in UpdatePlayerDetection: {e.Message}");
        }
    }

    private void UpdateBullets(float deltaTime)
    {
        try
        {
            if (bulletsObjects.Count != bulletsPos.Count ||
                bulletsObjects.Count != bulletDirections.Count ||
                bulletsObjects.Count != bulletIntervals.Count ||
                bulletsObjects.Count != bulletLifetimes.Count ||
                bulletsObjects.Count != bulletHitEnemies.Count ||
                bulletsObjects.Count != bulletStartPositions.Count)
            {
                CleanAllBullets();
                return;
            }

            for (int i = bulletsObjects.Count - 1; i >= 0; i--)
            {
                if (i >= bulletsObjects.Count || bulletsObjects[i] == null)
                {
                    RemoveBulletAtIndex(i);
                    continue;
                }

                bulletIntervals[i] += deltaTime;
                bulletLifetimes[i] += deltaTime;

                Vector3 currentPos = bulletsPos[i];
                Vector3 direction = bulletDirections[i];
                float speed = projectileRange / timeToLerp;
                Vector3 displacement = direction * speed * deltaTime;
                Vector3 newPos = currentPos + displacement;
                bool shouldDestroy = false;

                GameObject hitObject = null;
                RayCast ray = new RayCast();
                ray.PerformRaycast(currentPos, direction, displacement.Length());

                if (ray.hit.isHit)
                {
                    hitObject = ray.hit.gameObject;
                }

                if (hitObject != null)
                {
                    string tag = hitObject.tag;

                    if (tag != "PowerUp" && tag != "Ammunition" && tag != "Warrior")
                    {
                        if (!bulletHitEnemies[i].Contains(hitObject))
                        {
                            bulletHitEnemies[i].Add(hitObject);

                            if (tag == "Player" && pc != null && pc.playerData != null)
                            {
                                pc.playerData.TakeDamage(projectileDamage);

                                pc.StartFlashColor(pc.flashColor, pc.flashDuration);
                            }

                            shouldDestroy = true;
                        }
                    }
                }

                bulletsPos[i] = newPos;

                Transform bulletTransform = bulletsObjects[i].GetComponent<Transform>();
                if (bulletTransform != null)
                {
                    bulletTransform.position = newPos;
                }

                float distanceTraveled = Vector3.Distance(bulletStartPositions[i], newPos);
                if (distanceTraveled > projectileRange || shouldDestroy || bulletLifetimes[i] > 2.0f)
                {
                    RemoveBulletAtIndex(i);
                }
            }
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in UpdateBullets: {e.Message}");
        }
    }

    private void RemoveBulletAtIndex(int index)
    {
        try
        {
            if (index < 0 || index >= bulletsObjects.Count)
                return;

            if (bulletsObjects[index] != null)
            {
                Engineson.Destroy(bulletsObjects[index]);
            }

            bulletsObjects.RemoveAt(index);
            bulletsPos.RemoveAt(index);
            bulletDirections.RemoveAt(index);
            bulletIntervals.RemoveAt(index);
            bulletLifetimes.RemoveAt(index);
            bulletHitEnemies.RemoveAt(index);
            bulletStartPositions.RemoveAt(index);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR removing bullet at index {index}: {e.Message}");
            CleanAllBullets();
        }
    }

    private void CleanAllBullets()
    {
        try
        {
            foreach (GameObject bullet in bulletsObjects)
            {
                if (bullet != null)
                {
                    Engineson.Destroy(bullet);
                }
            }

            bulletsObjects.Clear();
            bulletsPos.Clear();
            bulletDirections.Clear();
            bulletIntervals.Clear();
            bulletLifetimes.Clear();
            bulletHitEnemies.Clear();
            bulletStartPositions.Clear();
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in CleanAllBullets: {e.Message}");
        }
    }

    public override void ResetEnemyCheckPoint()
    {
        try
        {
            if (isDead)
                return;

            currentHealth = maxHealth;

            if (gameObject.GetComponent<Collider>() != null)
            {
                gameObject.GetComponent<Collider>().SetPosition(startPosition);
            }

            if (rb != null)
            {
                rb.SetVelocity(Vector3.Zero);
            }

            currentState = EnemyState.IDLE;
            isAttacking = false;
            isShooting = false;
            hasPlayedDeathSound = false;
            hasDropped = false;
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
            if (isAttacking)
            {
                Audio.PlayOneShot(MeleeAttackSound);

                if (pc != null && pc.playerData != null)
                {
                    if (pc.redThirstManager != null && pc.redThirstManager.IsInBlackRage())
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
                }

                if (anim != null)
                {
                    anim.SetMeleeAnimation();
                }
            }
            else if (isShooting && transform != null)
            {
                Vector3 localOffset = new Vector3(0f, 0f, 0f);
                Vector3 bulletStart = transform.position +
                                      (transform.right * localOffset.X) +
                                      (transform.up * localOffset.Y) +
                                      (transform.forward * localOffset.Z);
                bulletStart.Y += 0.5f;

                Vector3 direction = Vector3.Normalize(transform.forward);
                float yaw = (float)(Math.Atan2(direction.X, direction.Z) * (180.0 / Math.PI));
                float pitch = (float)(-Math.Asin(direction.Y) * (180.0 / Math.PI));

                GameObject projectile = Engineson.CreateGameObject("WarriorProjectile", null);
                if (projectile == null)
                {
                    Engineson.print("ERROR: Failed to create projectile");
                    return;
                }

                Transform projectileTransform = projectile.GetComponent<Transform>();
                if (projectileTransform != null)
                {
                    projectileTransform.SetScale(0.25f, 0.25f, 0.25f);
                    projectileTransform.position = bulletStart;
                    projectileTransform.SetRotation(pitch, yaw, 0f);
                }

                ParticleFX particleFX = projectile.AddComponent<ParticleFX>();
                if (particleFX != null)
                {
                    particleFX.ApplyPreset(14);
                    particleFX.EmitBurst(1);
                }

                if (anim != null)
                {
                    anim.SetShootAnimation();
                }

                bulletsObjects.Add(projectile);
                bulletsPos.Add(bulletStart);
                bulletDirections.Add(direction);
                bulletIntervals.Add(0);
                bulletLifetimes.Add(0);
                bulletHitEnemies.Add(new HashSet<GameObject>());
                bulletStartPositions.Add(bulletStart);

                Audio.PlayOneShot(RangedAttackSound);
            }
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

            Audio.PlayOneShot(HitSound);
            currentHealth -= damage;

            if (anim != null)
            {
                anim.SetHurtAnimation();
            }
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

    private void UpdateProjectiles(float deltaTime)
    {
        try
        {
            foreach (BulletData proj in activeProjectiles)
            {
                if (proj == null || proj.markedForDestruction)
                    continue;

                proj.lifetime += deltaTime;

                if (proj.lifetime >= projectileLifetime)
                {
                    proj.markedForDestruction = true;
                    continue;
                }

                if (proj.transform != null)
                {
                    proj.transform.position += proj.direction * projectileSpeed * deltaTime;
                }
            }
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in UpdateProjectiles: {e.Message}");
        }
    }

    private void CleanupProjectiles()
    {
        try
        {
            for (int i = activeProjectiles.Count - 1; i >= 0; i--)
            {
                if (i >= activeProjectiles.Count)
                    continue;

                BulletData proj = activeProjectiles[i];
                if (proj == null || proj.markedForDestruction)
                {
                    if (proj != null && proj.gameObject != null)
                    {
                        Engineson.Destroy(proj.gameObject);
                    }

                    activeProjectiles.RemoveAt(i);
                }
            }
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in CleanupProjectiles: {e.Message}");
        }
    }
}