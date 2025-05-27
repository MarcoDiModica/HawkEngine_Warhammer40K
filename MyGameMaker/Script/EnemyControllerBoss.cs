using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.ComponentModel.Design;
using System.Diagnostics;
using System.Net;
using System.Numerics;
using System.Runtime.CompilerServices;
using System.Threading.Tasks;
using HawkEngine;

public class EnemyControllerBoss : EnemyController
{
    private float hurtboxDuration = 0.5f; 
    private Vector3 slamHurtboxSize = new Vector3(3.0f, 10.0f, 15.0f);
    private Vector3 hurtboxOffset = new Vector3(4.0f, 0.0f, 0.0f);
    private GameObject slamHurtboxObject;

    private List<GameObject> clawHurtboxObjects = new List<GameObject>();

    private Random rng = new Random();

    //audio
    //     private AudioSource music;
    //     private string combatMusic = "Assets/Audio/PlaceHolder_CombatMusic.wav";
    //     private AudioClip musicClip;

    private List<string> roarClips = new List<string>
    {
        "Assets/Audio/Mawloc_Growl_1.wav",
        "Assets/Audio/Mawloc_Growl_2.wav",
        "Assets/Audio/Mawloc_Growl_3.wav",

    };

    private const string BurrowClip = "Assets/Audio/Mawloc_Underground_move.wav";
    private const string UnburrowClip = "Assets/Audio/Mawloc_Underground_Attack.wav";
    private const string SlamClip = "Assets/Audio/Mawloc_Slam_Atack.wav";
    private const string ClawClip = "Assets/Audio/Mawloc_Claw_Attack.wav";
    private const string AcidClip = "Assets/Audio/Mawloc_Acid_Attack.wav";
    private const string DeathClip = "Assets/Audio/Mawloc_Death.wav";
    private const string BossTheme = "Assets/Audio/Music/Level2_BossFight_BossTheme_Gold.ogg";
    private const string BossThemePhase2 = "Assets/Audio/Music/Level2_MainTheme_BossFight_Gold.ogg";

    private bool isBossMusicPlaying = false;
    //stats
    bool isCombatMusicPlaying = false;
    private float health = 1500.0f;
    private float contactDamage = 10.0f;

    // unburrowing attack stats
    private float unburrowingAttackCooldown = 5.0f;
    private float postUnburrowingAttackDelay = 1.5f;
    private float restAfterThirdAttack = 4.0f;
    private float timer = 0.0f;
    private int attackCount = 0;
    public bool isBuried = true; 

    // phase 2 unburrowing/slam stats
    private float unburrowingAttackCooldownPhase2 = 5.0f;
    private float postAttackDelay = 2.0f;
    private float burrowTime = 2.0f;
    private bool isPreparingAttack = false;
    private float slamDamage = 10.0f;
    private float strikeDamage = 15.0f;
    //private Vector3[] fixedPositions = new Vector3[]
    //{
    //    new Vector3(10,-21.807f,1020),
    //    new Vector3(-10,-21.807f,1020),
    //    new Vector3(10,-21.807f,1000),
    //    new Vector3(-10,-21.807f,1000)
    //};

    private Vector3[] fixedPositions = new Vector3[]
    {
        new Vector3(-108.0f, 10.0f, -32.0f),
        new Vector3(-108.0f, 10.0f, 29.0f),
        new Vector3(-76.0f, 10.0f, -2.0f),
        new Vector3(-41.0f, 10.0f, -32.0f),
        new Vector3(-41.0f, 10.0f, 29.0f)
    };
    private float slamAttackDistance = 25.0f;
    private float slamAttackCooldown = 2.0f;
    private float slamAttackTimer = 0.0f;
    private bool isSlamActive = false;

    // Metal Slide stats
    private GameObject metalSlideObject;
    private Vector3 metalSlideStartOffset = new Vector3(0, 15.0f, 0);
    private float metalSlideSpeed = 30.0f;
    private bool phase3Started = false;
    private bool hasTeleportedToCenter = false;
    private bool isPhase3Attacking = false;
    private float metalSlideDamage = 8.0f;
    private bool metalSlideDamageApplied = false;

