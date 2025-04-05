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
    private bool isIdle = false;
    public bool isRunning = false;
    private bool isWalking = false;
    private bool isMoving = false;
    private bool isShootingStanding = false;
    private bool isShootingRunning = false;
    private bool isTransitioning = false;
    private float transitionTimer = 0f;
    private float transitionDelay = 0.1f;
    Vector3 moveDirection;
    private bool once = false;

    private float elapsedTime = 0f;
    private bool isInteracting = false;
    private float dashDelayTimer = 0f; 
    private float dashDelayDuration = 0.45f;

    private Audio sound;
    private bool isFootstepPlaying = false;
    private bool hasStoppedFootsteps = false;
    private string Runfootsteps = "Assets/Audio/SFX/Player/PlayerFootstep.wav";
    private string Walkfootsteps = "Assets/Audio/SFX/Player/PlayerWalkFootstep.wav";
    public string HitAudio = "Assets/Audio/SFX/Player/PlayerHit.wav";
    public string DeathAudio = "Assets/Audio/SFX/Player/PlayerDeath.wav";


    public PlayerData playerData;

    public override void Awake()
    {
        playerInput = gameObject.GetComponent<PlayerInput>();
        playerMovement = gameObject.GetComponent<PlayerMovement>();
        playerDash = gameObject.GetComponent<PlayerDash>();
        playerShooting = gameObject.GetComponent<PlayerShooting>();
        playerMesh = GameObject.Find("playerMesh");
        playerAnimations = playerMesh.GetComponent<PlayerAnimations>();
        playerMesh.GetComponent<SkeletalAnimation>().SetAnimationSpeed(2f);
        sound = gameObject.GetComponent<Audio>();
        //gameObject.GetComponent<Transform>().SetPosition(0, 0, 0);
        playerData = new PlayerData();
        // Add the blood splash effect directly to the player object
        bloodSplashEffect = gameObject.AddComponent<ParticleFX>();
        bloodSplashEffect.ApplyPreset(19); // BLOOD_SPLASH preset (index 19)

    }

    public override void Start()
    {
        gameObject.tag = "Player";
        bloodSplashEffect.ApplyPreset(19);
    }

    public override void Update(float deltaTime)
    {

        //upon pressing B take 10 damage
        if (Input.GetKeyDown(KeyCode.B))
        {
            playerData.TakeDamage(10);
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
        elapsedTime += deltaTime;
        playerMovement.SetMoveDirection(moveDirection);
        playerMovement.SetLookDirection(lookDirection);

        if (dashDelayTimer > 0f)
        {
            return;
        }
        if (playerInput.IsShooting())
        {
            SetShootingState();
            isIdle = false;
        }
        else
        {

           
            if (moveDirection == Vector3.Zero)
            {
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
                StopFootsteps();
            }
            else
            {
                if (!isFootstepPlaying) 
                {
                    PlayFootstep();
                }
                if (playerMovement.moveSpeed == playerMovement.walkSpeed)
                    SetWalkingState();
                else if (playerMovement.moveSpeed == playerMovement.runSpeed)
                    SetRunningState();
            }
        }

        if (playerInput.IsDashPressed() && playerDash.CanDash(elapsedTime))
        {
            playerDash.InitiateDash(moveDirection, elapsedTime);
            playerAnimations.SetDashAnimation();
            dashDelayTimer = dashDelayDuration;
            isRunning = false;
            isWalking = false;
            StopFootsteps();
        }
        if (isTransitioning && transitionTimer > 0f)
        {
            transitionTimer -= deltaTime;

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
            sound?.Stop();
            hasStoppedFootsteps = true;
            isFootstepPlaying = false;
        }
    }

    private void SetWalkingState()
    {
        if (playerInput.IsShooting() && !isShootingRunning)
        {
            TransitionShootingStandingToRunning();
        }
        else if (isShootingRunning && !playerInput.IsShooting())
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
        if (playerInput.IsShooting() && !isShootingRunning)
        {
            TransitionShootingStandingToRunning();
        }
        else if (isShootingRunning && !playerInput.IsShooting())
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
            sound?.Stop(); 
            sound?.LoadAudio(newFootstep);
            sound?.Play(true); 
            isFootstepPlaying = true;
            hasStoppedFootsteps = false;
            currentFootstep = newFootstep; 
        }
    }

    private void StopFootsteps()
    {
        if (isFootstepPlaying)
        {
            sound?.Stop();
            isFootstepPlaying = false;
            hasStoppedFootsteps = true;
            currentFootstep = ""; 
        }
    }

    public override void OnTriggerEnter(GameObject other)
    {
        if (other.name == "Hurtbox")
        {
            if (!playerDash.isInvulnerable && !playerData.GodMode)
            {
                sound.LoadAudio(HitAudio);
                sound.Play(true);

                if (bloodSplashEffect != null)
                {
                    bloodSplashEffect.EmitBurst(100);
                }

                if (playerData.GetHealth() <= 0)
                {
                    playerAnimations.SetDeathAnimation();
                    sound.LoadAudio(DeathAudio);
                    sound.Play(true);
                }
                else
                {
                    playerAnimations.SetHitIdleAnimation();
                }
            }
            else if (playerDash.isInvulnerable)
            {
                playerShooting.CounterAttack(other.GetComponent<BulletData>().owner);
            }
        }
    }
    public override void OnCollisionEnter(GameObject other)
    {
        if (other.tag == "EnemyAttack")
        {
            if (!playerDash.isInvulnerable && !playerData.GodMode)
            {
                //playerData.TakeDamage(10);

                sound.LoadAudio(HitAudio);
                sound.Play(true);
     
                if (bloodSplashEffect != null)
                {
                    bloodSplashEffect.EmitBurst(100);
                }
                
                if(playerData.GetHealth() <= 0)
                {
                    playerAnimations.SetDeathAnimation();
                    sound.LoadAudio(DeathAudio);
                }
                else
                {
                    playerAnimations.SetHitIdleAnimation();
                }
            }
            else if (playerDash.isInvulnerable)
            {
                playerShooting.CounterAttack(other.GetComponent<BulletData>().owner);
            }
        }
        
    }
}
