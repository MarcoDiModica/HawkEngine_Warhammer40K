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
    private bool isShootingStanding = false;
    private bool isShootingRunning = false;
    private bool hasStoppedFootsteps = false;
    private float elapsedTime = 0f;
    private bool isInteracting = false;

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
        //gameObject.GetComponent<Transform>().SetPosition(0, 0, 0);
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

        if (Input.GetControllerButtonDown(ControllerButton.Y))
        {
            Vector3 playerCenterPosition = gameObject.GetComponent<Transform>().GetPosition();
            playerCenterPosition.Y += 1;
            Vector3 playerDirection = gameObject.GetComponent<Transform>().forward;

            RayCast rayCast = new RayCast();
            rayCast.PerformRaycast(playerCenterPosition, playerDirection, 10f);

            if (rayCast.hit.isHit)
            {
                Engineson.print("Raycast hit: " + rayCast.hit.gameObject.name);
                Engineson.print("Raycast distance: " + rayCast.hit.distance);
                Engineson.print("Raycast normal: " + rayCast.hit.normal);
                Engineson.print("Raycast point: " + rayCast.hit.point);
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


        if (playerInput.IsDashPressed() && playerDash.CanDash(elapsedTime))
        {
            playerDash.InitiateDash(moveDirection, elapsedTime);
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