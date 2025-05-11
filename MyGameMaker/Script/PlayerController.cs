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
    private bool isIdle = false;
    public bool isRunning = false;
    private bool isWalking = false;
    private bool isMoving = false;
    private bool isDashInput = false;
    private bool isShootInput = false;
    private bool isRunningInput = false;
    private bool isShootingStanding = false;
    private bool isShootingRunning = false;
    private bool isTransitioning = false;
    private float transitionTimer = 0f;
    private float transitionDelay = 0.1f;
    private bool isDashing = false;
    Vector3 moveDirection;
    private bool once = false;

    public bool canMove = true;


    private float elapsedTime = 0f;
    private bool isInteracting = false;
    private float dashDelayTimer = 0f; 
    private float dashDelayDuration = 0.45f;


    //color change
    private bool isFlashingColor = false;
    private float flashTimer = 0f;
    private Vector4 originalColor;
    public Vector4 flashColor = new Vector4(1, 0, 0, 1); // rojo
    public float flashDuration = 0.1f;
    //private AudioSource sound;
    private bool isFootstepPlaying = false;
    private bool hasStoppedFootsteps = false;
    private const string Runfootsteps = "Assets/Audio/SFX/Player/PlayerFootstep_ready.wav";
    private const string Walkfootsteps = "Assets/Audio/SFX/Player/PlayerWalkFootstep_ready.wav";
    public const string HitAudio = "Assets/Audio/SFX/Player/PlayerHit_ready.wav";
    public const string DeathAudio = "Assets/Audio/SFX/PlayerPlayerDeath_ready.wav";
    private int audioRun;
    private int audioWalk;

    private ParticleFX inactiveDashFX;
    private ParticleFX walkingFX;
    
    public PlayerData playerData;

    public GameObject aimLaser;
    public GameObject aimLaserEnd;
    private Transform transform;

    public override void Awake()
    {
        playerInput = gameObject.GetComponent<PlayerInput>();
        playerMovement = gameObject.GetComponent<PlayerMovement>();
        playerDash = gameObject.GetComponent<PlayerDash>();
        playerShooting = gameObject.GetComponent<PlayerShooting>();
        playerMesh = GameObject.Find("playerMesh");
        redThirstManager = gameObject.GetComponent<RedThirstManager>();
        playerAnimations = playerMesh.GetComponent<PlayerAnimations>();
        playerMesh.GetComponent<SkeletalAnimation>().SetAnimationSpeed(2f);
        //sound = gameObject.GetComponent<AudioSource>();
        //gameObject.GetComponent<Transform>().SetPosition(0, 0, 0);
        playerData = PlayerData.Instance;
        // Add the blood splash effect directly to the player object
        bloodSplashEffect = gameObject.AddComponent<ParticleFX>();
        bloodSplashEffect.ApplyPreset(19); // BLOOD_SPLASH preset (index 19)
        inactiveDashFX = GameObject.Find("InactiveDashFX").GetComponent<ParticleFX>();
        walkingFX = GameObject.Find("WalkingFX").GetComponent<ParticleFX>();
        capsuleCollider = gameObject.GetComponent<CapsuleCollider>();
        transform = gameObject.GetComponent<Transform>();

    }

    public override void Start()
    {
        gameObject.tag = "Player";
        bloodSplashEffect.ApplyPreset(19);
       
    }

    public override void Update(float deltaTime)
    {
        if (!canMove)
        {
            playerMovement.SetMoveDirection(Vector3.Zero);
            StopFootsteps();
            walkingFX.Stop();
            return;
        }
        if (playerData.isHit )
        {
            if (!playerDash.isInvulnerable && !playerData.GodMode)
            {
                int audioHit = Audio.PlayOneShot(HitAudio);

                if (bloodSplashEffect != null)
                {
                    bloodSplashEffect.EmitBurst(100);
                }

                if (playerData.GetHealth() <= 0)
                {
                    playerAnimations.SetDeathAnimation();
                    playerInput.BlockMovement();
                    capsuleCollider.SetActive(false);
                    int audioDeath = Audio.PlayOneShot(DeathAudio);
                    SceneManager.LoadScene("LoseScene");
                }
                else
                {
                    playerAnimations.SetHitIdleAnimation();
                }
            }
            playerData.isHit = false; 
        }
        //upon pressing B take 10 damage
        if (Input.GetKeyDown(KeyCode.B))
        {
            playerData.TakeDamage(10);
            StartFlashColor(flashColor, flashDuration);
        }

        if (Input.GetKeyDown(KeyCode.V))
        {
            playerData.AddHealth(10);
            MeshRenderer renderer = playerMesh.GetComponent<MeshRenderer>();
            if (renderer != null)
            {
                renderer.SetColor(new Vector4(0, 1, 0, 1));
            }
        }

        if (Input.GetKeyDown(KeyCode.M))
        {
            SceneManager.LoadScene("Lvl2Alpha1Release");
        }

        if (Input.GetKeyDown(KeyCode.H))
        {
            SceneManager.LoadScene("BossFight_Alpha1_Release");
        }

        if (Input.GetKeyDown(KeyCode.K))
        {
            SceneManager.LoadScene("Lvl1Alpha1Release");
        }

        if (Input.GetKeyDown(KeyCode.N))
        {
            playerData.AddHealth(10);
        }
        if (Input.GetKeyDown(KeyCode.Z) )
        {
            if (playerData.GodMode == true)
            {
                playerData.GodMode = false;
            }
            else
            {
                playerData.GodMode = true;
            }
        }
        
        dashDelayTimer -= deltaTime;
        transitionTimer -= deltaTime;
        if (once == false)
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
        isRunningInput = playerInput.IsRunningPressed();
        if (isKeyboard)
        {
            playerMovement.SetSpeedToRun(); // Teclado siempre corre
        }
        else
        {
            if (isRunningInput)
                playerMovement.SetSpeedToRun();
            else
                playerMovement.SetSpeedToWalk();
        }
        elapsedTime += deltaTime;
        playerMovement.SetMoveDirection(moveDirection);
        playerMovement.SetLookDirection(lookDirection);

        Vector3 localOffset = new Vector3(-0.9f, 2.5f, 0.5f);

        Vector3 bulletStart = transform.position +
                              (transform.right * localOffset.X) +
                              (transform.up * localOffset.Y) +
                              (transform.forward * localOffset.Z);
        bulletStart.Y += 0.75f;

        RayCast rayAim = new RayCast();
        int maxDistance = 50;
        rayAim.PerformRaycast(bulletStart, Vector3.Normalize(transform.forward), maxDistance);

        //aimLaser.transform.LookAt(lookDirection);
        if (playerInput.IsShooting())
        {
            aimLaser.SetActive(true);
            

            if (rayAim.hit.isHit)
            {
                //aimLaser.transform.localScale = new Vector3(aimLaser.transform.localScale.X, rayAim.hit.distance / 2, aimLaser.transform.localScale.Z);
                aimLaserEnd.SetActive(true);
                aimLaser.transform.position = bulletStart + (Vector3.Normalize(transform.forward) * 2);
                aimLaserEnd.transform.position = bulletStart + (Vector3.Normalize(transform.forward) * (rayAim.hit.distance));
            }
            else
            {
                //aimLaser.transform.localScale = new Vector3(aimLaser.transform.localScale.X, maxDistance / 2, aimLaser.transform.localScale.Z);
                aimLaserEnd.SetActive(false);
                aimLaser.transform.position = bulletStart + (Vector3.Normalize(transform.forward) * 2);
            }

        }
        else
        {
            aimLaser.SetActive(false);
            aimLaserEnd.SetActive(false);
        }

        //if (dashDelayTimer > 0f)
        //{
        //    return;
        //}
        if (isShootInput)
        {
            SetShootingState();
            isIdle = false;
        }
        else
        {
           
            if (moveDirection == Vector3.Zero)
            {
                StopFootsteps();
                walkingFX.Stop();
                if (isWalking)
                {
                    SetWalkingToIdle();
                }
                else if (isRunning)
                {
                    SetRunningToIdle();
                }
                else 
                {
                    SetIdleState();
                }
                
            }
            else
            {
                walkingFX.Play();
                if (!isFootstepPlaying) 
                {
                    PlayFootstep();
                }
                if (playerMovement.moveSpeed == playerMovement.walkSpeed && isDashing == false)
                    SetWalkingState();
                else if (isRunningInput && isDashing == false)
                    SetRunningState();
            }
        }

        if (!playerDash.CanDash(elapsedTime))
        {
            inactiveDashFX.Play();
        }
        else
        {
            inactiveDashFX.Stop();
        }

        if (playerInput.GetDashInput() && playerDash.CanDash(elapsedTime))
        {
            isDashing = true;
            playerDash.InitiateDash(moveDirection, elapsedTime);
            playerAnimations.SetDashAnimation();
            //dashDelayTimer = dashDelayDuration;
            isRunning = false;
            isWalking = false;
            StopFootsteps();

        }
        if (playerAnimations.esk.IsAnimationFinished() && isDashing == true)
        {
            playerAnimations.SetStandardIdleAnimation();
            isDashing = false;
        }

        if (isFlashingColor)
        {
            flashTimer -= deltaTime;
            if (flashTimer <= 0f)
            {
                MeshRenderer renderer = playerMesh.GetComponent<MeshRenderer>();
                if (renderer != null)
                {
                    renderer.SetColor(originalColor);
                }
                isFlashingColor = false;
            }
        }
    }


    private void SetIdleState()
    {
        if (!isIdle)
        {
            if (isShootingStanding)
            {
                playerAnimations.SetShootingStandingToIdleAnimation();
            }

            playerAnimations.SetIdleRandomAnimation();
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
        if (isShootInput && !isShootingRunning)
        {
            TransitionShootingStandingToRunning();
        }
        else if (isShootingRunning && !isShootInput)
        {
            playerAnimations.SetShootingRunningToRunAnimation();
            isRunning = false;
            isWalking = false;
            isIdle = false;
            isShootingStanding = false;
            isShootingRunning = false;
            isMoving = true;
            isTransitioning = true;
            transitionTimer = transitionDelay;
            PlayFootstep();
        }
        else if (transitionTimer <= 0f && !isWalking)
        {
            playerAnimations.SetWalkAnimation();
            isRunning = false;
            isWalking = true;
            isIdle = false;
            isShootingStanding = false;
            isShootingRunning = false;
            isMoving = true;
            isTransitioning = false;
            PlayFootstep();
        }
    }

    private void SetRunningState()
    {
        if (isShootInput && !isShootingRunning)
        {
            TransitionShootingStandingToRunning();
        }
        else if (isShootingRunning && !isShootInput)
        {
            playerAnimations.SetShootingRunningToRunAnimation();
            isRunning = false;
            isWalking = false;
            isIdle = false;
            isShootingStanding = false;
            isShootingRunning = false;
            isMoving = true;
            isTransitioning = true;
            transitionTimer = transitionDelay;
            PlayFootstep();
        }
        else if (transitionTimer <= 0f && !isRunning)
        {
            playerAnimations.SetRunAnimation();
            isRunning = true;
            isWalking = false;
            isShootingStanding = false;
            isShootingRunning = false;
            isMoving = true;
            isTransitioning = false;
            PlayFootstep();
        }
    }

    private void SetShootingState()
    {
        if (moveDirection != Vector3.Zero && !isShootingRunning)
        {
            playerAnimations.SetRunningToShootRunningAnimation();
            isShootingStanding = false;
            isShootingRunning = true;
            isFootstepPlaying = false;
            isWalking = false;
            isRunning = false;
            isIdle = false;
        }
        else if (!isShootingStanding && moveDirection == Vector3.Zero)
        {
            playerAnimations.SetShootingStandingAnimation();
            isShootingStanding = true;
            isShootingRunning = false;
            isFootstepPlaying = false;
            isWalking = false;
            isRunning = false;
            isIdle = false;

        }
        if (isShootingStanding && moveDirection != Vector3.Zero)
        {
            playerAnimations.SetShootingStandingToShootingRunAnimation();
            isShootingStanding = false;
            isShootingRunning = true;
        }
    }
    private void SetWalkingToIdle()
    {
        if (isWalking)
        {
            playerAnimations.SetWalkingToIdleAnimation();
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
        if (isRunning)
        {
            playerAnimations.SetRunningToIdleAnimation();
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
        playerAnimations.SetShootingStandingToShootingRunAnimation();
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

        if (!isFootstepPlaying || currentFootstep != newFootstep)
        {
            if (audioRun > 0) Audio.Stop(audioRun);
            if (audioWalk > 0) Audio.Stop(audioWalk);

            audioRun = 0;
            audioWalk = 0;

            if (isRunning)
            {
                audioRun = Audio.Play(Runfootsteps, true);
            }
            else
            {
                audioWalk = Audio.Play(Walkfootsteps, true);
            }

            isFootstepPlaying = (audioRun > 0 || audioWalk > 0);
            hasStoppedFootsteps = !isFootstepPlaying;
            currentFootstep = newFootstep;
        }
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
        MeshRenderer renderer = playerMesh.GetComponent<MeshRenderer>();
        if (renderer != null)
        {
            originalColor = renderer.GetColor();
            renderer.SetColor(color);
            isFlashingColor = true;
            flashTimer = duration;
        }
    }
    public override void OnTriggerEnter(GameObject other)
    {
        if (other.name == "Hurtbox")
        {
            if (playerDash.isInvulnerable)
            {
                playerShooting.CounterAttack(other.GetComponent<BulletData>().owner);
            }
        }
    }
    public override void OnCollisionEnter(GameObject other)
    {
        if (other.tag == "EnemyAttack")
        {
            //if (!playerDash.isInvulnerable && !playerData.GodMode)
            //{
            //    //playerData.TakeDamage(10);

            //    sound.LoadAudio(HitAudio);
            //    sound.Play(true);
     
            //    if (bloodSplashEffect != null)
            //    {
            //        bloodSplashEffect.EmitBurst(100);
            //    }
                
            //    if(playerData.GetHealth() <= 0)
            //    {
            //        playerAnimations.SetDeathAnimation();
            //        sound.LoadAudio(DeathAudio);
            //    }
            //    else
            //    {
            //        playerAnimations.SetHitIdleAnimation();
            //    }
            //}
            //else if (playerDash.isInvulnerable)
            //{
            //    playerShooting.CounterAttack(other.GetComponent<BulletData>().owner);
            //}
        }
        
    }
}
