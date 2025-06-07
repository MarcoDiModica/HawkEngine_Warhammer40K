using System;
using System.Collections;
using System.Numerics;
using HawkEngine;
public class PlayerController : MonoBehaviour
{
    private PlayerInput playerInput;
    private PlayerMovement playerMovement;
    public PlayerDash playerDash;
    public PlayerShooting playerShooting;
    public RedThirstManager redThirstManager;
    public PlayerAnimations playerAnimations;
    private GameObject playerMesh;
    private ParticleFX bloodSplashEffect;
    private CapsuleCollider capsuleCollider;
    private Rigidbody rb;
    //private ShakeManager shakeManager;
    public bool isIdle = false;
    public bool isShootInput = false;
    public bool isRunning = false;
    private bool isWalking = false;
    private bool isMoving = false;
    private bool isDashInput = false;
    private bool isRunningInput = false;
    public bool isShootingStanding = false;
    public bool isShootingRunning = false;
    private bool isTransitioning = false;
    private float transitionTimer = 0f;
    private float transitionDelay = 0.1f;
    public bool isDashing = false;
    Vector3 moveDirection;
    Vector3 lookDirection;
    private bool once = false;

    private float elapsedTime = 0f;
    private bool isInteracting = false;
    private float dashDelayTimer = 0f;
    private float dashDelayDuration = 0.45f;

    //color change
    private bool isFlashingColor = false;
    private float flashTimer = 0f;
    private Vector4 originalColor = new Vector4(1, 1, 1, 1);
    public Vector4 flashColor = new Vector4(1, 0, 0, 1); // rojo
    public float flashDuration = 0.1f;
    //private AudioSource sound;
    private bool isFootstepPlaying = false;
    private bool hasStoppedFootsteps = false;
    private const string RunfootstepsSolid = "Assets/Audio/Player/Player_Footsteps.wav";
    private const string RunfootstepsDirt = "Assets/Audio/Player/PlayerDirtFootSteps.wav";
    private const string RunfootstepsMetal = "Assets/Audio/Player/PlayerMetalFootSteps.wav";
    private string Walkfootsteps = "Assets/Audio/Player/Player_Footsteps.wav";
    public const string HitAudio = "Assets/Audio/Player/PlayerHurt.wav";
    public const string DeathAudio = "Assets/Audio/Player/Player_Death.wav";

    private int audioRun;
    private int audioWalk;

    private ParticleFX inactiveDashFX;
    private ParticleFX walkingFX;

    public PlayerData playerData;

    public GameObject aimLaser;
    public GameObject aimLaserEnd;
    private Transform transform;
    private float dashEndTimer = 0.3f;
    private float blockTimer = 0.2f;
    private bool isBlockingInput = false;

    private bool componentsInitialized = false;
    private bool effectsInitialized = false;

    private bool isReloadingIdle = false;
    private bool isReloadingRunning = false;
    private bool isReloadingWalking = false;

    private bool isChangingWeaponIdle = false;
    private bool isChangingWeaponRunning = false;
    private bool isChangingWeaponWalking = false;

    private bool isHitIdle = false;
    private bool isHitRunning = false;
    private bool isHitShooting = false;

    private int frameCounter = 0;


    public enum TerrainType
    {
        Solid,
        Dirt,
        Metal,
    }

    public TerrainType currentTerrainType = TerrainType.Solid;
    public TerrainType previousTerrain = TerrainType.Solid;

    public enum LookingDirection
    {
        Backward,
        Forward,
        Idle,
        Left,
        Right
    }

    public LookingDirection currentLookingDirection = LookingDirection.Idle;

    public enum ShootingDirection
    {
        Backward,
        Forward,
        Idle,
        Left,
        Right
    }

    public ShootingDirection currentShootingDirection = ShootingDirection.Idle;

    public override void Awake()
    {
        
        playerInput = gameObject.GetComponent<PlayerInput>();
        playerMovement = gameObject.GetComponent<PlayerMovement>();
        playerDash = gameObject.GetComponent<PlayerDash>();
        playerShooting = gameObject.GetComponent<PlayerShooting>();
        redThirstManager = gameObject.GetComponent<RedThirstManager>();
        transform = gameObject.GetComponent<Transform>();
        capsuleCollider = gameObject.GetComponent<CapsuleCollider>();
        rb = gameObject.GetComponent<Rigidbody>();

        if (playerInput == null || playerMovement == null || playerDash == null ||
            playerShooting == null || transform == null || capsuleCollider == null ||
            rb == null)
        {
            Engineson.print("ERROR: PlayerController missing required components");
            return;
        }

        playerMesh = GameObject.Find("playerMesh");
        if (playerMesh != null)
        {
            playerAnimations = playerMesh.GetComponent<PlayerAnimations>();
            if (playerAnimations != null)
            {
                SkeletalAnimation anim = playerMesh.GetComponent<SkeletalAnimation>();
                if (anim != null)
                {
                    anim.SetAnimationSpeed(2f);
                }
            }
            else
            {
                Engineson.print("ERROR: PlayerAnimations component not found on playerMesh");
            }
        }
        else
        {
            Engineson.print("ERROR: playerMesh not found");
        }

        playerData = PlayerData.Instance;

        bloodSplashEffect = gameObject.AddComponent<ParticleFX>();
        if (bloodSplashEffect != null)
        {
            bloodSplashEffect.ApplyPreset(19); 
        }

        GameObject inactiveDashObj = GameObject.Find("InactiveDashFX");
        GameObject walkingObj = GameObject.Find("WalkingFX");

        if (inactiveDashObj != null && walkingObj != null)
        {
            inactiveDashFX = inactiveDashObj.GetComponent<ParticleFX>();
            walkingFX = walkingObj.GetComponent<ParticleFX>();
            walkingFX.ApplyPreset(43);
            walkingFX.EmitBurst(1);
            if (inactiveDashFX != null && walkingFX != null)
            {
                effectsInitialized = true;
            }
            else
            {
                Engineson.print("WARNING: Could not find particle effect components");
            }
        }
        else
        {
            Engineson.print("WARNING: Could not find particle effect objects");
        }

        aimLaser = gameObject.GetChild("AimLaser");
        aimLaserEnd = gameObject.GetChild("AimLaserEnd");

        if (aimLaser == null || aimLaserEnd == null)
        {
            Engineson.print("WARNING: Aim laser objects not found");
        }

        componentsInitialized = true;
    }

