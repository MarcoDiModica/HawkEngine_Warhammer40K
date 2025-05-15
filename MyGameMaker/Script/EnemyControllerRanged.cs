using System;
using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public class EnemyControllerRanged : EnemyController
{
    private List<BulletData> activeProjectiles = new List<BulletData>();
    public float shootCooldown = 2.0f;
    public float projectileSpeed = 90.0f;
    public float projectileLifetime = 0.5f;
    protected float shootTimer = 0f;
    private PlayerController pc;
    private Transform transform;
    private TermagauntAnimation anim;
    //stats
    private float health = 100.0f;
    private float damage = 20.0f;

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
    public float timeToLerp;

    //audio
    bool isCombatMusicPlaying = false;
//     private AudioSource music;
//     private string combatMusic = "Assets/Audio/PlaceHolder_CombatMusic.wav";
//     private AudioClip combatSound;

    public override void Awake()
    {
        //music = gameObject.GetComponent<AudioSource>();
        startPosition = gameObject.GetComponent<Transform>().position;
    }
    public override void Start()
    {

        playerTransform = GameObject.Find("Player").GetComponent<Transform>();
        rb = gameObject.GetComponent<Rigidbody>();
        pc = GameObject.Find("Player").GetComponent<PlayerController>();
        if (playerTransform == null)
        {
            Engineson.print("ERROR: Player couldn't be found!");
        }

        collider = gameObject.GetComponent<BoxCollider>();
        if (collider == null)
        {
            Engineson.print("ERROR: PlayerMovement requires a Collider component!");
            return;
        }
        transform = gameObject.GetComponent<Transform>();
        if (transform == null)
        {
            Engineson.print("ERROR: PlayerMovement requires a Transform component!");
            return;
        }

        anim = gameObject.GetChild("TermagauntMesh").GetComponent<TermagauntAnimation>();
        if (anim == null)
        {
            Engineson.print("ERROR: Anim requires a TermagauntAnimation component!");
            return;
        }

        //         sound = gameObject.GetComponent<AudioSource>();
        //         if (sound == null)
        //         {
        //             Engineson.print("PlayerShooting: Audio component not found");
        //         }

        enemyTransform = gameObject.GetComponent<Transform>();
        if (enemyTransform == null)
        {
            Engineson.print("ERROR: PlayerMovement requires a Transform component!");
            return;
        }

        particles = gameObject.AddComponent<ParticleFX>();
        maxHealth = health;
        currentHealth = maxHealth;
        gameObject.tag = "Ranged";
        damage = 20.0f;
        range = 30f;
        timeToLerp = 0.1f;

        //         combatSound = new AudioClip(combatMusic, "CombatMusic", true, false);
        //         sound.LoadAudioClip(combatSound);
    }

    public override void Update(float deltaTime)
    {
        if (!isDead)
        {
            if (currentHealth <= 0)
            {
                Engineson.print("This man is dead man.");
                anim.SetDeathAnimation();
                //Destroy(gameObject);
                isDead = true;
            }
            if (!isStunned)
            {
                Vector3 playerPos = playerTransform.position;
                for (int i = bulletsObjects.Count - 1; i >= 0; i--)
                {
                    bulletIntervals[i] += deltaTime;
                    bulletLifetimes[i] += deltaTime;

                    Vector3 currentPos = bulletsPos[i];
                    Vector3 direction = bulletDirections[i];
                    float speed = range / timeToLerp;
                    Vector3 displacement = direction * speed * deltaTime;
                    Vector3 newPos = currentPos + displacement;
                    bool shouldDestroy = false;
                    Engineson.print(""+currentPos);
                    Engineson.print(""+ direction);
                    Engineson.print("" + displacement.Length());
                    GameObject hitObject = null;

                    RayCast ray = new RayCast();
                    ray.PerformRaycast(currentPos, direction, displacement.Length());

                    if (ray.hit.isHit)
                    {
                        Engineson.print("Le di gente");
                        hitObject = ray.hit.gameObject;
                    }

                    if (hitObject != null)
                    {
                        string tag = hitObject.tag;

                        if (tag != "PowerUp" && tag != "Ammunition")
                        {
                            if (!bulletHitEnemies[i].Contains(hitObject))
                            {
                                bulletHitEnemies[i].Add(hitObject);
                                Engineson.print("Le di otra vez");

                                switch (tag)
                                {
                                    case "Player":
                                        pc.playerData.TakeDamage(damage);
                                        Engineson.print("Le redi gente");
                                        break;
                                }
                            }
                        }
                        
                    }
                    bulletsPos[i] = newPos;
                    bulletsObjects[i].GetComponent<Transform>().position = newPos;
                    float distanceTraveled = Vector3.Distance(bulletStartPositions[i], newPos);
                    if (distanceTraveled > range || shouldDestroy)
                    {
                        Engineson.Destroy(bulletsObjects[i]);
                        bulletsObjects.RemoveAt(i);
                        bulletsPos.RemoveAt(i);
                        bulletDirections.RemoveAt(i);
                        bulletIntervals.RemoveAt(i);
                        bulletLifetimes.RemoveAt(i);
                        bulletHitEnemies.RemoveAt(i);
                        bulletStartPositions.RemoveAt(i);
                    }
                }
                if (Vector3.Distance(enemyTransform.position, playerPos) < distToChase)
                {
                    if (shootTimer <= 0)
                    {
                        Attack();
                        shootTimer = shootCooldown;
                    }

                    if (isCombatMusicPlaying == false)
                    {
                        //sound?.Play(combatSound);
                        isCombatMusicPlaying = true;
                    }
                    else
                    {
                        shootTimer -= deltaTime;
                    }

                    if (Vector3.Distance(enemyTransform.position, playerPos) > minDistToChase)
                    {
                        Vector3 currentVelocity = rb.GetVelocity();
                        moveDirection = Vector3.Normalize(playerPos - gameObject.GetComponent<Transform>().position);
                        Vector3 desiredVelocity = moveDirection * speedMovement;

                        if (desiredVelocity.LengthSquared() > 0)
                        {
                            desiredVelocity = Vector3.Normalize(desiredVelocity) * speedMovement;
                        }

                        Vector3 newVelocity = Vector3.Lerp(currentVelocity, desiredVelocity, acceleration * deltaTime);
                        rb.SetVelocity(new Vector3(newVelocity.X, currentVelocity.Y, newVelocity.Z));
                        anim.SetRunningAnimation();
                        //enemyTransform.position += desiredVelocity * deltaTime;
                    }
                }
                else
                {
                    rb.SetVelocity(Vector3.Zero);
                    anim.SetStandardIdleAnimation();
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
            else if (isStunned)
            {
                stunTimer += deltaTime;
                rb.SetVelocity(Vector3.Zero);
                if (stunTimer >= stunDuration)
                {
                    isStunned = false;
                    stunTimer = 0.0f;
                }
            }
            if (isDead)
            {
                // Enemy Death
                collider.SetActive(false);
                if (pc.playerData.isPiercing == true)
                {
                    pc.playerData.AddHealth(5.0f);
                }
            }
        }
    }
    public override void ResetEnemyCheckPoint()
    {
        if (!isDead)
        {
            currentHealth = maxHealth;
            isStunned = false;
            rb.SetVelocity(Vector3.Zero);
            collider.SetActive(true);
            gameObject.GetComponent<Collider>().SetPosition(startPosition);
            //sound?.Stop();
            isCombatMusicPlaying = false;
        }
        
    }

    public override void Attack()
    {
        //int audio = Audio.PlayOneShot(boltgunShot);
        Vector3 localOffset = new Vector3(-0.9f, 2.5f, 0.5f); // Y = altura, Z = hacia adelante, X = lateral si se desea

        Vector3 bulletStart = transform.position +
                              (transform.right * localOffset.X) +
                              (transform.up * localOffset.Y) +
                              (transform.forward * localOffset.Z);
        bulletStart.Y += 0.5f;

        Vector3 direction = Vector3.Normalize(transform.forward);

        float yaw = (float)(Math.Atan2(direction.X, direction.Z) * (180.0 / Math.PI));
        float pitch = (float)(-Math.Asin(direction.Y) * (180.0 / Math.PI));


        GameObject projectile = Engineson.CreateGameObject("TermagauntProjectile", null);
        //projectile.AddComponent<MeshRenderer>();
        projectile.transform.SetScale(0.25f, 0.25f, 0.25f);
        projectile.transform.position = bulletStart;
        projectile.transform.SetRotation(pitch, yaw, 0f);
        projectile.AddComponent<ParticleFX>();
        projectile.GetComponent<ParticleFX>().ApplyPreset(14);
        projectile.GetComponent<ParticleFX>().EmitBurst(1);

        anim.SetAttackAnimation();

        bulletsObjects.Add(projectile);
        bulletsPos.Add(bulletStart);
        bulletDirections.Add(direction);
        bulletIntervals.Add(0);
        bulletLifetimes.Add(0);
        bulletHitEnemies.Add(new HashSet<GameObject>());
        bulletStartPositions.Add(bulletStart);
    }

    public override void TakeDamage(float damage)
    {
        currentHealth -= damage;
        particles.ApplyPreset(19);
        particles.EmitBurst(1);
        anim.SetHitAnimation();
        Engineson.print("Hit");
    }
    override public void OnCollisionEnter(GameObject other)
    {

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
