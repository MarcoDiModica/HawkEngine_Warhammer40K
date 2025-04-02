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
    private bool hasStoppedFootsteps = false;
    private float elapsedTime = 0f;
    private bool isInteracting = false;
    private float dashDelayTimer = 0f; 
    private float dashDelayDuration = 0.45f;

    private Audio sound;
    private string footsteps = "Assets/Audio/SFX/Player/PlayerFootstep.wav";
    private bool isFootstepPlaying = false;

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
            // Idle
            if (!isIdle)
            {
                playerAnimations.SetStandardIdleAnimation();
                isIdle = true;
            }
            playerAnimations.SetIdleRandomAnimation();

            if (dashDelayTimer <= 0f)
            {
                isRunning = false;
                isWalking = false;
            }

            isShootingStanding = false;
            isShootingRunning = false;
            isFootstepPlaying = false;

            if (!hasStoppedFootsteps)
            {
                sound?.Stop();
                hasStoppedFootsteps = true;
            }
        }
        else
        {
            if (moveDirection != Vector3.Zero && !isWalking && dashDelayTimer <= 0f && !playerInput.IsShooting() && playerMovement.moveSpeed == playerMovement.walkSpeed)
            {
                // Walking
               playerAnimations.SetWalkAnimation();
                isWalking = true;
                isShootingStanding = false;
                isShootingRunning = false;
                isIdle = false;
            }
            else if (moveDirection != Vector3.Zero && !isRunning && dashDelayTimer <= 0f && !playerInput.IsShooting() && playerMovement.moveSpeed == playerMovement.runSpeed)
            {
                // Running
                playerAnimations.SetRunAnimation();
                isRunning = true;
                isShootingStanding = false;
                isShootingRunning = false;
                isIdle = false;
            }
            else if (moveDirection != Vector3.Zero && !isShootingRunning && playerInput.IsShooting())
            {
                playerAnimations.SetShootingRunningAnimation();
                isRunning = false;
                isWalking = false;
                isShootingStanding = false;
                isShootingRunning = true;
                isIdle = false;
            }
        }

        if (playerInput.IsDashPressed() && playerDash.CanDash(elapsedTime))
        {
            playerDash.InitiateDash(moveDirection, elapsedTime);
            playerAnimations.SetDashAnimation();
            dashDelayTimer = dashDelayDuration;
            isRunning = false;
            isWalking = false;
        }

        if (dashDelayTimer <= 0f && isRunning && moveDirection == Vector3.Zero)
        {
            isRunning = false;
            isWalking = false;
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