    public override void Start()
    {
        if (!componentsInitialized)
            return;

        gameObject.tag = "Player";

        if (bloodSplashEffect != null)
        {
            bloodSplashEffect.ApplyPreset(19);
        }
    }

    public override void Update(float deltaTime)
    {
        //In case the rb component is not well initialized, the set mass is made on the 2nd frame
        if (frameCounter < 2)
        {
            frameCounter++;
            if (frameCounter == 2)
            {
                rb?.SetMass(100.0f); 
            }
        }
        

        if (!componentsInitialized || playerData == null)
            return;

        if (playerData.GodMode)
        {
            playerData.FullHealth();
        }

        if (playerData.isHit)
        {
            if ((playerDash == null || !playerDash.isInvulnerable) && !playerData.GodMode)
            {
                int audioHit = Audio.PlayOneShot(HitAudio);

                if (bloodSplashEffect != null)
                {
                    bloodSplashEffect.EmitBurst(100);
                }

                if (playerData.GetHealth() <= 0)
                {
                    if (playerAnimations != null)
                    {
                        playerAnimations.SetDeathAnimation();
                    }

                    if (playerInput != null)
                    {
                        playerInput.BlockInput();
                    }

                    if (capsuleCollider != null)
                    {
                        capsuleCollider.SetActive(false);
                    }

                    int audioDeath = Audio.PlayOneShot(DeathAudio);
                    //SceneManager.LoadScene("LoseScene");
                }
                else
                {
                    if (currentLookingDirection != LookingDirection.Backward)
                    {
                        if (isIdle)
                        {
                            isHitIdle = true;
                            playerAnimations.IdleToHitAnimation();
                        }
                        else if (isRunning || isShootingRunning)
                        {
                            isHitRunning = true;
                            playerAnimations.RunningToHitAnimation();
                        }
                        else if (isShootingStanding)
                        {
                            isHitShooting = true;
                            playerAnimations.ShootingStandingToHitAnimation();
                        }
                    }
                } 
            }
            playerData.isHit = false;
        }

        if (isHitIdle)
        {
            HitIdleAnimationFinished();
        }
        else if (isHitRunning)
        {
            HitRunningAnimationFinished();
        }
        else if (isHitShooting)
        {
            HitShootingAnimationFinished();
        }

        HandleDebugControls();

        CheckTerrain();

        ProcessInputAndState(deltaTime);

        ProcessDashLogic(deltaTime);

        if (isFlashingColor)
        {
            flashTimer -= deltaTime;
            if (flashTimer <= 0f)
            {
                if (playerMesh != null)
                {
                    MeshRenderer renderer = playerMesh.GetComponent<MeshRenderer>();
                    if (renderer != null)
                    {
                        renderer.SetColor(originalColor);
                    }
                }
                isFlashingColor = false;
            }
        }
    }

    private void HitIdleAnimationFinished()
    {


        if (playerAnimations.esk.IsAnimationFinished())
        {
            Engineson.print("Hit animation finished");
            isHitIdle = false;
            playerAnimations.HitIdleToIdleAnimation();

        }
    }

    private void HitRunningAnimationFinished()
    {


        if (playerAnimations.esk.IsAnimationFinished())
        {
            Engineson.print("Hit animation finished");
            isHitRunning = false;
            if (isShootingRunning)
            {
                playerAnimations.HitRunningToHitShootingRunningAnimation();
            }
            else
            {
                if (currentLookingDirection == LookingDirection.Forward)
                {
                    playerAnimations.HitRunningToRunningAnimation();
                }
                else if (currentLookingDirection == LookingDirection.Backward)
                {
                    playerAnimations.HitRunningToRunningBackwardsAnimation();
                }
            }
            
            

        }
    }

