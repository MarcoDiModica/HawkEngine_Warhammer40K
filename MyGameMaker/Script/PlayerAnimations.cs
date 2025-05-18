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

    public void SetHitAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        esk.TransitionAnimations(2, 5, 0.1f);
    }

    public void SetHitRunningAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        esk.TransitionAnimations(2, 6, 0.1f);
    }

    public void SetHitShootingRunningAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        esk.TransitionAnimations(2, 8, 0.1f);
    }

    public void SetHitShootingStandingAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        esk.TransitionAnimations(2, 8, 0.1f);
    }

    public void SetHitWalkingAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        esk.TransitionAnimations(2, 9, 0.1f);
    }

    public void SetDeathAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.PlayAnimOnce(1, 0.1f);
            esk.SetAnimationSpeed(1.5f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetDeathAnimation: {e.Message}");
        }
    }

    public void SetStandardIdleAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(5, 5, 0.1f);
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
                int idleIndex = rand.Next(4, 5);
                esk.SetAnimation(idleIndex);
                esk.SetAnimationSpeed(2f);
            }
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetIdleRandomAnimation: {e.Message}");
        }
    }

    public void SetShootingStandingAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(5, 7, 0.1f);
            esk.SetAnimationSpeed(5f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetShootingStandingAnimation: {e.Message}");
        }
    }

    public void SetRunningToShootRunningAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(6, 8, 0.1f);
            esk.SetAnimationSpeed(3f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningToShootRunningAnimation: {e.Message}");
        }
    }

    public void SetWalkAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(5, 9, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetWalkAnimation: {e.Message}");
        }
    }

    public void SetRunAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(9, 6, 0.1f);
            esk.SetAnimationSpeed(2.0f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunAnimation: {e.Message}");
        }
    }

    //-----------DASHING ANIMATION----------------//
    public void SetDashAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.PlayAnimOnce(0, 0.1f);
            esk.SetAnimationSpeed(2.5f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetDashAnimation: {e.Message}");
        }
    }

    public void SetDashToRunningAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(0, 6, 0.1f);
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
            esk.TransitionAnimations(0, 8, 0.1f);
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
            esk.TransitionAnimations(0, 7, 0.1f);
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
            esk.TransitionAnimations(0, 9, 0.1f);
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
            esk.TransitionAnimations(0, 5, 0.1f);
            esk.SetAnimationSpeed(1.5f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetDashToIdleAnimation: {e.Message}");
        }
    }

    public void SetShootingStandingToIdleAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(7, 5, 0.1f);
            esk.SetAnimationSpeed(1.5f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetShootingStandingToIdleAnimation: {e.Message}");
        }
    }

    public void SetRunningToIdleAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(6, 5, 0.1f);
            esk.SetAnimationSpeed(1.5f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetRunningToIdleAnimation: {e.Message}");
        }
    }

    public void SetWalkingToIdleAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(9, 5, 0.1f);
            esk.SetAnimationSpeed(1.5f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetWalkingToIdleAnimation: {e.Message}");
        }
    }

    public void SetShootingRunningToRunAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(8, 6, 0.1f);
            esk.SetAnimationSpeed(1.5f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetShootingRunningToRunAnimation: {e.Message}");
        }
    }

    public void SetShootingStandingToShootingRunAnimation()
    {
        if (!componentsInitialized || esk == null)
            return;

        try
        {
            esk.TransitionAnimations(7, 8, 0.1f);
            esk.SetAnimationSpeed(1.5f);
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in SetShootingStandingToShootingRunAnimation: {e.Message}");
        }
    }
}