using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Net;
using System.Numerics;
using System.Runtime.CompilerServices;
using HawkEngine;

public class EnemyControllerBoss : EnemyController
{
    private float hurtboxDuration = 0.5f; 
    private Vector3 slamHurtboxSize = new Vector3(3.0f, 1.0f, 10.0f); 
    private GameObject slamHurtboxObject;

    private GameObject clawHurtboxObject;

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
    //private Vector3[] fixedPositions = new Vector3[]
    //{
    //    new Vector3(10,-21.807f,1020),
    //    new Vector3(-10,-21.807f,1020),
    //    new Vector3(10,-21.807f,1000),
    //    new Vector3(-10,-21.807f,1000)
    //};

    private Vector3[] fixedPositions = new Vector3[]
    {
        new Vector3(10, 0, -10),
        new Vector3(-10, 0, -10),
        new Vector3(10, 0, 10),
        new Vector3(-10, 0, 10)
    };
    private float slamAttackDistance = 20.0f;
    private float slamAttackCooldown = 2.0f;
    private float slamAttackTimer = 0.0f;
    private bool isSlamActive = false;

    // Metal Slide stats
    private float metalSlideDuration = 7.0f;
    private float metalSlideTimer = 0.0f;
    private bool isMetalSlideActive = false;
    private float metalSlideStartTime = 0.0f;
    private List<(float impactTime, Vector3 position)> pendingImpacts = new List<(float, Vector3)>();
    private float maxFragmentOffset = 3.0f;
    private int numberOfFragments;
    private Random random;
    private Vector3 playerVelocity;
    private float bossTime = 0.0f;

    private enum BossPhase
    {
        PHASE1,
        PHASE2,
        PHASE3
    }

    private BossPhase currentPhase;

    private class FragmentImpact
    {
        public Vector3 position;
        public float impactTime;
    }

    public override void Awake()
    {
        music = gameObject.GetComponent<Audio>();
    }

    public override void Start()
    {
        playerTransform = GameObject.Find("Player").GetComponent<Transform>();
        playerVelocity = GameObject.Find("Player").GetComponent<Rigidbody>().GetVelocity();
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
        currentHealth = 399.0f;
        gameObject.tag = "Boss";
        isDead = false;
    }