    private void HitShootingAnimationFinished()
    {


        if (playerAnimations.esk.IsAnimationFinished())
        {
            Engineson.print("Hit animation finished");
            isHitRunning = false;
            if (isShootingStanding)
            {
                playerAnimations.HitShootingStandingToShootingAnimation();
            }
            else 
            {
                playerAnimations.HitShootingStandingToIdleAnimation();
            }

        }
    }

    private void CheckTerrain()
    {
        RayCast ray = new RayCast();

        Vector3 rayStart = transform.position + transform.forward * 3 + new Vector3(0, 3f, 0);
        ray.PerformRaycast(rayStart, new Vector3(0, -1f, 0), 5f);

        if (ray.hit.isHit)
        {
            string terrainTag = ray.hit.gameObject.tag;
            Engineson.print($"Terrain hit: {terrainTag}");

            switch (terrainTag)
            {
                case "Solid":
                    currentTerrainType = TerrainType.Solid;
                    break;
                case "Dirt":
                    currentTerrainType = TerrainType.Dirt;
                    break;
                case "Metal":
                    currentTerrainType = TerrainType.Metal;
                    break;
                default:
                    currentTerrainType = TerrainType.Solid; // Default case
                    break;
            }

            if (currentTerrainType != previousTerrain)
            {
                Engineson.print($"Terrain changed from {previousTerrain} to {currentTerrainType}");
                previousTerrain = currentTerrainType;

                if (isFootstepPlaying)
                {
                    StopFootsteps();
                    PlayFootstep();
                }
            }
        }
    }

    private void ProcessInputAndState(float deltaTime)
    {
        if (playerInput == null)
            return;

        dashDelayTimer -= deltaTime;
        transitionTimer -= deltaTime;

        if (!once && playerAnimations != null)
        {
            playerAnimations.SetStandardIdleAnimation();
            once = true;
        }

        moveDirection = playerInput.GetCurrentMoveDirection();
        lookDirection = playerInput.GetCurrentLookDirection();
        bool isRunningInput = playerInput.IsRunningPressed();
        bool isKeyboard = playerInput.IsKeyboardMoving();
        isDashInput = playerInput.GetDashInput();
        isShootInput = playerInput.GetShootInput();

        if (playerMovement != null)
        {
            if (isKeyboard)
            {
                playerMovement.SetSpeedToRun(); 
            }
            else
            {
                if (isRunningInput)
                    playerMovement.SetSpeedToRun();
                else
                    playerMovement.SetSpeedToWalk();
            }

            playerMovement.SetMoveDirection(moveDirection);
            playerMovement.SetLookDirection(lookDirection);
        }

        if (playerInput.IsReloading())
        {

            if (playerShooting.GetCurrentAmmo() >= playerShooting.GetMaxMagazineAmmo())
                return;

            if (playerShooting.GetCurrentGun() == 0)
            {
                if (isIdle || isShootInput && !isShootingRunning)
                {
                    isReloadingIdle = true;
                    playerAnimations.ReloadBoltgunWhileIdleAnimation();
                }
                else if (isRunning)
                {
                    isReloadingRunning = true;
                    playerAnimations.ReloadBoltgunWhileRunningAnimation();
                }
                else if (isWalking || isShootingRunning)
                {
                    isReloadingWalking = true;
                    playerAnimations.ReloadBoltgunWhileWalkingForwardAnimation();
                }
            }
            else if (playerShooting.GetCurrentGun() == 1)
            {
                if (isIdle || isShootInput && !isShootingRunning)
                {
                    isReloadingIdle = true;
                    playerAnimations.ReloadShotgunWhileIdleAnimation();
                }
                else if (isRunning)
                {
                    isReloadingRunning = true;
                    playerAnimations.ReloadShotgunWhileRunningAnimation();
                }
                else if (isWalking || isShootingRunning)
                {
                    isReloadingWalking = true;
                    playerAnimations.ReloadShotgunWhileWalkingForwardAnimation();
                }
            }
        }

        if (isReloadingIdle)
        {
            ReloadingIdleAnimationFinished();
        }
        else if (isReloadingRunning)
        {
            ReloadingRunningAnimationFinished();
        }
        else if (isReloadingWalking)
        {
            ReloadingWalkingAnimationFinished();
        }

        if (playerInput.IsChangingWeaponLeft() || playerInput.IsChangingWeaponRight())
        {
            if (playerData.hasShotgun || playerData.hasRailgun)
            {
                if (isIdle || isShootInput && !isShootingRunning)
                {
                    isChangingWeaponIdle = true;
                    playerAnimations.WeaponSwapWhileIdleAnimation();
                }
                else if (isRunning)
                {
                    isChangingWeaponRunning = true;
                    playerAnimations.WeaponSwapWhileRunningAnimation();
                }
                else if (isWalking || isShootingRunning)
                {
                    isChangingWeaponWalking = true;
                    playerAnimations.WeaponSwapWhileWalkingStraightAnimation();
                }
            }
            
        }

        if (isChangingWeaponIdle)
        {
            WeaponSwapIdleAnimationFinished();
        }
        else if (isChangingWeaponRunning)
        {
            WeaponSwapRunningAnimationFinished();
        }
        else if (isChangingWeaponWalking)
        {
            WeaponSwapWalkingAnimationFinished();
        }

        elapsedTime += deltaTime;

        UpdateAimLaser();

        UpdateCharacterState();

        if (effectsInitialized && playerDash != null)
        {
            if (!playerDash.CanDash(elapsedTime))
            {
                inactiveDashFX.Play();
            }
            else
            {
                inactiveDashFX.Stop();
            }
        }

        if (isDashInput && playerDash != null && playerDash.CanDash(elapsedTime))
        {
            isDashing = true;
            isIdle = false;
            isWalking = false;
            isRunning = false;
            isShootingStanding = false;
            isShootingRunning = false;
            playerDash.InitiateDash(moveDirection, elapsedTime);
            isBlockingInput = true;
            if (playerAnimations != null)
            {
                playerAnimations.SetDashAnimation();

            }

            if (playerInput != null)
            {
                playerInput.BlockInput();
            }

            StopFootsteps();
        }
    }
    private void UpdateCharacterState()
    {
        if (isShootInput && !isDashing)
        {
            SetShootingState();
            return;
        }

        if (moveDirection != Vector3.Zero)
        {
            if (effectsInitialized)
            {
                walkingFX.Play();
            }

            if (!isFootstepPlaying)
            {
                PlayFootstep();
            }

            bool shouldBeRunning = playerMovement != null &&
                                  (playerMovement.moveSpeed > playerMovement.walkSpeed ||
                                   isRunningInput);

            if (shouldBeRunning && !isRunning && !isDashing)
            {
                isWalking = false;
                isIdle = false;
                isShootingStanding = false;
                isShootingRunning = false;

                SetRunningAnimation();

                isRunning = true;
                isMoving = true;
                transitionTimer = 0f;
            }
            else if (!shouldBeRunning && !isWalking)
            {
                isRunning = false;
                isIdle = false;
                isShootingStanding = false;
                isShootingRunning = false;

                playerAnimations?.IdleToWalkingAnimation();
                isWalking = true;
                isMoving = true;
                transitionTimer = 0f;
            }

            if (isRunning)
            {
                SetRunningAnimation();
            }
        }
        else
        {
            StopFootsteps();

            if (effectsInitialized)
            {
                walkingFX.Stop();
            }

            if (isWalking)
            {
                SetWalkingToIdle();
                currentLookingDirection = LookingDirection.Idle;
                currentShootingDirection = ShootingDirection.Idle;
            }
            else if (isRunning)
            {
                SetRunningToIdle();
                currentLookingDirection = LookingDirection.Idle;
                currentShootingDirection = ShootingDirection.Idle;
            }
            else if (!isIdle && !isDashing)
            {
                SetIdleState();
                currentLookingDirection = LookingDirection.Idle;
                currentShootingDirection = ShootingDirection.Idle;
            }
        }
    }