    private EnemyControllerBossTail tailController;
    private bool hasUnburiedInPhase2 = false;
    private bool clawDamageAppliedThisFrame = false;

    private MawlocAnimation anim;
    private PlayerController pc;
    private float deathTimer = 0f;
    private float deathCoolodown = 5f;

    private enum BossPhase
    {
        PHASE1,
        PHASE2,
        PHASE3
    }

    private BossPhase currentPhase;

    public override void Awake()
    {
        //music = gameObject.GetComponent<AudioSource>();
    }

    public override void Start()
    {
        playerTransform = GameObject.Find("Player").GetComponent<Transform>();
        rb = gameObject.GetComponent<Rigidbody>();
        pc = GameObject.Find("Player").GetComponent<PlayerController>();
        rb.SetMass(100000.0f);
        tailController = GameObject.Find("MawlocTail").GetComponent<EnemyControllerBossTail>();
        tailController?.gameObject.SetActive(false);
        currentHealth = 1500.0f;
        if (playerTransform == null)
        {
            Engineson.print("ERROR: Player couldn't be found!");
        }
        collider = gameObject.GetComponent<BoxCollider>();
        collider.SetSize(new Vector3(3.0f, 2.0f, 3.0f));
        if (collider == null)
        {
            Engineson.print("ERROR: PlayerMovement requires a Collider component!");
            return;
        }
        //         sound = gameObject.GetComponent<AudioSource>();
        //         if (sound == null)
        //         {
        //             Engineson.print("PlayerShooting: Audio component not found");
        //         }

        anim = gameObject.GetChild("MawlocMesh").GetComponent<MawlocAnimation>();
        if (anim == null)
        {
            Engineson.print("ERROR: Mawloc animation requires a script component!");
            return;
        }

        enemyTransform = gameObject.GetComponent<Transform>();
        if (enemyTransform == null)
        {
            Engineson.print("ERROR: PlayerMovement requires a Transform component!");
            return;
        }
        currentHealth = 1500.0f;
        gameObject.tag = "Boss";
        isDead = false;
//         musicClip = new AudioClip(combatMusic, "BossMusic", true, false);
//         sound.LoadAudioClip(musicClip);

    }

