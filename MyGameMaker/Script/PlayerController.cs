using System.Collections;
using System.Numerics;
using HawkEngine;

public class PlayerController : MonoBehaviour
{
    private PlayerInput playerInput;
    private PlayerMovement playerMovement;
    private PlayerDash playerDash;
    private PlayerShooting playerShooting;
    private PlayerAnimations playerAnimations;
    private GameObject playerMesh;
    private float currentTime = 0;
    private float maxIdleTimer = 1.0f;
    private bool isIdle = false;
    private bool isRunning = false;
    private bool isShootingStanding = false;
    private bool isShootingRunning = false;
    private bool hasStoppedFootsteps = false;

    private Audio sound;
    private string footsteps = "Assets/Audio/SFX/Player/PlayerFootstep.wav";
    private bool isFootstepPlaying = false;

    public override void Start()
    {
        playerInput = gameObject.GetComponent<PlayerInput>();
        playerMovement = gameObject.GetComponent<PlayerMovement>();
        playerDash = gameObject.GetComponent<PlayerDash>();
        playerShooting = gameObject.GetComponent<PlayerShooting>();
        playerMesh = GameObject.Find("playerMesh");
        playerAnimations = playerMesh.GetComponent<PlayerAnimations>();
        playerMesh.GetComponent<SkeletalAnimation>().SetAnimationSpeed(2f);
        sound = gameObject.GetComponent<Audio>();

        if (playerInput == null || playerMovement == null || playerDash == null || playerShooting == null || playerMesh == null)
        {
            Engineson.print("ERROR: PlayerController is missing required components!");
        }
    }

    public override void Update(float deltaTime)
    {
        

        Vector3 moveDirection = playerInput.GetCurrentMoveDirection();
        Vector3 lookDirection = playerInput.GetCurrentLookDirection();

        playerMovement.SetMoveDirection(moveDirection);
        playerMovement.SetLookDirection(lookDirection);

        if (moveDirection == Vector3.Zero && !playerInput.IsShooting())
        {
            // Idle
            if (!isIdle)
            {
                playerAnimations.SetStandardIdleAnimation();
                isIdle = true;
            }
            playerAnimations.SetIdleRandomAnimation();
            
            isRunning = false;
            isShootingStanding = false;
            isShootingRunning = false;
            isFootstepPlaying = false;

            if (!hasStoppedFootsteps)
            {
                sound?.Stop();
                hasStoppedFootsteps = true;
            }


        }

        

        if (moveDirection != Vector3.Zero && !playerInput.IsShooting() && !isFootstepPlaying)
        {
            sound?.LoadAudio(footsteps);
            sound?.Play(true);
            isFootstepPlaying = true;
            hasStoppedFootsteps = false;
        }
        else if (playerInput.IsShooting())
        {
            isFootstepPlaying = false;
        }
        
     

        if (moveDirection == Vector3.Zero && playerInput.IsShooting() && !isShootingStanding)
        {
            // Shooting while standing
            playerAnimations.SetShootingStandingAnimation();
            isRunning = false;
            isShootingStanding = true;
            isShootingRunning = false;
            isIdle = false;
            
        }
        
        if (moveDirection != Vector3.Zero && !isRunning && !playerInput.IsShooting())
        {
            // Running
            playerAnimations.SetRunAnimation();
            isRunning = true;
            isShootingStanding = false;
            isShootingRunning = false;
            isIdle = false;
            
        }
        
        if (moveDirection != Vector3.Zero && !isShootingRunning && playerInput.IsShooting() /*&& !isShooting*/)
        {
            // Shooting while running
            playerAnimations.SetShootingRunningAnimation();
            isRunning = false;
            isShootingStanding = false;
            isShootingRunning = true;
            isIdle = false;
        }


        if (playerInput.IsDashPressed() && playerDash.CanDash() && moveDirection != Vector3.Zero)
        {
            playerDash.InitiateDash(moveDirection);
        }
    }

    
}