    private void SetRunningAnimation()
    {
        const float epsilon = 0.1f;
        

        if (moveDirection.LengthSquared() < epsilon)
        {
            // No se está moviendo
            return;
        }

        Vector3 moveDir = Vector3.Normalize(moveDirection);
        Vector3 lookDir = Vector3.Normalize(lookDirection);

        if (lookDirection == Vector3.Zero)
        {
            lookDir = moveDir;
        }

        float dot = Vector3.Dot(lookDir, moveDir);

        if (dot > 0.7f && (currentLookingDirection != LookingDirection.Forward || isShootingRunning))
        {
            
            if (currentLookingDirection == LookingDirection.Idle && !isShootingRunning)
            {
               
                playerAnimations.IdleToRunAnimation();
            }
            else if (currentLookingDirection == LookingDirection.Backward || isShootingRunning)
            {
                if (isShootingRunning)
                {
                    
                    switch (currentShootingDirection)
                    {
                        
                        case ShootingDirection.Left:
                            playerAnimations.ShootingWalkingLeftToRunAnimation();
                            break;
                        case ShootingDirection.Right:
                            playerAnimations.ShootingWalkingRightToRunAnimation();
                            break;
                        case ShootingDirection.Forward:
                            playerAnimations.ShootingWalkingStraightToRunAnimation();
                            break;
                        case ShootingDirection.Backward:
                            playerAnimations.ShootingWalkingBackwardsToRunAnimation();
                            break;
                    }
                }
                else
                {
                    playerAnimations.RunningBackwardsToRunningForwardAnimation();
                   
                }
                
            }
            

            currentLookingDirection = LookingDirection.Forward;
        }
        else if (dot < -0.7f && (currentLookingDirection != LookingDirection.Backward || isShootingRunning))
        {

            if (currentLookingDirection == LookingDirection.Idle)
            {
                playerAnimations.IdleToRunBackwardsAnimation();
            }
            else if (currentLookingDirection == LookingDirection.Forward || isShootingRunning)
            {
                if (isShootingRunning)
                {
                    switch (currentShootingDirection)
                    {
                        case ShootingDirection.Left:
                            playerAnimations.ShootingWalkingLeftToRunBackwardsAnimation();
                            break;
                        case ShootingDirection.Right:
                            playerAnimations.ShootingWalkingRightToRunBackwardsAnimation();
                            break;
                        case ShootingDirection.Forward:
                            playerAnimations.ShootingWalkingStraightToRunBackwardsAnimation();
                            break;
                        case ShootingDirection.Backward:
                            playerAnimations.ShootingWalkingBackwardsToRunBackwardsAnimation();
                            break;
                    }
                }
                else
                {
                    playerAnimations.RunningForwardToRunningBackwardsAnimation();
                }
                
            }
            

            currentLookingDirection = LookingDirection.Backward;
        }
        currentShootingDirection = ShootingDirection.Idle;
    }


