using HawkEngine;
using System;
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

    // Enemy Stats
    private float health = 100.0f;
    private float damage = 20.0f;

        // Claw Attack
    private float lastClawTime = 0f;
    public float clawCooldown = 2f;

        // Leap Attack
    public float maxLeapRange = 15f;
    public float minLeapRange = 7f;
    private float leapTimer = 0f;
    public float leapTime = 2f;
    public float leapCooldown = 8f;
    private bool hasLeap = true;

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

        collider = gameObject.GetComponent<BoxCollider>();
        if (collider == null)
        {
            Engineson.print("ERROR: PlayerMovement requires a Collider component!");
            return;
        }

        soundAttack = gameObject.GetComponent<Audio>();
        if (soundAttack == null)
        {
            Engineson.print("PlayerShooting: Audio component not found");
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
        isDead = false;
    }

    public override void Update(float deltaTime)
    {
        if(!isDead)
        {
            if(!isStunned)
            {
                Vector3 playerPos = playerTransform.position;
                float distanceToPlayer = Vector3.Distance(enemyTransform.position, playerPos);

                if (distanceToPlayer < distToChase)
                {
                    if (distanceToPlayer <= maxLeapRange && distanceToPlayer >= minLeapRange)
                    {
                        if (hasLeap)
                        {
                            Engineson.print("ENEMY HAS LEAP");
                            /*Random random = new Random();
                            int rand = random.Next(0, 4);
                            if (random.Next(0, 100) == (8 + 1 * rand))
                            {
                                Leap();
                            }*/
                            Leap();
                            hasLeap = false;
                        }
                        leapTimer += deltaTime;

                        if (leapTimer >= leapCooldown)
                        {
                            Engineson.print("LEAP RESTORED");
                            hasLeap = true;
                        }
                    }
                    if (IsPlayerInHurtbox(playerPos))
                    {
                        //Engineson.print("Player in hurtbox");
                        hurtboxTimer += deltaTime;
                        if (dodgewindow)
                        {
                            dodgeTimer += deltaTime;
                        }
                        if (hurtboxTimer >= hurtboxActivationTime)
                        {
                            //CreateHurtbox();
                            Engineson.print("Atack is ready");
                            hurtboxTimer = 0f;
                            dodgeTimer = 0f;
                            dodgewindow = true;
                        }
                        else if (dodgeTimer >= 0.5f && dodgewindow)
                        {
                            Attack();
                            soundAttack?.Play();
                            //DestroyHurtbox();
                            hurtboxTimer = 0f;
                            dodgeTimer = 0f;
                            dodgewindow = false;
                        }
                    }
                    if (Vector3.Distance(enemyTransform.position, playerPos) < distToChase)
                    {

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

                            //enemyTransform.position += desiredVelocity * deltaTime;
                        }
                    }
                    else
                    {
                        rb.SetVelocity(Vector3.Zero);
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
                if (isAttacking)
                {
                    anim.SetRandomAttackAnimation();
                    isAttacking = false;
                }
                if (isDead)
                {
                    anim.SetDeathAnimation();
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
        }
    }

    public override void Attack()
    {
        isAttacking = true;
        Engineson.print("Melee attack executed!");
        pc.playerData.TakeDamage(damage);
        //GameObject.Find("Player").GetComponent<PlayerData>().TakeDamage(damage);
        Engineson.print("Current health: " + (pc.playerData.GetHealth()));
    }

    public void Leap()
    {
        Engineson.print("LEAP JUMP EXECUTED");
        rb.SetVelocity(rb.GetVelocity() * 2);
        //GameObject.Find("Player").GetComponent<PlayerData>().TakeDamage(damage);
        //Engineson.print("Current health: " + (GameObject.Find("Player").GetComponent<PlayerData>().GetHealth()));
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

    override public void OnCollisionEnter(GameObject other)
    {
        if (other.tag == "BoltgunProjectile")
        {
            currentHealth -= 20.0f;
            Engineson.print("Boltgun hit!");
        } 
        else if (other.tag == "ShotgunProjectile")
        {
            //cosas de la shotgun
        } 
        else if (other.tag == "RailgunProjectile")
        {
            //Cosas de railgun
        }
        if (currentHealth <= 0)
        {
            Engineson.print("This man is dead man.");
            //Destroy(gameObject);
            isDead = true;
        }
        //Engineson.print("Player hit!");
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
