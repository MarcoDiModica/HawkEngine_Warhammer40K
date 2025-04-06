using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Net;
using System.Numerics;
using HawkEngine;

public class EnemyControllerBoss : EnemyController
{
    private float hurtboxDuration = 0.5f; 
    private Vector3 hurtboxSize = new Vector3(10.0f, 1.0f, 10.0f); 
    private Vector3 hurtboxOffset = new Vector3(3.0f, 0.0f, 0.0f); 
    private GameObject hurtboxObject;

    //audio
    private Audio music;
    private string combatMusic = "Assets/Audio/PlaceHolder_CombatMusic.wav";
   
    //stats

    bool isCombatMusicPlaying = false;
    private float health = 1500.0f;
    private float damage = 25.0f;

    // unburrowing attack stats
    private float unburrowingAttackCooldown = 5.0f;
    private float postUnburrowingAttackDelay = 1.5f;
    private float restAfterThirdAttack = 4.0f;
    private float timer = 0.0f;
    private int attackCount = 0;
    private bool isBuried = true;

    // phase 2 unburrowing/slam stats
    private float unburrowingAttackCooldownPhase2 = 5.0f;
    private float postAttackDelay = 2.0f;
    private float burrowTime = 2.0f;
    private bool isPreparingAttack = false;
    private Vector3[] fixedPositions = new Vector3[]
    {
        new Vector3(10,-21.807f,1087),
        new Vector3(-10,-21.807f,1087),
        new Vector3(10,-21.807f,1077),
        new Vector3(-10,-21.807f,1077)
    };
    private float slamAttackDistance = 20.0f;
    private float slamAttackCooldown = 2.0f;
    private float slamAttackTimer = 0.0f;
    private bool isSlamActive = false;

    private enum BossPhase
    {
        PHASE1,
        PHASE2,
        PHASE3
    }

    private BossPhase currentPhase;

    public override void Awake()
    {
        music = gameObject.GetComponent<Audio>();
    }