    private void ReloadingIdleAnimationFinished()
    {

        
        if (playerAnimations.esk.IsAnimationFinished())
        {
            Engineson.print("Reloading animation finished");
            isReloadingIdle = false;
            if (playerInput.IsShooting())
            {
                if (playerShooting.GetCurrentGun() == 0)
                {
                    playerAnimations.ReloadBoltgunIdleToShootingStandingAnim();
                }
                else if (playerShooting.GetCurrentGun() == 1)
                {
                    playerAnimations.ReloadShotgunIdleToShootingStandingAnim();
                }
            }
            else
            {
                if (playerShooting.GetCurrentGun() == 0)
                {
                    playerAnimations.ReloadBoltgunIdleToIdleAnim();
                }
                else if (playerShooting.GetCurrentGun() == 1)
                {
                    playerAnimations.ReloadShotgunIdleToIdleAnim();
                }
            }
            
        }
    }

    private void ReloadingRunningAnimationFinished()
    {
        if (playerAnimations.esk.IsAnimationFinished())
        {
            Engineson.print("Reloading animation finished");
            isReloadingRunning = false;
            if (playerShooting.GetCurrentGun() == 0)
            {
                playerAnimations.ReloadBoltgunRunningToRunningAnimation();
            }
            else if (playerShooting.GetCurrentGun() == 1)
            {
                playerAnimations.ReloadShotgunRunningToRunningAnimation();
            }
            
        }
    }

    private void ReloadingWalkingAnimationFinished()
    {
        if (playerAnimations.esk.IsAnimationFinished())
        {
            Engineson.print("Reloading animation finished");
            isReloadingWalking = false;
            if (playerInput.IsShooting())
            {
                if (playerShooting.GetCurrentGun() == 0)
                {
                    playerAnimations.ReloadBoltgunWalkingToShootingWalkingAnimation();
                }
                else if (playerShooting.GetCurrentGun() == 1)
                {
                    playerAnimations.ReloadShotgunWalkingToShootingWalkingAnimation();
                }
            }
            else
            {
                if (playerShooting.GetCurrentGun() == 0)
                {
                    playerAnimations.ReloadBoltgunWalkingToWalkingAnimation();
                }
                else if (playerShooting.GetCurrentGun() == 1)
                {
                    playerAnimations.ReloadShotgunWalkingToWalkingAnimation();
                }
            }
            

        }
    }

    private void WeaponSwapIdleAnimationFinished()
    {


        if (playerAnimations.esk.IsAnimationFinished())
        {
            isChangingWeaponIdle = false;
            if (playerInput.IsShooting())
            {
                playerAnimations.WeaponSwapShootingToShootingAnimation();
            }
            else
            {
                playerAnimations.WeaponSwapIdleToIdleAnimation();
            }

        }
    }

    private void WeaponSwapRunningAnimationFinished()
    {
        if (playerAnimations.esk.IsAnimationFinished())
        {
            
            isChangingWeaponRunning = false;
            playerAnimations.WeaponSwapRunningToRunningAnimation();

        }
    }

    private void WeaponSwapWalkingAnimationFinished()
    {
        if (playerAnimations.esk.IsAnimationFinished())
        {
            isChangingWeaponWalking = false;
            if (playerInput.IsShooting())
            {
                playerAnimations.WeaponSwapWalkingShootingToWalkingShootingAnimation();
            }
            else
            {
                playerAnimations.WeaponSwapWalkingToWalkingAnimation();
            }


        }
    }

    private void UpdateAimLaser()
    {
        if (transform == null || aimLaser == null || aimLaserEnd == null)
            return;

        Vector3 localOffset = new Vector3(-0.18f, 2.5f, 3.5f);
        Vector3 bulletStart = transform.position +
                              (transform.right * localOffset.X) +
                              (transform.up * localOffset.Y) +
                              (transform.forward * localOffset.Z);
        bulletStart.Y += 0.75f;

        if (playerInput != null && playerInput.IsShooting())
        {
            aimLaser.SetActive(true);

            RayCast rayAim = new RayCast();
            int maxDistance = 50;
            rayAim.PerformRaycast(bulletStart, Vector3.Normalize(transform.forward), maxDistance);

            if (rayAim.hit.isHit)
            {
                aimLaserEnd.SetActive(true);
                aimLaser.transform.position = bulletStart + (Vector3.Normalize(transform.forward) * 2);
                aimLaserEnd.transform.position = bulletStart + (Vector3.Normalize(transform.forward) * (rayAim.hit.distance));
            }
            else
            {
                aimLaserEnd.SetActive(false);
                aimLaser.transform.position = bulletStart + (Vector3.Normalize(transform.forward) * 2);
            }
        }
        else
        {
            aimLaser.SetActive(false);
            aimLaserEnd.SetActive(false);
        }
    }

    

