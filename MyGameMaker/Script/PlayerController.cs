using System.Collections;
using System.Numerics;
using HawkEngine;
public class PlayerController : MonoBehaviour
{
    private PlayerInput playerInput;
    private PlayerMovement playerMovement;
    private PlayerDash playerDash;
    public PlayerShooting playerShooting;
    private PlayerAnimations playerAnimations;
    private GameObject playerMesh;
    private bool isIdle = false;
    private bool isRunning = false;
    private bool isWalking = false;
    private bool isShootingStanding = false;
    private bool isShootingRunning = false;
    private bool isFootstepPlaying = false;
    private bool hasStoppedFootsteps = false;
    private float elapsedTime = 0f;
    private bool isInteracting = false;
    private float dashDelayTimer = 0f; 
    private float dashDelayDuration = 0.45f;

    private Audio sound;
    private string footsteps = "Assets/Audio/SFX/Player/PlayerFootstep.wav";
    private bool isMoving = false; 

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
        gameObject.GetComponent<Transform>().SetPosition(0, 0, 0);
        playerData = new PlayerData();

        if (playerInput == null || playerMovement == null || playerDash == null || playerShooting == null || playerMesh == null)
        {
            Engineson.print("ERROR: PlayerController is missing required components!");
        }
    }

    public override void Start()
    {

    }

    public override void Update(float deltaTime)
    {
        dashDelayTimer -= deltaTime;

        Vector3 moveDirection = playerInput.GetCurrentMoveDirection();
        Vector3 lookDirection = playerInput.GetCurrentLookDirection();
        elapsedTime += deltaTime;
        playerMovement.SetMoveDirection(moveDirection);
        playerMovement.SetLookDirection(lookDirection);

        if (moveDirection == Vector3.Zero && !playerInput.IsShooting())
        {
            SetIdleState();
            StopFootsteps();
        }
        else
        {
            if (dashDelayTimer <= 0f) 
            {
                if (moveDirection != Vector3.Zero && !playerInput.IsShooting())
                {
                    PlayFootstep();
                    if (playerMovement.moveSpeed == playerMovement.walkSpeed)
                    {
                        SetWalkingState();
                    }
                    else if (playerMovement.moveSpeed == playerMovement.runSpeed)
                    {
                        SetRunningState();
                    }
                }

                if (playerInput.IsShooting())
                {
                    SetShootingState();
                }
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
    }

    private void SetIdleState()
    {
        if (!isIdle)
        {
            playerAnimations.SetStandardIdleAnimation();
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
        if (!isWalking)
        {
            playerAnimations.SetWalkAnimation();
            isWalking = true;
            isRunning = false;
            isIdle = false;
            isMoving = true;
            PlayFootstep();
        }
    }

    private void SetRunningState()
    {
        if (!isRunning)
        {
            playerAnimations.SetRunAnimation();
            isRunning = true;
            isWalking = false;
            isIdle = false;
            isMoving = true;
            PlayFootstep();
        }
    }

    private void SetShootingState()
    {
        if (!isShootingRunning && isMoving)
        {
            playerAnimations.SetShootingRunningAnimation();
            isShootingStanding = false;
            isShootingRunning = true;
        }
        else if (!isShootingStanding && !isMoving)
        {
            playerAnimations.SetShootingStandingAnimation();
            isShootingStanding = true;
            isShootingRunning = false;
        }
    }

    private void PlayFootstep()
    {
        if (!isFootstepPlaying)
        {
            sound?.LoadAudio(footsteps);
            sound?.Play(true);
            isFootstepPlaying = true;
            hasStoppedFootsteps = false; 
        }
    }

    private void StopFootsteps()
    {
        if (isFootstepPlaying)
        {
            sound?.Stop();
            isFootstepPlaying = false;
            hasStoppedFootsteps = true;
        }
    }


    public override void OnCollisionEnter(GameObject other)
    {
        if (other.tag == "EnemyAttack")
        {
            if (!playerDash.isInvulnerable)
            {
                playerData.TakeDamage(10);
                Engineson.print($"Player took damage! Health: {playerData.GetHealth()}");
            }
            else if (playerDash.isInvulnerable)
            {
                playerShooting.CounterAttack(other.GetComponent<BulletData>().owner);
            }
        }

        if (other.tag == "Enemy")
        {
            if (!playerDash.isInvulnerable)
            {
                playerData.TakeDamage(10);
                Engineson.print($"Player took damage! Health: {playerData.GetHealth()}");
            }
            else if (playerDash.isInvulnerable)
            {
                playerShooting.CounterAttack(other);
            }
        }
    }
}
