using System;
using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public class EnemyControllerStalker : EnemyController
{
    // Hurtbox
    private float hurtboxActivationTime = 1.5f; // Tiempo que el jugador debe estar en la hurtbox para activarla
    private float hurtboxTimer = 0f;
    private Vector3 hurtboxSize = new Vector3(3.0f, 2.0f, 3.0f); // Tamaño de la hurtbox
    private Vector3 hurtboxOffset = new Vector3(4.0f, 0.0f, 0.0f); // Desplazamiento de la hurtbox hacia adelante
    private GameObject hurtboxObject;

    // Perfect Dodge
    private bool dodgewindow = false;
    private float dodgeActivationTime = 0.5f;
    private float dodgeTimer = 0f;

    // Auxs
    private HormagauntAnimation anim;
    PlayerController pc;

    // Audio
    bool isCombatMusicPlaying = false;
    private Audio music;
    private string combatMusic = "Assets/Audio/PlaceHolder_CombatMusic.wav";

    // Enemy Stats
    private float health = 350.0f;
    private float clawDamage = 25.0f;
    private float pounceDamage = 35.0f;

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
            Engineson.print("ERROR: Player not found!");
        }

        collider = gameObject.GetComponent<BoxCollider>();
        if (collider == null)
        { 
            Engineson.print("ERROR: Lictor Collider not found!"); 
            return; 
        }

        sound = gameObject.GetComponent<Audio>();
        if (sound == null)
        {
            Engineson.print("ERROR: Lictor Sound not found!");
        }

        enemyTransform = gameObject.transform;
        if (enemyTransform == null)
        {
            Engineson.print("ERROR: Lictor transform not found!");
            return;
        }

        //anim = GameObject.Find("LictorMesh").GetComponent<LictorAnimation>();
        //if (anim == null)
        //{
        //    Engineson.print("ERROR: LictorAnimation requires SkeletalANimation component");
        //    return;
        //}

        particles = gameObject.GetComponent<ParticleFX>();
        //particles.ApplyPreset(9);

        pc = GameObject.Find("Player").GetComponent<PlayerController>();
        maxHealth = health;
        currentHealth = maxHealth;
        gameObject.tag = "Stalker";
    }

    public override void Update(float deltaTime)
    {
        if (currentState != EnemyState.DEAD)
        {
            if (currentHealth <= 0)
            {
                Engineson.print("Lictor is dead");
                currentState = EnemyState.DEAD;
                return;
            }

            if (currentState != EnemyState.STUNNED)
            {
                Vector3 playerPos = playerTransform.position;
                float distanceToPlayer = Vector3.Distance(enemyTransform.position, playerPos);

                if (distanceToPlayer < distToChase)
                {
                    currentState = EnemyState.CHASE;
                    Engineson.print("Lictor is chasing");

                    Vector3 currentVelocity = rb.GetVelocity();
                    moveDirection = Vector3.Normalize(playerPos - gameObject.GetComponent<Transform>().position);
                    Vector3 desiredVelocity = moveDirection * speedMovement;

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
                    currentState = EnemyState.IDLE;
                    Engineson.print("Lictor is idle");
                }
            }
            else
            {
                Engineson.print("Lictor is stunned");
            }
        }
        else
        {
            Engineson.print("Lictor is dead");
            collider.SetActive(false);
        }

        switch (currentState)
        {
            case EnemyState.IDLE:
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

                break;

            case EnemyState.ATTACK:
                break;

            case EnemyState.STUNNED:
                break;

            case EnemyState.DEAD:
                //anim.SetDeathAnimation();
                sound.LoadAudio("Assets/Audio/SFX/Enemies/Hormagaunt/HormagauntDeath_ready.wav");
                sound?.Play();
                break;

            default:
                break;
        }
    }

    public override void Attack()
    {
        throw new NotImplementedException();
    }

    public override void TakeDamage(float damage)
    {
        throw new NotImplementedException();
    }
}