    public override void Update(float deltaTime)
    {
        if (!isDead)
        {
            bossTime += deltaTime;

            float distanceToPlayer = Vector3.Distance(enemyTransform.position, playerTransform.position);

            if (playerTransform != null)
            {
                Vector3 directionToPlayer = Vector3.Normalize(playerTransform.position - enemyTransform.position);
                float targetAngle = (float)Math.Atan2(directionToPlayer.X, directionToPlayer.Z) * (180.0f / (float)Math.PI);
                Quaternion newRotation = Quaternion.CreateFromYawPitchRoll(targetAngle * ((float)Math.PI / 180.0f), 0, 0);
                enemyTransform.SetRotationQuat(newRotation);
                collider.SetRotation(newRotation);
            }

            if (currentHealth < 200)
            {
                currentPhase = BossPhase.PHASE3;
            }
            else if (currentHealth < 400)
            {
                currentPhase = BossPhase.PHASE2;
            }
            switch (currentPhase)
            {
                case BossPhase.PHASE1:

                    if (distanceToPlayer <= 200.0f)
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
                            if (slamAttackTimer <= 0.0f)
                            {
                                if (distanceToPlayer >= 5.0f && distanceToPlayer <= 10.0f)
                                {
                                    ClawStrike();
                                }
                                else if (distanceToPlayer <= slamAttackDistance)
                                {
                                    SlamAttack();
                                }
                                slamAttackTimer = slamAttackCooldown;
                            }
                            //if (distanceToPlayer <= slamAttackDistance && slamAttackTimer <= 0.0f)
                            //{
                            //    ClawStrike();
                            //    slamAttackTimer = slamAttackCooldown;
                            //}
                            //else
                            //{
                            //    ChangePositionToClosest();
                            //}
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
                                ClawStrike();
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

            //if (isMetalSlideActive)
            //{
            //    float currentTime = bossTime;
            //    for (int i = pendingImpacts.Count - 1; i>= 0; i--)
            //    {
            //        if (currentTime >= pendingImpacts[i].impactTime)
            //        {
            //            CreateSlideHurtbox(pendingImpacts[i].position);
            //            pendingImpacts.RemoveAt(i);
            //        }
            //    }

            //    if (pendingImpacts.Count == 0)
            //    {
            //        isMetalSlideActive = false;
            //    }

            //}

            if (slamHurtboxObject != null || clawHurtboxObject != null)
            {
                hurtboxDuration += deltaTime;
                if (hurtboxDuration >= 0.5)
                {
                    DestroyHurtboxes();
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
                Engineson.print("Unburrowing Attack");
            }
            isBuried = false;
        }
    }

    private void SlamAttack()
    {
        if (isDead == false)
        {
            CreateSlamHurtbox();
            hurtboxDuration = 0.0f;
            isSlamActive = true;
            slamAttackTimer = 0.0f;
        }
    }

    private void ClawStrike()
    {
        if (isDead == false)
        {
            CreateClawHurtbox();
            
        }
    }

    private void MetalSlide()
    {
        if (playerTransform == null) return;

        if (isDead == false)
        {
            numberOfFragments = random.Next(5, 9);
            pendingImpacts.Clear();
            metalSlideStartTime = bossTime;
            isMetalSlideActive = true;

            Vector3 playerPosition = playerTransform.position;

            for (int i = 0; i<numberOfFragments; i++)
            {
                float baseTime = i * metalSlideDuration / numberOfFragments;
                float randomOffset = ((float)random.NextDouble() - 0.5f) * (metalSlideDuration / numberOfFragments);
                float ti = metalSlideStartTime + baseTime + randomOffset;

                Vector3 predictedPosition = playerPosition + playerVelocity * (ti - bossTime);

                float offsetX = ((float)random.NextDouble() * 2 - 1) * maxFragmentOffset;
                float offsetZ = ((float)random.NextDouble() * 2 - 1) * maxFragmentOffset;

                Vector3 impactPosition = predictedPosition + new Vector3(offsetX, 0, offsetZ);
                pendingImpacts.Add((ti, impactPosition));

                Engineson.print($"Fragment {i} will impact at {impactPosition} at time {ti}");
            }
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
        enemyTransform.position = new Vector3(0.0f, -40.0f, 1080.0f);
        collider.SetPosition(enemyTransform.position);
        isDead = true;
        SceneManager.LoadScene("WinScene");
    }

    private void CreateSlamHurtbox()
    {
        if (enemyTransform == null) return;

        slamHurtboxObject = Engineson.CreateGameObject("SlamHurtbox", null);
        slamHurtboxObject.AddComponent<MeshRenderer>();
        slamHurtboxObject.AddComponent<BoxCollider>();
        slamHurtboxObject.GetComponent<BoxCollider>().SetTrigger(true);
        slamHurtboxObject.tag = "EnemyAttack";

        Vector3 forward = enemyTransform.forward;
        Vector3 bossPosition = enemyTransform.position;

        float halfLength = slamHurtboxSize.Z / 2.0f;
        float offset = 5.0f;

        Vector3 hurtboxPosition = bossPosition + forward * (halfLength + offset);

        var hurtboxTransform = slamHurtboxObject.GetComponent<Transform>();
        hurtboxTransform.position = hurtboxPosition;
        hurtboxTransform.SetScale(slamHurtboxSize.X, slamHurtboxSize.Y, slamHurtboxSize.Z);

        float angle = (float)Math.Atan2(forward.X, forward.Z);
        Quaternion rotation = Quaternion.CreateFromAxisAngle(Vector3.UnitY, angle);
        hurtboxTransform.SetRotationQuat(rotation);
    }

    private void CreateClawHurtbox()
    {
        if (playerTransform == null) return;

        int segments = 4;
        float baseWidth = 2.0f;
        float segmentLength = 1.5f;
        float height = 1.0f;
        float spacing = 1.0f;

        Vector3 forward = Vector3.Normalize(enemyTransform.forward);
        Vector3 origin = enemyTransform.position;

        for (int i = 1; i <= segments; i++)
        {
            float width = baseWidth + (i * 0.5f);
            float length = segmentLength;
            Vector3 size = new Vector3(width, height, length);

            Vector3 offset = forward * ((length + spacing) * i);
            Vector3 position = origin + offset;

            clawHurtboxObject = Engineson.CreateGameObject("ClawHurtbox", null);
            clawHurtboxObject.AddComponent<MeshRenderer>();
            clawHurtboxObject.AddComponent<BoxCollider>();
            clawHurtboxObject.GetComponent<BoxCollider>().SetTrigger(true);
            clawHurtboxObject.tag = "EnemyAttack";

            var hurtboxTransform = clawHurtboxObject.GetComponent<Transform>();
            hurtboxTransform.position = position;
            hurtboxTransform.SetScale(size.X, size.Y, size.Z);

            float angle = (float)Math.Atan2(forward.X, forward.Z);
            Quaternion rotation = Quaternion.CreateFromAxisAngle(Vector3.UnitY, angle);
            hurtboxTransform.SetRotationQuat(rotation);
        }

    }

    private void CreateSlideHurtbox(Vector3 position)
    {
        var fragment = Engineson.CreateGameObject("FragmentImpact", null);
        fragment.AddComponent<MeshRenderer>();
        fragment.AddComponent<BoxCollider>();
        fragment.GetComponent<BoxCollider>().SetTrigger(true);
        fragment.tag = "EnemyAttack";

        var fragmentTransform = fragment.GetComponent<Transform>();
        fragmentTransform.position = position;
        fragmentTransform.SetScale(4.0f, 1.0f, 4.0f);

    }


    private void DestroyHurtboxes()
    {
        if (slamHurtboxObject != null)
        {
            Engineson.Destroy(slamHurtboxObject);
            slamHurtboxObject = null;
        }

        if (clawHurtboxObject != null)
        {
            Engineson.Destroy(clawHurtboxObject);
            clawHurtboxObject = null;
        }
    }
}