using System;
using System.Numerics;
using HawkEngine;

public class PlayerAnimations : MonoBehaviour
{
    public SkeletalAnimation esk;
    private PlayerController playerController;
    private PlayerData playerData;
    public int animIndex = 0;
    private bool componentsInitialized = false;

    public override void Awake()
    {
        esk = gameObject.GetComponent<SkeletalAnimation>();
        if (esk == null)
        {
            Engineson.print("ERROR: SkeletalAnimation component not found on player mesh");
            return;
        }

        animIndex = esk.GetAnimationIndex();
        componentsInitialized = true;
    }

    public override void Start()
    {
        if (!componentsInitialized)
            return;

        GameObject playerObj = GameObject.Find("Player");
        if (playerObj != null)
        {
            playerController = playerObj.GetComponent<PlayerController>();
            if (playerController != null && playerController.playerData != null)
            {
                playerData = playerController.playerData;
            }
            else
            {
                Engineson.print("WARNING: PlayerController or PlayerData not found");
            }
        }
        else
        {
            Engineson.print("ERROR: Player GameObject not found");
        }
    }

    public override void Update(float deltaTime)
    {
        if (!componentsInitialized)
            return;

        if (Input.GetKeyDown(KeyCode.B))
        {
            if (playerData != null)
            {
                playerData.isHit = true;
            }
        }
    }

