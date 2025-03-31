using System;
using System.Diagnostics;
using System.Numerics;
using HawkEngine;

public class EnemyControllerMelee : EnemyController
{
    private float hurtboxActivationTime = 1.5f; // Tiempo que el jugador debe estar en la hurtbox para activarla
    private float hurtboxTimer = 0f;
    private Vector3 hurtboxSize = new Vector3(3.0f, 2.0f, 3.0f); // Tama�o de la hurtbox
    private Vector3 hurtboxOffset = new Vector3(5.0f, 2.0f, 0.0f); // Desplazamiento de la hurtbox hacia adelante
    private GameObject hurtboxObject;
    private bool dodgewindow = false;
    private float dodgeActivationTime = 0.5f;
    private float dodgeTimer = 0f;

    //stats
    private float health = 100.0f;
    private float damage = 20.0f;
    
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

        maxHealth = 100.0f;
        currentHealth = maxHealth;
    }

    public override void Update(float deltaTime)
    {
        if (!isStunned)
        {
            Vector3 playerPos = playerTransform.position;

            if (Vector3.Distance(enemyTransform.position, playerPos) < distToChase)
            {
                if (IsPlayerInHurtbox(playerPos))
                {
                    hurtboxTimer += deltaTime;
                    if (hurtboxTimer >= hurtboxActivationTime)
                    {
                        CreateHurtbox();
                        hurtboxTimer = 0f;
                        dodgewindow = true;
                    }
                    else if (hurtboxTimer >= 0.5f && dodgewindow)
                    {
                        Attack();
                        soundAttack?.Play();
                        DestroyHurtbox();
                        hurtboxTimer = 0f;
                        dodgeTimer = 0f;
                        dodgewindow = false;
                    }
                }
                else
                {
                    hurtboxTimer = 0f;
                }
                if (dodgewindow)
                {
                    dodgeTimer += deltaTime;
                    if (dodgeTimer >= dodgeActivationTime)
                    {
                        DestroyHurtbox();
                        dodgeTimer = 0f;
                        dodgewindow = false;
                    }
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
                }
            }
            else
            {
                Engineson.print("Player not in hurtbox");
                hurtboxTimer = 0f;
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

    public override void Attack()
    {
        Engineson.print("Melee attack executed!");
    }

    private bool IsPlayerInHurtbox(Vector3 playerPos)
    {
        Vector3 hurtboxCenter = enemyTransform.position + enemyTransform.forward * hurtboxOffset;
        Vector3 halfSize = hurtboxSize * 0.5f;
        //Engineson.print("Player in hurtbox");

        return (playerPos.X >= hurtboxCenter.X - halfSize.X && playerPos.X <= hurtboxCenter.X + halfSize.X) &&
               (playerPos.Y >= hurtboxCenter.Y - halfSize.Y && playerPos.Y <= hurtboxCenter.Y + halfSize.Y) &&
               (playerPos.Z >= hurtboxCenter.Z - halfSize.Z && playerPos.Z <= hurtboxCenter.Z + halfSize.Z);
    }

    override public void OnCollisionEnter(Collider other)
    {
        Engineson.print("Player hit!");
    }

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