    private void ProcessDashLogic(float deltaTime)
    {
        if (isDashing)
        {
            dashEndTimer -= deltaTime;
            blockTimer -= deltaTime;
            if (blockTimer <= 0f && playerInput != null && isBlockingInput)
            {
                playerInput.UnBlockInput();
                blockTimer = 0.2f;
                isBlockingInput = false;
            }
            if (dashEndTimer <= 0f)
            {
                if (playerInput != null)
                {
                    playerInput.UnBlockInput();
                }

                TransitionFromDashState();
                isDashing = false;
                dashEndTimer = 0.3f;
            }
        }
    }

    private void HandleDebugControls()
    {
        if (Input.GetKeyDown(KeyCode.B))
        {
            playerData.TakeDamage(10);
            StartFlashColor(flashColor, flashDuration);
        }

        if (Input.GetKeyDown(KeyCode.V))
        {
            playerData.AddHealth(10);

            if (playerMesh != null)
            {
                MeshRenderer renderer = playerMesh.GetComponent<MeshRenderer>();
                if (renderer != null)
                {
                    renderer.SetColor(new Vector4(0, 1, 0, 1));
                }
            }
        }

        if (Input.GetKeyDown(KeyCode.M))
        {
            SceneManager.LoadSceneFromCheckpoint("BetaRelease_Week1_Lvl2");
        }

        if (Input.GetKeyDown(KeyCode.H))
        {
            SceneManager.LoadSceneFromCheckpoint("BetaRelease_Week1_Bossfight");
        }

        if (Input.GetKeyDown(KeyCode.K))
        {
            SceneManager.LoadSceneFromCheckpoint("BetaRelease_Week1_Lvl1");
        }

        if (Input.GetKeyDown(KeyCode.N))
        {
            playerData.AddHealth(10);
        }

        if (Input.GetKeyDown(KeyCode.Z))
        {
            playerData.GodMode = !playerData.GodMode;
        }
    }

    private void TransitionFromDashState()
    {
        if (playerAnimations == null)
            return;

        isIdle = false;
        isWalking = false;
        isRunning = false;
        isShootingStanding = false;
        isShootingRunning = false;
        isTransitioning = false;

        if (moveDirection != Vector3.Zero)
        {
            if (isShootInput)
            {
                playerAnimations.SetDashToShootingRunningAnimation();
                isShootingRunning = true;
            }
            else if (playerMovement != null &&
                    (playerMovement.moveSpeed > playerMovement.walkSpeed ||
                     isRunningInput))
            {
                playerAnimations.SetDashToRunningAnimation();
                isRunning = true;
            }
            else
            {
                playerAnimations.SetDashToWalkingAnimation();
                isWalking = true;
            }
            isMoving = true;
        }
        else
        {
            if (isShootInput)
            {
                playerAnimations.SetDashToShootingStandingAnimation();
                isShootingStanding = true;
            }
            else
            {
                if(Input.GetLeftStick().Length() > 0.1f)
                {
                    playerAnimations.SetDashToRunningAnimation();
                    isWalking = true;
                }
                else
                {
                    playerAnimations.SetStandardIdleAnimation();
                    isIdle = true;
                }
               
            }
            isMoving = false;
        }
    }

    private void SetIdleState()
    {
        if (playerAnimations == null)
            return;

        if (!isIdle)
        {
            if (isShootingStanding)
            {
                playerAnimations.ShootingStillToIdleAnimation();
            }
            else
            {
                playerAnimations.SetStandardIdleAnimation();
            }

            isIdle = true;
            isRunning = false;
            isWalking = false;
            isShootingStanding = false;
            isShootingRunning = false;
            isMoving = false;
        }

        if (!hasStoppedFootsteps)
        {
            Audio.Stop(RunfootstepsSolid);
            Audio.Stop(RunfootstepsDirt);
            Audio.Stop(RunfootstepsMetal);
            Audio.Stop(Walkfootsteps);
            hasStoppedFootsteps = true;
            isFootstepPlaying = false;
        }
    }

    private void SetWalkingState()
    {
        if (playerAnimations == null)
            return;

        if (isShootInput && !isShootingRunning)
        {
            TransitionShootingStandingToRunning();
        }
        else if (isShootingRunning && !isShootInput)
        {
            playerAnimations.ShootingWalkingStraightToRunAnimation();
            isRunning = false;
            isWalking = false;
            isIdle = false;
            isShootingStanding = false;
            isShootingRunning = false;
            isMoving = true;
            isTransitioning = true;
            transitionTimer = transitionDelay;
        }
        else if (transitionTimer <= 0f && !isWalking)
        {
            playerAnimations.IdleToWalkingAnimation();
            isRunning = false;
            isWalking = true;
            isIdle = false;
            isShootingStanding = false;
            isShootingRunning = false;
            isMoving = true;
            isTransitioning = false;
        }
    }