    public override void Start()
    {
        playerTransform = GameObject.Find("Player").GetComponent<Transform>();
        rb = gameObject.GetComponent<Rigidbody>();
        rb.SetMass(1000.0f);
        currentHealth = maxHealth;
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
            Engineson.print("PlayerShooting: Audio component not found");
        }
        enemyTransform = gameObject.GetComponent<Transform>();
        if (enemyTransform == null)
        {
            Engineson.print("ERROR: PlayerMovement requires a Transform component!");
            return;
        }
        currentHealth = 150.0f;
        gameObject.tag = "Boss";
        isDead = false;
    }

    public override void Update(float deltaTime)
    {
        if (!isDead)
        {
            float distanceToPlayer = Vector3.Distance(enemyTransform.position, playerTransform.position);

            if (playerTransform != null)
            {
                Vector3 directionToPlayer = Vector3.Normalize(playerTransform.position - enemyTransform.position);
                float targetAngle = (float)Math.Atan2(directionToPlayer.X, directionToPlayer.Z) * (180.0f / (float)Math.PI);
                Quaternion newRotation = Quaternion.CreateFromYawPitchRoll(targetAngle * ((float)Math.PI / 180.0f), 0, 0);
                enemyTransform.SetRotationQuat(newRotation);
                collider.SetRotation(newRotation);
            }

            if (currentHealth < 50)
            {
                currentPhase = BossPhase.PHASE3;
            }
            else if (currentHealth < 100)
            {
                currentPhase = BossPhase.PHASE2;
            }
            switch (currentPhase)
            {
                case BossPhase.PHASE1:

                    if (distanceToPlayer <= 300.0f)
                    {
                        if (isCombatMusicPlaying == false)
                        {
                            sound?.LoadAudio(combatMusic);
                            sound?.Play(true);
                            isCombatMusicPlaying = true;
                        }

                        timer += deltaTime;

                        if (isBuried && timer >= unburrowingAttackCooldown)
                        {
                            UnburrowingAttack();
                            timer = 0.0f;
                        }
                        else if (!isBuried && timer >= postUnburrowingAttackDelay)
                        {
                            if (attackCount % 3 == 0 && attackCount > 0)
                            {
                                if (timer >= restAfterThirdAttack)
                                {
                                    Burrow();
                                    timer = 0.0f;
                                }
                            }
                            else
                            {
                                Burrow();
                                timer = 0.0f;
                            }
                        }
                    }

                    break;
                case BossPhase.PHASE2:

                    timer += deltaTime;

                    if (isBuried && timer >= unburrowingAttackCooldown)
                    {
                        isPreparingAttack = true;
                        timer = 0.0f;
                    }
                    else if (isPreparingAttack && timer >= burrowTime)
                    {
                        UnburrowingAttackPhase2();
                        isPreparingAttack = false;
                        timer = 0.0f;
                    }
                    else if (!isBuried && timer >= postAttackDelay)
                    {
                        if (playerTransform != null)
                        {

                            if (distanceToPlayer <= slamAttackDistance && slamAttackTimer <= 0.0f)
                            {
                                SlamAttack();
                                slamAttackTimer = slamAttackCooldown;
                            }
                            else
                            {
                                ChangePositionToClosest();
                            }
                        }
                        timer = 0.0f;
                    }

                    if (slamAttackTimer > 0.0f)
                    {
                        slamAttackTimer -= deltaTime;
                    }

                    break;
                case BossPhase.PHASE3:

                    // The same as phase 2 for the moment
                    
                    timer += deltaTime;

                    if (isBuried && timer >= unburrowingAttackCooldown)
                    {
                        isPreparingAttack = true;
                        timer = 0.0f;
                    }
                    else if (isPreparingAttack && timer >= burrowTime)
                    {
                        UnburrowingAttackPhase2();
                        isPreparingAttack = false;
                        timer = 0.0f;
                    }
                    else if (!isBuried && timer >= postAttackDelay)
                    {
                        if (playerTransform != null)
                        {

                            if (distanceToPlayer <= slamAttackDistance && slamAttackTimer <= 0.0f)
                            {
                                SlamAttack();
                                slamAttackTimer = slamAttackCooldown;
                            }
                            else
                            {
                                ChangePositionToClosest();
                            }
                        }
                        timer = 0.0f;
                    }

                    if (slamAttackTimer > 0.0f)
                    {
                        slamAttackTimer -= deltaTime;
                    }
                    break;
        }

        if (hurtboxObject != null)
        {
            hurtboxDuration += deltaTime;
            if (hurtboxDuration >= 0.5)
            {
                DestroyHurtbox();
                hurtboxDuration = 0.0f;
            }
        }

        }

        if (isDead)
        {
            collider.SetActive(false);
            if (isCombatMusicPlaying == true)
            {
                sound?.Stop();
                isCombatMusicPlaying = false;
            }
        }
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
        //Engineson.print("Player hit!");
    }

    public override void Attack()
    {

    }

    public override void TakeDamage(float damage)
    {
        currentHealth -= damage;

        if (currentHealth <= 0.0f)
        {
            Die();
        }
        //anim.SetHitAnimation();
        //sound.LoadAudio("Assets/Audio/SFX/Enemies/Hormagaunt/HormagauntHit_ready.wav");
        //sound?.Play();
    }
    private void UnburrowingAttack()
    {
        if (isDead == false)
        {
            if (playerTransform != null)
            {
                Engineson.print("Unburrowing Attack");
                enemyTransform.position = playerTransform.position;
                collider.SetPosition(playerTransform.position);
            }
            attackCount++;
            isBuried = false;
        }
    }

    private void UnburrowingAttackPhase2()
    {
        if (isDead == false)
        {
            if (playerTransform != null)
            {
                enemyTransform.position = fixedPositions[FindClosestFixedPosition()];
                collider.SetPosition(enemyTransform.position);
                Engineson.print("Unburrowing Attack ");
            }
            isBuried = false;
        }
    }

    public void ChangePositionToClosest()
    {
        if (isDead == false)
        {
            enemyTransform.position = fixedPositions[FindClosestFixedPosition()];
            collider.SetPosition(enemyTransform.position);
            Burrow();
        }
    }

    private int FindClosestFixedPosition()
    {
        int closestIndex = 0;
        float closestDistance = float.MaxValue;

        if (isDead == false)
        {
            for (int i = 0; i < fixedPositions.Length; i++)
            {
                float distance = Vector3.Distance(playerTransform.position, fixedPositions[i]);
                if (distance < closestDistance)
                {
                    closestDistance = distance;
                    closestIndex = i;
                }
            }
        }

        return closestIndex;
    }

    private void SlamAttack()
    {
        if (isDead == false)
        {
            CreateHurtbox();
            hurtboxDuration = 0.0f;
            isSlamActive = true;
            slamAttackTimer = 0.0f;
        }
    }

    private void Burrow()
    {
        if (isDead == false)
        {
            Engineson.print("Burrowed");
            enemyTransform.position = new Vector3(0.0f, -40.0f, 0.0f);
            collider.SetPosition(enemyTransform.position);
            isBuried = true;
        }
    }

    private void Die()
    {
        enemyTransform.position = new Vector3(0.0f, -40.0f, 0.0f);
        collider.SetPosition(enemyTransform.position);
        isDead = true;
        // Triggerear WinScreen
    }

    private void CreateHurtbox()
    {
        hurtboxObject = Engineson.CreateGameObject("SlamHurtbox", null);
        hurtboxObject.AddComponent<MeshRenderer>();

        var hurtboxTransform = hurtboxObject.GetComponent<Transform>();
        hurtboxTransform.position = enemyTransform.position + (enemyTransform.forward * hurtboxOffset.X);
        hurtboxTransform.SetScale(hurtboxSize.X, hurtboxSize.Y, hurtboxSize.Z);

        hurtboxObject.AddComponent<BoxCollider>();
        hurtboxObject.GetComponent<BoxCollider>().SetTrigger(true);
        hurtboxObject.tag = "EnemyAttack";
    }

    private void DestroyHurtbox()
    {
        if (hurtboxObject != null)
        {
            Engineson.Destroy(hurtboxObject);
            hurtboxObject = null;
        }
    }
}