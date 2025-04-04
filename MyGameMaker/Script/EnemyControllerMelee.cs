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
    private Vector3 hurtboxOffset = new Vector3(5.0f, 0.0f, 0.0f); // Desplazamiento de la hurtbox hacia adelante
    private GameObject hurtboxObject;

    // Perfect Dodge
    private bool dodgewindow = false;
    private float dodgeActivationTime = 0.5f;
    private float dodgeTimer = 0f;
    private HormagauntAnimation anim;
    PlayerController pc;

    //audio
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
            Engineson.print("ERROR: PlayerMovement requires a Collider component!");
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
            Engineson.print("ERROR: PlayerMovement requires a Transform component!");
            return;
        }

        anim = GameObject.Find("HormagauntMesh").GetComponent<HormagauntAnimation>();
        if (anim == null)
        {
            Engineson.print("ERROR: PlayerAnimation requires a SkeletalAnimation component!");
            return;
        }

        pc = GameObject.Find("Player").GetComponent<PlayerController>();
        maxHealth = health;
        currentHealth = maxHealth;
        gameObject.tag = "Melee";
        isDead = false;
    }

    public override void Update(float deltaTime)
    {
        if (!isDead)
        {
            if (currentHealth <= 0)
            {
                Engineson.print("This man is dead man.");
                //Destroy(gameObject);
                anim.SetDeathAnimation();
                isDead = true;
                sound.LoadAudio("Assets/Audio/SFX/Enemies/Hormagaunt/HormagauntDeath_ready.wav");
                sound?.Play();
            }
            if (!isStunned)
            {
                Vector3 playerPos = playerTransform.position;
                float distanceToPlayer = Vector3.Distance(enemyTransform.position, playerPos);

                if (distanceToPlayer < distToChase)
                {
                    // Enemy Attack
                    if (IsPlayerInHurtbox(playerPos))
                    {
                        hurtboxTimer += deltaTime;
                        if (dodgewindow)
                        {
                            dodgeTimer += deltaTime;
                        }
                        if (hurtboxTimer >= hurtboxActivationTime)
                        {
                            //CreateHurtbox();
                            anim.SetRandomAttackAnimation();
                            Engineson.print("Atack is ready");
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
                        }
                    }

                    // Enemy Movement
                    if (Vector3.Distance(enemyTransform.position, playerPos) > minDistToChase && !isAttacking)
                    {
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
                        moveDirection = Vector3.Normalize(playerPos - gameObject.GetComponent<Transform>().position);
                        Vector3 desiredVelocity = moveDirection * speedMovement;
                   
                        if (desiredVelocity.LengthSquared() > 0)
                        {
                            desiredVelocity = Vector3.Normalize(desiredVelocity) * speedMovement;
                        }
                   
                        Vector3 newVelocity = Vector3.Lerp(currentVelocity, desiredVelocity, acceleration * deltaTime);
                        rb.SetVelocity(new Vector3(newVelocity.X, currentVelocity.Y, newVelocity.Z));
                    }

                    // Enemy Leap
                    if (distanceToPlayer <= maxLeapRange && distanceToPlayer >= minLeapRange && hasLeap)
                    {
                        Random random = new Random();
                        int rand = random.Next(3, 4);
                        int rand2 = random.Next(3, 4);

                        if (rand == rand2)
                        {
                            leapTimer += deltaTime;
                            Leap();
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
                    // Enemy Idle
                    if (!isIdle)
                    {
                        rb.SetVelocity(Vector3.Zero);
                        anim.SetStandardIdleAnimation();
                        isIdle = true;
                    }
                    isRunning = false;
                    isFootstepPlaying = false;
                    if (!hasStoppedFootsteps)
                    {
                        sound?.Stop();
                        hasStoppedFootsteps = true;
                    }
                }
            }
            else if (isStunned)
            {
                // Enemy Stun
                stunTimer += deltaTime;
                rb.SetVelocity(Vector3.Zero);
                if (stunTimer >= stunDuration)
                {
                    isStunned = false;
                    stunTimer = 0.0f;
                }
            }
        }
        if (isDead)
        {
            // Enemy Death
            collider.SetActive(false);
        }
    }

    public override void Attack()
    {
        Engineson.print("Melee attack executed!");
        pc.playerData.TakeDamage(clawDamage);
        Engineson.print("Player health: " + (pc.playerData.GetHealth()));

        sound.LoadAudio("Assets/Audio/SFX/Enemies/Hormagaunt/HormagauntMeleeAttack_ready.wav");
        sound?.Play();
    }

    public void Leap()
    {
        isLeaping = true;
        lastLeap = 0.0f;
        if (leapTimer <= leapDuration)  rb.SetVelocity(rb.GetVelocity() * 1.8f);
        anim.SetWholeLeapAnimation();
    }

    private bool IsPlayerInHurtbox(Vector3 playerPos)
    {
        Vector3 hurtboxCenter = enemyTransform.position + (enemyTransform.forward * hurtboxOffset.X) + (Vector3.UnitY * hurtboxOffset.Y);
        Vector3 halfSize = hurtboxSize * 0.5f;
        //Engineson.print("Player in hurtbox");

        return (playerPos.X >= hurtboxCenter.X - halfSize.X && playerPos.X <= hurtboxCenter.X + halfSize.X) &&
               (playerPos.Y >= hurtboxCenter.Y - halfSize.Y && playerPos.Y <= hurtboxCenter.Y + halfSize.Y) &&
               (playerPos.Z >= hurtboxCenter.Z - halfSize.Z && playerPos.Z <= hurtboxCenter.Z + halfSize.Z);
    }

    public override void TakeDamage(float damage)
    {
        currentHealth -= damage;
        anim.SetHitAnimation();
        sound.LoadAudio("Assets/Audio/SFX/Enemies/Hormagaunt/HormagauntHit_ready.wav");
        sound?.Play();
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
        else if (other.tag == "Player" && isLeaping)
        {
            Engineson.print("Player hit while Leaping!");
            pc.playerData.TakeDamage(leapDamage);
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
    }

    private void DestroyHurtbox()
    {
        if (hurtboxObject != null)
        {
            Engineson.Destroy(hurtboxObject);
        }
    }
}
