using System;
using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public class EnemyControllerRanged : EnemyController
{
    private List<BulletData> activeProjectiles = new List<BulletData>();
    public float shootCooldown = 2.0f;
    public float projectileSpeed = 30.0f;
    public float projectileLifetime = 0.5f;
    protected float shootTimer = 0f;
    private PlayerController pc;
    private Transform transform;
    private TermagauntAnimation anim;
    private RedThirstManager redThirstManager;

    private bool isCombatMusicPlaying = false;
    private const string MUSIC_COMBAT = "Assets/Audio/PlaceHolder_CombatMusic.wav";
    private const string SFX_DEATH = "Assets/Audio/SFX/Enemies/Hormagaunt/HormagauntDeath_ready.wav";
    private const string SFX_ATTACK = "Assets/Audio/SFX/Enemies/Hormagaunt/HormagauntMeleeAttack_ready.wav";
    private const string SFX_HIT = "Assets/Audio/SFX/Enemies/Hormagaunt/HormagauntHit_ready.wav";


    private float health = 50.0f;
    private float damage = 10.0f;

    private List<Vector3> bulletDirections = new List<Vector3>();
    private List<HashSet<GameObject>> bulletHitEnemies = new List<HashSet<GameObject>>();
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
    public float timeToLerp;


    private bool componentsInitialized = false;
    private float deathTimer = 0f;
    private float deathCooldown = 2f;
    private bool hasChangedVelocity;
    private bool needsCleanup;
    public override void Awake()
    {
        try
        {
            startPosition = gameObject.GetComponent<Transform>()?.position ?? Vector3.Zero;
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in EnemyControllerRanged.Awake: {e.Message}");
        }
    }

    public override void Start()
    {
        try
        {
            GameObject playerObj = GameObject.Find("Player");
            if (playerObj != null)
            {
                playerTransform = playerObj.GetComponent<Transform>();
                pc = playerObj.GetComponent<PlayerController>();

                if (playerTransform == null)
                {
                    Engineson.print("ERROR: Player Transform couldn't be found!");
                }

                if (pc == null)
                {
                    Engineson.print("ERROR: PlayerController couldn't be found!");
                }
            }
            else
            {
                Engineson.print("ERROR: Player GameObject couldn't be found!");
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

            transform = gameObject.GetComponent<Transform>();
            if (transform == null)
            {
                Engineson.print("ERROR: Transform component not found!");
                return;
            }

            enemyTransform = transform;

            GameObject meshObject = gameObject.GetChild("TermagauntMesh");
            if (meshObject != null)
            {
                anim = meshObject.GetComponent<TermagauntAnimation>();
                if (anim == null)
                {
                    Engineson.print("ERROR: TermagauntAnimation component not found!");
                }
            }
            else
            {
                Engineson.print("ERROR: TermagauntMesh child object not found!");
            }

            renderer = meshObject?.GetComponent<MeshRenderer>();
            if (renderer == null)
            {
                Engineson.print("ERROR: Renderer component not found!");
            }

            particles = gameObject.AddComponent<ParticleFX>();

            maxHealth = health;
            currentHealth = maxHealth;
            gameObject.tag = "Ranged";
            damage = 20.0f;
            range = 40f;
            timeToLerp = 0.5f;
            rb.SetFriction(5);

            componentsInitialized = true;
            Engineson.print("EnemyControllerRanged initialized successfully");

            redThirstManager = playerObj.GetComponent<RedThirstManager>();
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in EnemyControllerRanged.Start: {e.Message}");
        }
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

        if (needsCleanup)
        {
            CleanBullets();
            needsCleanup = false;
        }

        if (!componentsInitialized)
            return;

        try
        {
            if (isDead)
            {
                renderer?.SetColor(new Vector4(1, 1, 1, 1));
                return;
            }

            if (currentHealth <= 0 && !isDead)
            {
                Engineson.print("This enemy has died.");
                Audio.PlayOneShot(SFX_DEATH);
                if (anim != null)
                {
                    anim.SetDeathAnimation();
                }
                redThirstManager.AddRedThirstPoint(1);
                isDead = true;

                if (collider != null)
                {
                    collider.SetActive(false);
                }

                if (anim.isAnimFinished)
                {
                    deathTimer += deltaTime;
                    if (deathTimer >= deathCooldown)
                    {
                        Engineson.Destroy(gameObject);
                        return;
                    }
                }

                if (pc != null && pc.playerData != null && pc.playerData.isPiercing)
                {
                    pc.playerData.AddHealth(5.0f);
                }

                return;
            }

            if (isStunned)
            {
                stunTimer += deltaTime;
                if (rb != null)
                {
                    rb.SetVelocity(Vector3.Zero);
                }

                if (stunTimer >= stunDuration)
                {
                    isStunned = false;
                    stunTimer = 0.0f;
                }

                return;
            }

            if (isSlowed)
            {
                HandleSlowedState(deltaTime);
            }

            UpdateBullets(deltaTime);

            if (playerTransform != null && enemyTransform != null)
            {
                Vector3 playerPos = playerTransform.position;
                float distanceToPlayer = Vector3.Distance(enemyTransform.position, playerPos);

                if (distanceToPlayer < distToChase)
                {
                    if (shootTimer <= 0)
                    {
                        Attack();
                        shootTimer = shootCooldown;
                    }
                    else
                    {
                        shootTimer -= deltaTime;
                    }

                    if (!isCombatMusicPlaying)
                    {
                        isCombatMusicPlaying = true;
                    }

                    if (distanceToPlayer > minDistToChase)
                    {
                        moveDirection = Vector3.Normalize(playerPos - enemyTransform.position);

                        if (rb != null)
                        {
                            Vector3 currentVelocity = rb.GetVelocity();
                            Vector3 desiredVelocity;
                            if (isSlowed)
                            {
                                desiredVelocity = moveDirection * slowedSpeed;
                            }
                            else
                            {
                                desiredVelocity = moveDirection * speedMovement;
                            }

                            if (desiredVelocity.LengthSquared() > 0)
                            {
                                desiredVelocity = Vector3.Normalize(desiredVelocity) * speedMovement;
                            }

                            Vector3 newVelocity = Vector3.Lerp(currentVelocity, desiredVelocity, acceleration * deltaTime);
                            rb.SetVelocity(new Vector3(newVelocity.X, currentVelocity.Y, newVelocity.Z));
                        }

                        if (anim != null)
                        {
                            anim.SetRunningAnimation();
                        }
                    }
                }
                else
                {
                    if (rb != null)
                    {
                        rb.SetVelocity(Vector3.Zero);
                    }

                    if (anim != null)
                    {
                        anim.SetStandardIdleAnimation();
                    }
                }

                if (moveDirection != Vector3.Zero && collider != null)
                {
                    UpdateRotation(deltaTime);
                }
            }

            CleanupProjectiles();
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in EnemyControllerRanged.Update: {e.Message}");
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
                Engineson.print("ERROR: Bullet collections out of sync, cleaning bullets");
                CleanBullets();
                return;
            }

            for (int i = bulletsObjects.Count - 1; i >= 0; i--)
            {
                if (bulletsObjects[i] == null)
                {
                    RemoveBulletAtIndex(i);
                    continue;
                }

                bulletIntervals[i] += deltaTime;
                bulletLifetimes[i] += deltaTime;

                Vector3 currentPos = bulletsPos[i];
                Vector3 direction = bulletDirections[i];
                float speed = range / timeToLerp;
                Vector3 displacement = direction * speed * deltaTime;
                Vector3 newPos = currentPos + displacement;

                bool shouldDestroy = false;
                GameObject hitObject = null;

                try
                {
                    RayCast ray = new RayCast();
                    ray.PerformRaycast(currentPos, direction, displacement.Length());

                    if (ray.hit.isHit)
                    {
                        hitObject = ray.hit.gameObject;
                    }
                }
                catch (Exception e)
                {
                    Engineson.print($"ERROR in raycast: {e.Message}");
                }

                if (hitObject != null)
                {
                    string tag = hitObject.tag;

                    if (tag != "PowerUp" && tag != "Ammunition" && tag != "Ranged")
                    {
                        if (!bulletHitEnemies[i].Contains(hitObject))
                        {
                            bulletHitEnemies[i].Add(hitObject);

                            if (tag == "Player" && pc != null && pc.playerData != null)
                            {
                                pc.playerData.TakeDamage(damage);
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
                if (distanceTraveled > range || shouldDestroy || bulletLifetimes[i] > 2.0f)
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
        if (index < 0 || index >= bulletsObjects.Count)
            return;

        try
        {
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
            needsCleanup = true;
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
    private void CleanBullets()
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
            Engineson.print($"ERROR in CleanBullets: {e.Message}");
        }
    }

    private void UpdateRotation(float deltaTime)
    {
        try
        {
            Transform enemyT = GetComponent<Transform>();
            if (enemyT == null)
                return;

            currentRotationAngle = enemyT.eulerAngles.Y;

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

    public override void ResetEnemyCheckPoint()
    {
        try
        {
            if (isDead)
                return;

            currentHealth = maxHealth;
            isStunned = false;

            if (rb != null)
            {
                rb.SetVelocity(Vector3.Zero);
            }

            if (collider != null)
            {
                collider.SetActive(true);
                collider.SetPosition(startPosition);
            }

            isCombatMusicPlaying = false;
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in ResetEnemyCheckPoint: {e.Message}");
        }
    }

    public override void Attack()
    {
        if (!componentsInitialized || transform == null)
            return;

        try
        {
            Vector3 localOffset = new Vector3(-0.9f, 2.5f, 0.5f);
            Vector3 bulletStart = transform.position +
                                  (transform.right * localOffset.X) +
                                  (transform.up * localOffset.Y) +
                                  (transform.forward * localOffset.Z);
            bulletStart.Y += 0.5f;

            Vector3 direction = Vector3.Normalize(transform.forward);

            float yaw = (float)(Math.Atan2(direction.X, direction.Z) * (180.0 / Math.PI));
            float pitch = (float)(-Math.Asin(direction.Y) * (180.0 / Math.PI));

            GameObject projectile = Engineson.CreateGameObject("TermagauntProjectile", null);
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
                particleFX.ApplyPreset(10);
                particleFX.EmitBurst(1);
            }

            if (anim != null)
            {
                anim.SetAttackAnimation();
            }

            Audio.PlayOneShot(SFX_ATTACK);

            bulletsObjects.Add(projectile);
            bulletsPos.Add(bulletStart);
            bulletDirections.Add(direction);
            bulletIntervals.Add(0);
            bulletLifetimes.Add(0);
            bulletHitEnemies.Add(new HashSet<GameObject>());
            bulletStartPositions.Add(bulletStart);
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
            currentHealth -= damage;

            if (particles != null)
            {
                EnemySquirting();
            }

            StartFlashColor(flashColor, flashDuration);

            if (anim != null)
            {
                anim.SetHitAnimation();
            }

            Audio.PlayOneShot(SFX_HIT);
            Engineson.print("Enemy took damage: " + damage);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in TakeDamage: {e.Message}");
        }
    }

    public override void OnCollisionEnter(GameObject other)
    {
    }

    private void UpdateProjectiles(float deltaTime)
    {
        try
        {
            for (int i = activeProjectiles.Count - 1; i >= 0; i--)
            {
                BulletData proj = activeProjectiles[i];
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

    private void CleanupProjectiles()
    {
        try
        {
            for (int i = activeProjectiles.Count - 1; i >= 0; i--)
            {
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