    public override void Update(float deltaTime)
    {
        if (!isDead)
        {
            if (!isBossMusicPlaying)
            {
                Audio.Play(BossTheme, true);
                isBossMusicPlaying = true;
            }

            float distanceToPlayer = Vector3.Distance(enemyTransform.position, playerTransform.position);
            clawDamageAppliedThisFrame = false;

                if (playerTransform != null)
                {
                    Vector3 directionToPlayer = Vector3.Normalize(playerTransform.position - enemyTransform.position);
                    float targetAngle = (float)Math.Atan2(directionToPlayer.X, directionToPlayer.Z) * (180.0f / (float)Math.PI);
                    Quaternion newRotation = Quaternion.CreateFromYawPitchRoll(targetAngle * ((float)Math.PI / 180.0f), 0, 0);
                    enemyTransform.SetRotationQuat(newRotation);
                    collider.SetRotation(newRotation);
                }

                if (currentHealth < 500)
                {
                    currentPhase = BossPhase.PHASE3;
                }
                else if (currentHealth < 1000)
                {
                    currentPhase = BossPhase.PHASE2;
                    Audio.Stop(BossTheme);
                    Audio.Play(BossThemePhase2, true);
            }

                switch (currentPhase)
                {
                    case BossPhase.PHASE1:
                        if (distanceToPlayer <= 1000.0f)
                        {
                            if (!isCombatMusicPlaying)
                            {
                                //sound?.LoadAudio(combatMusic);
                                //sound?.Play(true);
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
                                if (distanceToPlayer <= 15.0f && slamAttackTimer <= 0.0f)
                                {
                                    ClawStrike();
                                    slamAttackTimer = slamAttackCooldown;
                                }
                                else if (distanceToPlayer <= slamAttackDistance && slamAttackTimer <= 0.0f)
                                {
                                    SlamAttack();
                                    slamAttackTimer = slamAttackCooldown;
                                }
                                else if (distanceToPlayer > slamAttackDistance && distanceToPlayer < 35.0f && metalSlideObject == null)
                                {
                                    MetalSlide();
                                    slamAttackTimer = slamAttackCooldown;
                                }
                                else if (distanceToPlayer >= 35.0f)
                                {
                                    Engineson.print("ChangePositionToClosest");
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
                        timer += deltaTime;
                        
                        if (!phase3Started)
                        {
                            Burrow();
                            phase3Started = true;
                            timer = 0.0f;
                        }
                        else if (phase3Started && !hasTeleportedToCenter && timer >= burrowTime)
                        {
                            UnburrowAtCenter();
                            hasTeleportedToCenter = true;
                            isBuried = false;
                            timer = 0.0f;
                            tailController.Activate();
                        }
                        else if (hasTeleportedToCenter && !isBuried && timer >= postAttackDelay)
                        {
                            if (playerTransform != null)
                            {
                                if (distanceToPlayer <= 15.0f && slamAttackTimer <= 0.0f)
                                {
                                    ClawStrike();
                                    slamAttackTimer = slamAttackCooldown;
                                }
                                else if (distanceToPlayer <= slamAttackDistance && slamAttackTimer <= 0.0f)
                                {
                                    SlamAttack();
                                    slamAttackTimer = slamAttackCooldown;
                                }
                                else if (distanceToPlayer > slamAttackDistance && metalSlideObject == null)
                                {
                                    MetalSlide();
                                    slamAttackTimer = slamAttackCooldown;
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

                if (slamHurtboxObject != null || clawHurtboxObjects != null)
                {
                    hurtboxDuration += deltaTime;
                    if (hurtboxDuration >= 0.5f)
                    {
                        DestroyHurtboxes();
                        hurtboxDuration = 0.0f;
                    }
                }

                UpdateMetalSlide(deltaTime);
                CheckBossHurtboxes();
            }
        if (isDead)
        {
            collider.SetActive(false);
            Audio.Stop(BossTheme); 
            if (anim.isAnimFinished)
            {
                deathTimer += deltaTime;
                if (deathTimer >= deathCoolodown)
                {
                    Audio.Stop(BossTheme);
                    Engineson.Destroy(gameObject);
                    SceneManager.LoadSceneWithFade("WinScene", 0.5f);
                }
            }
        }
    }

    public override void ResetEnemyCheckPoint()
    {
        
    }
    override public void OnCollisionEnter(GameObject other)
    {
        if (other.tag == "Player")
        {
            pc.playerData.TakeDamage(contactDamage);
            pc.StartFlashColor(pc.flashColor, pc.flashDuration);
        }
    }

    public override void Attack()
    {

    }

    private void CheckBossHurtboxes()
    {
        if (pc == null || pc.playerData == null) return;

        Vector3 playerPos = playerTransform.position;

        if (slamHurtboxObject != null && IsPlayerInCollider(slamHurtboxObject, playerPos))
        {
            ApplyBossDamage(slamDamage);
        }

        if (clawHurtboxObjects != null && !clawDamageAppliedThisFrame)
        {
            foreach (GameObject claw in clawHurtboxObjects)
            {
                if (claw != null && IsPlayerInCollider(claw, playerPos))
                {
                    ApplyBossDamage(strikeDamage);
                    clawDamageAppliedThisFrame = true;
                    break;
                }
            }
        }

        if (metalSlideObject != null && IsPlayerInCollider(metalSlideObject, playerPos))
        { 
            if(!metalSlideDamageApplied)
            {
                ApplyBossDamage(metalSlideDamage);
                metalSlideDamageApplied = true;
            }
        }
    }

    private bool IsPlayerInCollider(GameObject hurtbox, Vector3 playerPos)
    {
        var bc = hurtbox.GetComponent<BoxCollider>();
        Vector3 halfSize = bc.GetSize() * 0.5f; 
        Vector3 center = hurtbox.GetComponent<Transform>().position;

        return (playerPos.X >= center.X - halfSize.X && playerPos.X <= center.X + halfSize.X) &&
               (playerPos.Y >= center.Y - halfSize.Y && playerPos.Y <= center.Y + halfSize.Y) &&
               (playerPos.Z >= center.Z - halfSize.Z && playerPos.Z <= center.Z + halfSize.Z);
    }

    private void ApplyBossDamage(float amount)
    {
        if (pc.redThirstManager.redThirstBonus < amount)
        {
            pc.playerData.TakeDamage(amount - pc.redThirstManager.redThirstBonus);
            pc.StartFlashColor(pc.flashColor, pc.flashDuration);
        }
        else
        {
            pc.playerData.TakeDamage(0.0f);
        }
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
                anim.SetUnburrowingAnimation();
                AddComponent<ParticleFX>().ApplyPreset(25);
                GetComponent<ParticleFX>().EmitBurst(25);
            }
            Audio.PlayOneShot(UnburrowClip);
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
                Engineson.print("Unburrowing Attack Phase 2");
                anim.SetUnburrowingAnimation();
                AddComponent<ParticleFX>().ApplyPreset(25);
                GetComponent<ParticleFX>().EmitBurst(25);
            }
            Audio.PlayOneShot(UnburrowClip);
            Engineson.print("PlaySound Attack Phase 2");
            isBuried = false;
        }
    }

    private void UnburrowAtCenter()
    {
        if (isDead == false)
        {
            Audio.PlayOneShot(UnburrowClip);
            enemyTransform.position = fixedPositions[2];
            collider.SetPosition(enemyTransform.position);
            anim.SetUnburrowingAnimation();
            AddComponent<ParticleFX>().ApplyPreset(25);
            GetComponent<ParticleFX>().EmitBurst(25);
        }
        isBuried = false;
    }

    private void SlamAttack()
    {
        if (isDead == false)
        {
            Audio.PlayOneShot(SlamClip);
            CreateSlamHurtbox();
            hurtboxDuration = 0.0f;
            isSlamActive = true;
            slamAttackTimer = 0.0f;
            anim.SetSlamAnimation();
        }
    }

    private void ClawStrike()
    {
        if (isDead == false)
        {
            Audio.PlayOneShot(ClawClip);
            CreateClawHurtbox();
            slamAttackTimer = 0.0f;
            anim.SetClawStrikeAnimation();
        }
    }


    private void MetalSlide()
    {
        if (isDead == false && playerTransform != null)
        {
            if (metalSlideObject != null) return;

            Vector3 spawnPosition = playerTransform.position + metalSlideStartOffset;

            anim.SetRoarAnimation();
            metalSlideObject = Engineson.CreateGameObject("MetalSlide", null);
            //metalSlideObject.AddComponent<MeshRenderer>();
            metalSlideObject.AddComponent<BoxCollider>();
            metalSlideObject.GetComponent<BoxCollider>().SetTrigger(true);
            metalSlideObject.tag = "EnemyAttack";

            var transform = metalSlideObject.GetComponent<Transform>();
            transform.position = spawnPosition;
            transform.SetScale(3, 3, 3);
            anim.SetIdleAnimation(); //Temporary, missing metal slide animation
            metalSlideObject.AddComponent<ParticleFX>();
            metalSlideObject.GetComponent<ParticleFX>().ApplyPreset(25);
            metalSlideObject.GetComponent<ParticleFX>().EmitBurst(50);
            Audio.PlayOneShot(AcidClip);
        }
    }

    private void UpdateMetalSlide(float deltaTime)
    {
        if (metalSlideObject != null)
        {
            var transform = metalSlideObject.GetComponent<Transform>();
            Vector3 position = transform.position;
            position.Y -= metalSlideSpeed * deltaTime;

            transform.position = position;

            if (position.Y < -10.0f)
            {
                Engineson.Destroy(metalSlideObject);
                metalSlideObject = null;
                metalSlideDamageApplied = false;
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

    public void Roar()
    {
        string selectedRoar = roarClips[rng.Next(roarClips.Count)];

        if (currentPhase == BossPhase.PHASE1)
        {
            Audio.SchedulePlay(selectedRoar, 0.25f);
        }
        else if (currentPhase == BossPhase.PHASE2)
        {
            Audio.SchedulePlay(selectedRoar, 0.5f);
        }
        else
        {
            Audio.PlayOneShot(selectedRoar);
        }
    }
   
    private void Burrow()
    {
        if (isDead == false)
        {
            Engineson.print("Burrowed");
            Engineson.print("PlaySound Burrowed");
            Audio.PlayOneShot(BurrowClip);
            enemyTransform.position = new Vector3(0.0f, -40.0f, 0.0f);
            collider.SetPosition(enemyTransform.position);
            anim.SetBurrowingAnimation();
            AddComponent<ParticleFX>().ApplyPreset(25);
            GetComponent<ParticleFX>().EmitBurst(25);
            isBuried = true;
        }
    }

    private void Die()
    {
        anim.SetDeathAnimation();
        tailController.Die();
        //Engineson.Destroy(GetGameObject());
        isDead = true;
        Audio.PlayOneShot(DeathClip);
        //SceneManager.LoadScene("WinScene");
    }

    private void CreateSlamHurtbox()
    {
        if (enemyTransform == null) return;

        slamHurtboxObject = Engineson.CreateGameObject("SlamHurtbox", null);
        //slamHurtboxObject.AddComponent<MeshRenderer>();
        slamHurtboxObject.AddComponent<BoxCollider>();
        slamHurtboxObject.GetComponent<BoxCollider>().SetTrigger(true);
        slamHurtboxObject.AddComponent<ParticleFX>();
        slamHurtboxObject.GetComponent<ParticleFX>().ApplyPreset(24);
        slamHurtboxObject.GetComponent<ParticleFX>().EmitBurst(1);
        slamHurtboxObject.tag = "EnemyAttack";

        Vector3 forward = enemyTransform.forward;
        Vector3 bossPosition = enemyTransform.position;

        float halfLength = slamHurtboxSize.Z / 2.0f;
        float offset = 5.0f;

        Vector3 hurtboxPosition = bossPosition + forward * (halfLength + offset) + new Vector3(0, 5, 0);

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
            Vector3 position = origin + offset + new Vector3(0, 2, 0);

            GameObject clawSegment = Engineson.CreateGameObject("ClawHurtbox", null);
            //clawSegment.AddComponent<MeshRenderer>();
            clawSegment.AddComponent<BoxCollider>();
            clawSegment.GetComponent<BoxCollider>().SetTrigger(true);
            clawSegment.tag = "EnemyAttack";

            var hurtboxTransform = clawSegment.GetComponent<Transform>();
            hurtboxTransform.position = position;
            hurtboxTransform.SetScale(size.X, size.Y, size.Z);

            float angle = (float)Math.Atan2(forward.X, forward.Z);
            Quaternion rotation = Quaternion.CreateFromAxisAngle(Vector3.UnitY, angle);
            hurtboxTransform.SetRotationQuat(rotation);

            clawHurtboxObjects.Add(clawSegment);
        }

    }

    private void DestroyHurtboxes()
    {
        if (slamHurtboxObject != null)
        {
            Engineson.Destroy(slamHurtboxObject);
            slamHurtboxObject = null;
        }

        foreach (var hurtbox in clawHurtboxObjects)
        {
            Engineson.Destroy(hurtbox);
        }
        clawHurtboxObjects.Clear();
    }
}