    private void SetShootingState()
    {
        if (playerAnimations == null)
            return;

        if (isShootingRunning)
        {
            SetRunningShootingAnimation();
        }

        if (moveDirection != Vector3.Zero && !isShootingRunning)
        {
            SetRunningShootingAnimation();
            isShootingStanding = false;
            isShootingRunning = true;
            isFootstepPlaying = false;
            isWalking = false;
            isRunning = false;
            isIdle = false;
        }
        else if (!isShootingStanding && moveDirection == Vector3.Zero)
        {

            switch(currentShootingDirection)
            {
                case ShootingDirection.Forward:
                    playerAnimations.ShootingWalkingStraightToShootingStandingAnimation();
                    break;
                case ShootingDirection.Backward:
                    playerAnimations.ShootingWalkingBackwardsToShootingStandingAnimation();
                    break;
                case ShootingDirection.Left:
                    playerAnimations.ShootingWalkingLeftToShootingStandingAnimation();
                    break;
                case ShootingDirection.Right:
                    playerAnimations.ShootingWalkingRightToShootingStandingAnimation();
                    break;
                case ShootingDirection.Idle:
                    playerAnimations.IdleToShootingStillAnimation();
                    break;
            }

            currentShootingDirection = ShootingDirection.Idle;
            isShootingStanding = true;
            isShootingRunning = false;
            isFootstepPlaying = false;
            isWalking = false;
            isRunning = false;
            isIdle = false;
        }

        

        if (isShootingStanding && moveDirection != Vector3.Zero)
        {
            playerAnimations.ShootingStandingToShootingWalkingStraightAnimation();
            isShootingStanding = false;
            isShootingRunning = true;
        }
    }

    private void SetRunningShootingAnimation()
    {
        const float epsilon = 0.1f;
        const float angleThreshold = 10f;

        if (moveDirection.LengthSquared() < epsilon)
            return;

        Vector3 moveDir = Vector3.Normalize(moveDirection);
        Vector3 lookDir = Vector3.Normalize(lookDirection);

        if (lookDirection == Vector3.Zero)
        {
            lookDir = moveDir;
        }

        float dot = Vector3.Dot(lookDir, moveDir);
        dot = Mathf.Clamp(dot, -1f, 1f);
        float angleRad = Mathf.Acos(dot);
        float angleDeg = angleRad * (180f / Mathf.PI);

        ShootingDirection newDir;

        if (currentShootingDirection != ShootingDirection.Idle)
        {
           

            if (angleDeg < 45f)
                newDir = ShootingDirection.Forward;
            else if (angleDeg > 135f)
                newDir = ShootingDirection.Backward;
            else
                newDir = Vector3.Cross(lookDir, moveDir).Y > 0 ? ShootingDirection.Left : ShootingDirection.Right;

            if (newDir == currentShootingDirection)
                return;

            float diff = GetDirectionAngleDifference(currentShootingDirection, newDir);
            if (diff < angleThreshold)
                return;

            

        }
        else
        {
            newDir = ShootingDirection.Forward;
        }


        switch (currentShootingDirection)
        {
            case ShootingDirection.Idle:
                switch (newDir)
                {
                    case ShootingDirection.Forward:
                        if (isShootingStanding)
                        {
                            playerAnimations.ShootingStandingToShootingWalkingStraightAnimation();
                        }
                        else
                        {
                            playerAnimations.RunningToShootingWalkingStraightAnimation();
                        }
                        break;
                    case ShootingDirection.Backward:
                        if (isShootingStanding)
                        {
                            playerAnimations.ShootingStandingToShootingWalkingBackwardsAnimation();
                        }
                        else
                        {
                            playerAnimations.RunningToShootingWalkingBackwardsAnimation();
                        }
                        break;
                    case ShootingDirection.Left:
                        if (isShootingStanding)
                        {
                            playerAnimations.ShootingStandingToShootingWalkingLeftAnimation();
                        } 
                        else
                        {
                            playerAnimations.RunningToShootingWalkingLeftAnimation();
                        }
                            
                        break;
                    case ShootingDirection.Right:
                        if (isShootingStanding)
                        {
                            playerAnimations.ShootingStandingToShootingWalkingRightAnimation();
                        }
                        else
                        {
                            playerAnimations.RunningToShootingWalkingRightAnimation();
                        }
                        break;
                }
                break;

            case ShootingDirection.Forward:
                if (newDir == ShootingDirection.Left) playerAnimations.ShootingWalkingStraightToShootingWalkingLeftAnimation();
                else if (newDir == ShootingDirection.Right) playerAnimations.ShootingWalkingStraightToShootingWalkingRightAnimation();
                else if (newDir == ShootingDirection.Backward) playerAnimations.ShootingWalkingStraightToShootingWalkingBackwardsAnimation();
                break;

            case ShootingDirection.Backward:
                if (newDir == ShootingDirection.Left) playerAnimations.ShootingWalkingBackwardsToShootingWalkingLeftAnimation();
                else if (newDir == ShootingDirection.Right) playerAnimations.ShootingWalkingBackwardsToShootingWalkingRightAnimation();
                else if (newDir == ShootingDirection.Forward) playerAnimations.ShootingWalkingBackwardsToShootingWalkingStraightAnimation();
                break;

            case ShootingDirection.Left:
                if (newDir == ShootingDirection.Forward) playerAnimations.ShootingWalkingLeftToShootingWalkingStraightAnimation();
                else if (newDir == ShootingDirection.Right) playerAnimations.ShootingWalkingLeftToShootingWalkingRightAnimation();
                else if (newDir == ShootingDirection.Backward) playerAnimations.ShootingWalkingLeftToShootingWalkingBackwardsAnimation();
                break;

            case ShootingDirection.Right:
                if (newDir == ShootingDirection.Forward) playerAnimations.ShootingWalkingRightToShootingWalkingStraightAnimation();
                else if (newDir == ShootingDirection.Left) playerAnimations.ShootingWalkingRightToShootingWalkingLeftAnimation();
                else if (newDir == ShootingDirection.Backward) playerAnimations.ShootingWalkingRightToShootingWalkingBackwardsAnimation();
                break;
        }

        currentShootingDirection = newDir;
        currentLookingDirection = LookingDirection.Idle;
    }