    public void SetStandardIdleAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(11, 11, 0.1f);
            esk.SetAnimationSpeed(2f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetStandardIdleAnimation: {e.Message}");
        }
    }

    public void SetIdleRandomAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            float animTime = esk.GetAnimationTime();
            float animLength = esk.GetAnimationLength();

            if (animTime >= animLength - 0.2f)
            {
                Engineson.print("Idle Random Animation");
                Random rand = new Random();
                int idleIndex = rand.Next(0, 2);
                if (idleIndex == 0)
                {
                    idleIndex = 11; // Idle animation
                }
                else if (idleIndex == 1)
                {
                    idleIndex = 13; // Idle animation
                }
                esk.SetAnimation(idleIndex);
                esk.SetAnimationSpeed(2f);
            }
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetIdleRandomAnimation: {e.Message}");
        }
    }

    public void IdleToAimAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(11, 12, 0.1f);
            esk.SetAnimationSpeed(2f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetStandardIdleAnimation: {e.Message}");
        }
    }

    public void IdleToWalkingAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(11, 41, 0.1f);
            esk.SetAnimationSpeed(2f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetStandardIdleAnimation: {e.Message}");
        }
    }

    public void IdleToShootingStillAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(11, 33, 0.1f);
            esk.SetAnimationSpeed(2f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetStandardIdleAnimation: {e.Message}");
        }
    }

    public void ReloadBoltgunIdleToIdleAnim()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(21, 11, 0.1f);
            esk.SetAnimationSpeed(2f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetStandardIdleAnimation: {e.Message}");
        }
    }

    public void ReloadBoltgunIdleToShootingStandingAnim()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(21, 33, 0.1f);
            esk.SetAnimationSpeed(2f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetStandardIdleAnimation: {e.Message}");
        }
    }

    public void ReloadBoltgunRunningToRunningAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(20, 32, 0.1f);
            esk.SetAnimationSpeed(2f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetStandardIdleAnimation: {e.Message}");
        }
    }

    public void ReloadBoltgunWalkingToWalkingAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(23, 41, 0.1f);
            esk.SetAnimationSpeed(2f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetStandardIdleAnimation: {e.Message}");
        }
    }

    public void ReloadBoltgunWalkingToShootingWalkingAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(23, 37, 0.1f);
            esk.SetAnimationSpeed(2f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetStandardIdleAnimation: {e.Message}");
        }
    }

    public void ReloadShotgunIdleToIdleAnim()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(27, 11, 0.1f);
            esk.SetAnimationSpeed(2f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetStandardIdleAnimation: {e.Message}");
        }
    }

    public void ReloadShotgunIdleToShootingStandingAnim()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(27, 33, 0.1f);
            esk.SetAnimationSpeed(2f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetStandardIdleAnimation: {e.Message}");
        }
    }

    public void ReloadShotgunRunningToRunningAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(21, 32, 0.1f);
            esk.SetAnimationSpeed(2f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetStandardIdleAnimation: {e.Message}");
        }
    }

    public void ReloadShotgunWalkingToWalkingAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(29, 41, 0.1f);
            esk.SetAnimationSpeed(2f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetStandardIdleAnimation: {e.Message}");
        }
    }

    public void ReloadShotgunWalkingToShootingWalkingAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(29, 37, 0.1f);
            esk.SetAnimationSpeed(2f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetStandardIdleAnimation: {e.Message}");
        }
    }

    public void WalkingToIdleAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(41, 11, 0.1f);
            esk.SetAnimationSpeed(2f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetStandardIdleAnimation: {e.Message}");
        }
    }

    public void RunToIdleAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(32, 11, 0.1f);
            esk.SetAnimationSpeed(2f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetStandardIdleAnimation: {e.Message}");
        }
    }

    public void SetDashAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.PlayAnimOnce(6, 0.1f);
            esk.SetAnimationSpeed(5f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetDeathAnimation: {e.Message}");
        }
    }

    public void SetDeathAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.PlayAnimOnce(7, 0.1f);
            esk.SetAnimationSpeed(2.5f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetDeathAnimation: {e.Message}");
        }
    }

    public void IdleToRunAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(11, 32, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingStillToIdleAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(33, 11, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingStillToRunAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(33, 32, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingWalkingBackwardsToRunAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(34, 32, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingWalkingLeftToRunAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(35, 32, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingWalkingRightToRunAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(36, 32, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingWalkingStraightToRunAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(37, 32, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WalkingBackToRunAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(38, 32, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WalkingLeftToRunAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(39, 32, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WalkingRightToRunAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(40, 32, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WalkingStraightToRunAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(41, 32, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void RunningToIdleAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(32, 11, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void RunningToShootingStandingAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(32, 33, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void RunningToShootingWalkingBackwardsAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(32, 34, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void RunningToShootingWalkingLeftAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(32, 35, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void RunningToShootingWalkingRightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(32, 36, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void RunningToShootingWalkingStraightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(32, 37, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void RunningToWalkingBackAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(32, 38, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void RunningToWalkingLeftAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(32, 39, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void RunningToWalkingRightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(32, 40, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void RunningToWalkingStraightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(32, 41, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingWalkingBackwardsToShootingStandingAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(34, 33, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingWalkingLeftToShootingStandingAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(35, 33, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingWalkingRightToShootingStandingAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(36, 33, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingWalkingStraightToShootingStandingAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(37, 33, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WalkingBackToShootingStandingAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(38, 33, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WalkingLeftToShootingStandingAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(39, 33, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WalkingRightToShootingStandingAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(40, 33, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WalkingStraightToShootingStandingAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(41, 33, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingStandingToShootingWalkingBackwardsAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(33, 34, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingStandingToShootingWalkingLeftAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(33, 35, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingStandingToShootingWalkingRightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(33, 36, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingStandingToShootingWalkingStraightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(33, 37, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingStandingToWalkingBackAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(33, 38, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingStandingToWalkingLeftAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(33, 39, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingStandingToWalkingRightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(33, 40, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingStandingToWalkingStraightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(33, 41, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingWalkingLeftToShootingWalkingBackwardsAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(35, 34, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingWalkingRightToShootingWalkingBackwardsAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(36, 34, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingWalkingStraightToShootingWalkingBackwardsAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(37, 34, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WalkingBackToShootingWalkingBackwardsAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(38, 34, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WalkingLeftToShootingWalkingBackwardsAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(39, 34, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WalkingRightToShootingWalkingBackwardsAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(40, 34, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WalkingStraightToShootingWalkingBackwardsAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(41, 34, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingWalkingBackwardsToShootingWalkingLeftAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(34, 35, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingWalkingBackwardsToShootingWalkingRightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(34, 36, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingWalkingBackwardsToShootingWalkingStraightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(34, 37, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingWalkingBackwardsToWalkingBackAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(34, 38, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingWalkingBackwardsToWalkingLeftAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(34, 39, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingWalkingBackwardsToWalkingRightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(34, 40, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingWalkingBackwardsToWalkingStraightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(34, 41, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingWalkingRightToShootingWalkingLeftAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(36, 35, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingWalkingStraightToShootingWalkingLeftAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(37, 35, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WalkingBackwardsToShootingWalkingLeftAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(38, 35, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WalkingLeftToShootingWalkingLeftAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(39, 35, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WalkingRightToShootingWalkingLeftAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(40, 35, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WalkingStraightToShootingWalkingLeftAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(41, 35, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingWalkingLeftToShootingWalkingRightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(35, 36, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingWalkingLeftToShootingWalkingStraightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(35, 37, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingWalkingLeftToWalkingBackAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(35, 38, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingWalkingLeftToWalkingLeftAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(35, 39, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingWalkingLeftToWalkingRightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(35, 40, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingWalkingLeftToWalkingStraightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(35, 41, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingWalkingStraightToShootingWalkingRightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(37, 36, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WalkingBackwardsToShootingWalkingRightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(38, 36, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WalkingLeftToShootingWalkingRightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(39, 36, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WalkingRightToShootingWalkingRightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(40, 36, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WalkingStraightToShootingWalkingRightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(41, 36, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingWalkingRightToShootingWalkingStraightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(36, 37, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingWalkingRightToWalkingBackAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(36, 38, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingWalkingRightToWalkingLeftAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(36, 39, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingWalkingRightToWalkingRightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(36, 40, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingWalkingRightToWalkingStraightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(36, 41, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WalkingBackwardsToShootingWalkingStraightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(38, 37, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WalkingLeftToShootingWalkingStraightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(39, 37, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WalkingRightToShootingWalkingStraightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(40, 37, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WalkingStraightToShootingWalkingStraightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(41, 37, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingWalkingStraightToWalkingBackAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(37, 38, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingWalkingStraightToWalkingLeftAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(37, 39, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingWalkingStraightToWalkingRightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(37, 40, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingWalkingStraightToWalkingStraightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(37, 41, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WalkingLeftToWalkingBackwardsAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(39, 38, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WalkingRightToWalkingBackwardsAnimation()
    {

        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(40, 38, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WalkingStraightToWalkingBackwardsAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(41, 38, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WalkingBackwardsToWalkingLeftAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(38, 39, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WalkingBackwardsToWalkingRightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(38, 40, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WalkingBackwardsToWalkingStraightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(38, 41, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WalkingRightToWalkingLeftAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(40, 39, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WalkingStraightToWalkingLeftAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(41, 39, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WalkingLeftToWalkingRightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(39, 40, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WalkingLeftToWalkingStraightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(39, 41, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WalkingStraightToWalkingRightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(41, 40, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WalkingRightToWalkingStraightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(40, 41, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void IdleToHitAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.PlayAnimOnce(10, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void RunningToHitAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.PlayAnimOnce(8, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingStandingToHitAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.PlayAnimOnce(9, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingWalkingBackwardsToHitAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(34, 9, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingWalkingLeftToHitAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(35, 9, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingWalkingRightToHitAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(36, 9, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ShootingWalkingStraightToHitAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(37, 9, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WalkingBackwardsToHitAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(38, 8, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WalkingLeftToHitAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(39, 8, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WalkingRightToHitAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(40, 8, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WalkingStraightToHitAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(41, 8, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ActivateBlackRageWhileRunningAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.PlayAnimOnce(0, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ActiveBlackRageToRunningAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(0, 32, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ActivateBlackRageWhileIdleAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.PlayAnimOnce(1, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ActiveBlackRageToIdleAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(1, 11, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ActivateBlackRageWhileWalkingBackwardsAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.PlayAnimOnce(2, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ActiveBlackRageToWalkingBackwardsAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(2, 34, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ActivateBlackRageWhileWalkingLeftAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.PlayAnimOnce(3, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ActiveBlackRageToWalkingLeftAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(3, 35, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ActivateBlackRageWhileWalkingRightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.PlayAnimOnce(4, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ActiveBlackRageToWalkingRightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(4, 36, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ActivateBlackRageWhileWalkingStraightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.PlayAnimOnce(5, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ActiveBlackRageToWalkingForwardAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(5, 37, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void OverheatWhileRunningAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(32, 14, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void OverheatWhileIdleAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(11, 15, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void OverheatWhileWalkingBackwardsAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(38, 16, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void OverheatWhileWalkingForwardAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(41, 17, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void OverheatWhileWalkingLeftAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(39, 18, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void OverheatWhileWalkingRightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(40, 19, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    

    public void ReloadBoltgunWhileRunningAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.PlayAnimOnce(20, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ReloadBoltgunWhileIdleAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.PlayAnimOnce(21, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ReloadBoltgunWhileWalkingBackwardsAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(38, 22, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ReloadBoltgunWhileWalkingForwardAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.PlayAnimOnce(23, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ReloadBoltgunWhileWalkingLeftAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(39, 24, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ReloadBoltgunWhileWalkingRightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(40, 25, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ReloadShotgunWhileRunningAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.PlayAnimOnce(26, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ReloadShotgunWhileIdleAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.PlayAnimOnce(27, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ReloadShotgunWhileWalkingBackwardsAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(38, 28, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ReloadShotgunWhileWalkingForwardAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.PlayAnimOnce(29, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ReloadShotgunWhileWalkingLeftAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(39, 30, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void ReloadShotgunWhileWalkingRightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(40, 31, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WeaponSwapWhileRunningAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.PlayAnimOnce(42, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WeaponSwapWhileIdleAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.PlayAnimOnce(43, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WeaponSwapWhileWalkingBackwardsAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.PlayAnimOnce(44, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WeaponSwapWhileWalkingLeftAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.PlayAnimOnce(45, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WeaponSwapWhileWalkingRightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.PlayAnimOnce(46, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WeaponSwapWhileWalkingStraightAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.PlayAnimOnce(47, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WeaponSwapIdleToIdleAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(43, 11, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WeaponSwapShootingToShootingAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(43, 33, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WeaponSwapRunningToRunningAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(42, 32, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WeaponSwapWalkingToWalkingAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(47, 41, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }

    public void WeaponSwapWalkingShootingToWalkingShootingAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(47, 37, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningAnimation: {e.Message}");
        }
    }


    //public void SetHitAnimation()
    //{
    //    if (!componentsInitialized || esk == null)
    //        return;

    //    esk.TransitionAnimations(2, 11, 0.1f);
    //}

    //public void SetHitRunningAnimation()
    //{
    //    if (!componentsInitialized || esk == null)
    //        return;

    //    esk.TransitionAnimations(2, 6, 0.1f);
    //}

    //public void SetHitShootingRunningAnimation()
    //{
    //    if (!componentsInitialized || esk == null)
    //        return;

    //    esk.TransitionAnimations(2, 8, 0.1f);
    //}

    //public void SetHitShootingStandingAnimation()
    //{
    //    if (!componentsInitialized || esk == null)
    //        return;

    //    esk.TransitionAnimations(2, 8, 0.1f);
    //}

    //public void SetHitWalkingAnimation()
    //{
    //    if (!componentsInitialized || esk == null)
    //        return;

    //    esk.TransitionAnimations(2, 9, 0.1f);
    //}

    //public void SetShootingStandingAnimation()
    //{
    //    if (!componentsInitialized || esk == null)
    //        return;

    //    try
    //    {
    //        esk.TransitionAnimations(11, 7, 0.1f);
    //        esk.SetAnimationSpeed(5f);
    //    }
    //    catch (Exception e)
    //    {
    //        Engineson.print($"ERROR in SetShootingStandingAnimation: {e.Message}");
    //    }
    //}

    //public void SetRunningToShootRunningAnimation()
    //{
    //    if (!componentsInitialized || esk == null)
    //        return;

    //    try
    //    {
    //        esk.TransitionAnimations(6, 8, 0.1f);
    //        esk.SetAnimationSpeed(3f);
    //    }
    //    catch (Exception e)
    //    {
    //        Engineson.print($"ERROR in SetRunningToShootRunningAnimation: {e.Message}");
    //    }
    //}

    //public void SetWalkAnimation()
    //{
    //    if (!componentsInitialized || esk == null)
    //        return;

    //    try
    //    {
    //        esk.TransitionAnimations(11, 9, 0.1f);
    //        esk.SetAnimationSpeed(2.0f);
    //    }
    //    catch (Exception e)
    //    {
    //        Engineson.print($"ERROR in SetWalkAnimation: {e.Message}");
    //    }
    //}

    //public void SetRunAnimation()
    //{
    //    if (!componentsInitialized || esk == null)
    //        return;

    //    try
    //    {
    //        esk.TransitionAnimations(9, 6, 0.1f);
    //        esk.SetAnimationSpeed(2.0f);
    //    }
    //    catch (Exception e)
    //    {
    //        Engineson.print($"ERROR in SetRunAnimation: {e.Message}");
    //    }
    //}

    ////-----------DASHING ANIMATION----------------//

    public void SetDashToRunningAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(6, 32, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetDashToRunningAnimation: {e.Message}");
        }
    }

    public void SetDashToShootingRunningAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(6, 37, 0.1f);
            esk.SetAnimationSpeed(1.5f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetDashToShootingRunningAnimation: {e.Message}");
        }
    }

    public void SetDashToShootingStandingAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(6, 33, 0.1f);
            esk.SetAnimationSpeed(1.5f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetDashToShootingStandingAnimation: {e.Message}");
        }
    }

    public void SetDashToWalkingAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(6, 41, 0.1f);
            esk.SetAnimationSpeed(1.5f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetDashToWalkingAnimation: {e.Message}");
        }
    }

    public void SetDashToIdleAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(6, 11, 0.1f);
            esk.SetAnimationSpeed(1.5f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetDashToIdleAnimation: {e.Message}");
        }
    }

    //public void SetShootingStandingToIdleAnimation()
    //{
    //    if (!componentsInitialized || esk == null)
    //        return;

    //    try
    //    {
    //        esk.TransitionAnimations(7, 11, 0.1f);
    //        esk.SetAnimationSpeed(1.5f);
    //    }
    //    catch (Exception e)
    //    {
    //        Engineson.print($"ERROR in SetShootingStandingToIdleAnimation: {e.Message}");
    //    }
    //}

    //public void SetRunningToIdleAnimation()
    //{
    //    if (!componentsInitialized || esk == null)
    //        return;

    //    try
    //    {
    //        esk.TransitionAnimations(6, 11, 0.1f);
    //        esk.SetAnimationSpeed(1.5f);
    //    }
    //    catch (Exception e)
    //    {
    //        Engineson.print($"ERROR in SetRunningToIdleAnimation: {e.Message}");
    //    }
    //}

    //public void SetWalkingToIdleAnimation()
    //{
    //    if (!componentsInitialized || esk == null)
    //        return;

    //    try
    //    {
    //        esk.TransitionAnimations(9, 11, 0.1f);
    //        esk.SetAnimationSpeed(1.5f);
    //    }
    //    catch (Exception e)
    //    {
    //        Engineson.print($"ERROR in SetWalkingToIdleAnimation: {e.Message}");
    //    }
    //}

    //public void SetShootingRunningToRunAnimation()
    //{
    //    if (!componentsInitialized || esk == null)
    //        return;

    //    try
    //    {
    //        esk.TransitionAnimations(8, 6, 0.1f);
    //        esk.SetAnimationSpeed(1.5f);
    //    }
    //    catch (Exception e)
    //    {
    //        Engineson.print($"ERROR in SetShootingRunningToRunAnimation: {e.Message}");
    //    }
    //}

    //public void SetShootingStandingToShootingRunAnimation()
    //{
    //    if (!componentsInitialized || esk == null)
    //        return;

    //    try
    //    {
    //        esk.TransitionAnimations(7, 8, 0.1f);
    //        esk.SetAnimationSpeed(1.5f);
    //    }
    //    catch (Exception e)
    //    {
    //        Engineson.print($"ERROR in SetShootingStandingToShootingRunAnimation: {e.Message}");
    //    }
    //}


}