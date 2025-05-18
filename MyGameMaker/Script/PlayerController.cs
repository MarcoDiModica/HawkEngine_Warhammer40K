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
    private PlayerAnimations playerAnimations;
    private GameObject playerMesh;
    private ParticleFX bloodSplashEffect;
    private CapsuleCollider capsuleCollider;
    //private ShakeManager shakeManager;
    private bool isIdle = false;
    public bool isShootInput = false;
    public bool isRunning = false;
    private bool isWalking = false;
    private bool isMoving = false;
    private bool isDashInput = false;
    private bool isRunningInput = false;
    private bool isShootingStanding = false;
    private bool isShootingRunning = false;
    private bool isTransitioning = false;
    private float transitionTimer = 0f;
    private float transitionDelay = 0.1f;
    private bool isDashing = false;
    Vector3 moveDirection;
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
    private const string Runfootsteps = "Assets/Audio/Player/Player_Footsteps.wav";
    private const string Walkfootsteps = "Assets/Audio/Player/Player_Footsteps.wav";
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
    private float dashEndTimer = 0.25f;

    private bool componentsInitialized = false;
    private bool effectsInitialized = false;

    private bool isReloadingIdle = false;
    private bool isReloadingRunning = false;
    private bool isReloadingWalking = false;

    private bool isChangingWeaponIdle = false;
    private bool isChangingWeaponRunning = false;
    private bool isChangingWeaponWalking = false;

    public override void Awake()
    {
        playerInput = gameObject.GetComponent<PlayerInput>();
        playerMovement = gameObject.GetComponent<PlayerMovement>();
        playerDash = gameObject.GetComponent<PlayerDash>();
        playerShooting = gameObject.GetComponent<PlayerShooting>();
        redThirstManager = gameObject.GetComponent<RedThirstManager>();
        transform = gameObject.GetComponent<Transform>();
        capsuleCollider = gameObject.GetComponent<CapsuleCollider>();

        if (playerInput == null || playerMovement == null || playerDash == null ||
            playerShooting == null || transform == null || capsuleCollider == null)
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
        if (!componentsInitialized || playerData == null)
            return;

        //playerData.FullHealth();

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
                    SceneManager.LoadScene("LoseScene");
                }
            }
            playerData.isHit = false;
        }

        HandleDebugControls();

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
        Vector3 lookDirection = playerInput.GetCurrentLookDirection();
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

        if(isReloadingIdle)
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
            playerDash.InitiateDash(moveDirection, elapsedTime);

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

        Vector3 localOffset = new Vector3(-0.9f, 2.5f, 0.5f);
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

            if (shouldBeRunning && !isRunning)
            {
                isWalking = false;
                isIdle = false;
                isShootingStanding = false;
                isShootingRunning = false;

                playerAnimations?.IdleToRunAnimation();
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
            }
            else if (isRunning)
            {
                SetRunningToIdle();
            }
            else if (!isIdle)
            {
                SetIdleState();
            }
        }
    }

    private void ProcessDashLogic(float deltaTime)
    {
        if (isDashing)
        {
            dashEndTimer -= deltaTime;
            if (dashEndTimer <= 0f)
            {
                if (playerInput != null)
                {
                    playerInput.UnBlockInput();
                }

                TransitionFromDashState();
                isDashing = false;
                dashEndTimer = 0.25f;
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
            SceneManager.LoadSceneFromCheckpoint("BossFight_Alpha1_Release");
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
                playerAnimations.SetDashToIdleAnimation();
                isIdle = true;
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
            Audio.Stop(Runfootsteps);
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

    private void SetRunningState()
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
        else if (transitionTimer <= 0f && !isRunning)
        {
            playerAnimations.IdleToRunAnimation();
            isRunning = true;
            isWalking = false;
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

        if (moveDirection != Vector3.Zero && !isShootingRunning)
        {
            playerAnimations.RunningToShootingWalkingStraightAnimation();
            isShootingStanding = false;
            isShootingRunning = true;
            isFootstepPlaying = false;
            isWalking = false;
            isRunning = false;
            isIdle = false;
        }
        else if (!isShootingStanding && moveDirection == Vector3.Zero)
        {
            playerAnimations.IdleToShootingStillAnimation();
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
        string newFootstep = isRunning ? Runfootsteps : Walkfootsteps;

        if (isFootstepPlaying && currentFootstep == newFootstep)
            return;

        StopFootsteps();

        if (isRunning)
        {
            audioRun = Audio.Play(Runfootsteps, true); 
        }
        else
        {
            audioWalk = Audio.Play(Walkfootsteps, true);
        }

        isFootstepPlaying = true;
        hasStoppedFootsteps = false;
        currentFootstep = newFootstep;
    }

    private void StopFootsteps()
    {
        if (isFootstepPlaying)
        {
            if (audioRun > 0) Audio.Stop(audioRun);
            if (audioWalk > 0) Audio.Stop(audioWalk);

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