    private float GetDirectionAngleDifference(ShootingDirection oldDir, ShootingDirection newDir)
    {
        int oldAngle = GetDirectionAngle(oldDir);
        int newAngle = GetDirectionAngle(newDir);
        int diff = Math.Abs(oldAngle - newAngle);
        return Math.Min(diff, 360 - diff);
    }

    private int GetDirectionAngle(ShootingDirection dir)
    {
        switch (dir)
        {
            case ShootingDirection.Forward:
                return 0;
            case ShootingDirection.Right:
                return 90;
            case ShootingDirection.Backward:
                return 180;
            case ShootingDirection.Left:
                return 270;
            default:
                return -1;
        }
    }

    private void SetWalkingToIdle()
    {
        if (playerAnimations == null)
            return;

        if (isWalking)
        {
            playerAnimations.WalkingToIdleAnimation();
            isWalking = false;
            isIdle = true;
            isRunning = false;
            isShootingStanding = false;
            isShootingRunning = false;
            isMoving = false;
            isTransitioning = false;
            StopFootsteps();
        }
    }

    private void SetRunningToIdle()
    {
        if (playerAnimations == null)
            return;

        if (isRunning)
        {
            playerAnimations.RunToIdleAnimation();
            isRunning = false;
            isWalking = false;
            isIdle = true;
            isShootingStanding = false;
            isShootingRunning = false;
            isMoving = false;
            isTransitioning = false;
            StopFootsteps();
        }
    }

    private void TransitionShootingStandingToRunning()
    {
        if (playerAnimations == null)
            return;

        playerAnimations.ShootingStandingToShootingWalkingStraightAnimation();
        isShootingRunning = true;
        isShootingStanding = false;
        isTransitioning = true;
        transitionTimer = transitionDelay;
        isWalking = false;
        isRunning = false;
        isIdle = false;
        isMoving = true;
    }

    private string currentFootstep = "";
    private void PlayFootstep()
    {
        

        StopFootsteps();
        Engineson.print("PlayFootStep");

        switch (currentTerrainType)
        {
            case TerrainType.Solid:
                Engineson.print("Playing solid footstep sound");
                Audio.Play(RunfootstepsSolid, true);
                break;
            case TerrainType.Dirt:
                Engineson.print("Playing dirt footstep sound");
                Audio.Play(RunfootstepsDirt, true);
                break;
            case TerrainType.Metal:
                Engineson.print("Playing metal footstep sound");
                Audio.Play(RunfootstepsMetal, true);
                break;
        }
        
        

        isFootstepPlaying = true;
        hasStoppedFootsteps = false;
        
    }

    private void StopFootsteps()
    {
        if (isFootstepPlaying)
        {
            Audio.Stop(RunfootstepsSolid);
            Audio.Stop(RunfootstepsDirt);
            Audio.Stop(RunfootstepsMetal);
            Audio.Stop(Walkfootsteps);

            audioRun = 0;
            audioWalk = 0;
            isFootstepPlaying = false;
            hasStoppedFootsteps = true;
            currentFootstep = "";
        }
    }

    public void StartFlashColor(Vector4 color, float duration)
    {
        if (playerMesh == null)
            return;

        MeshRenderer renderer = playerMesh.GetComponent<MeshRenderer>();
        if (renderer != null)
        {
            renderer.SetColor(color);
            isFlashingColor = true;
            flashTimer = duration;
        }
    }

    public override void OnTriggerEnter(GameObject other)
    {
        if (other == null || playerShooting == null || playerDash == null)
            return;

        if (other.name == "Hurtbox")
        {
            if (playerDash.isInvulnerable)
            {
                BulletData bulletData = other.GetComponent<BulletData>();
                if (bulletData != null && bulletData.owner != null)
                {
                    playerShooting.CounterAttack(bulletData.owner);
                }
            }
        }
    }

    public override void OnCollisionEnter(GameObject other)
    {
    }

    public void BlockMovement()
    {
        if (playerMovement != null)
        {
            playerMovement.SetMoveDirection(Vector3.Zero);
        }

        StopFootsteps();

        if (effectsInitialized)
        {
            walkingFX.Stop();
        }

        if (playerInput != null)
        {
            playerInput.BlockMovement();
        }

        SetIdleState();
    }

    public void UnBlockMovement()
    {
        if (playerInput != null)
        {
            playerInput.UnBlockMovement();
        }
    }
}