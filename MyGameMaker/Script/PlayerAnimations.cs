using System;
using System.Numerics;
using HawkEngine;

public class PlayerAnimations : MonoBehaviour
{
    public SkeletalAnimation esk;
    private PlayerController playerController;
    private PlayerData playerData;
    int animIndex = 0;

    public override void Awake()
    {

    }
    public override void Start()
    {
        esk = gameObject.GetComponent<SkeletalAnimation>();
        playerController = GameObject.Find("Player").GetComponent<PlayerController>();
        playerData = playerController.playerData;
        animIndex = esk.GetAnimationIndex();
    }

    public override void Update(float deltaTime)
    {

        if (Input.GetKeyDown(KeyCode.B))
        {
            if(playerData.GetHealth() <= 0)
            {
                animIndex += 1;
                SetDeathAnimation();
            }
            else
            {
                animIndex += 1;

                SetHitIdleAnimation();
            }
            
        }
    }
    public void SetHitIdleAnimation()
    {
        esk.TransitionAnimations(2, 5, 0.1f);
    }
    public void SetDeathAnimation()
    {
        esk.PlayAnimOnce(1, 0.1f);
        esk.SetAnimationSpeed(1.5f);
    }
    public void SetStandardIdleAnimation()
    {

        esk.TransitionAnimations(5, 5, 0.1f);
        esk.SetAnimationSpeed(2f);
      
    }

    public void SetIdleRandomAnimation()
    {
        if (esk.GetAnimationTime() >= esk.GetAnimationLength() - 0.2f)
        {
            Random rand = new Random();
            int idleIndex = rand.Next(4, 5);
            esk.SetAnimation(idleIndex);
            esk.SetAnimationSpeed(2f);
        }
        
    }

    public void SetShootingStandingAnimation()
    {
        esk.TransitionAnimations(5, 7, 0.1f);
        esk.SetAnimationSpeed(5f);
    }

    public void SetRunningToShootRunningAnimation()
    {
        esk.TransitionAnimations(6, 8, 0.1f);
        esk.SetAnimationSpeed(3f);
    }
    public void SetWalkAnimation()  
    {
        esk.TransitionAnimations(5, 9, 0.1f);
        esk.SetAnimationSpeed(2.0f);

    }
    public void SetRunAnimation()
    {
        esk.TransitionAnimations(9, 6, 0.1f);
        esk.SetAnimationSpeed(2.0f);
    }

    public void SetDashAnimation()
    {
        esk.PlayAnimOnce(0, 0.1f);
        esk.SetAnimationSpeed(2.5f);
    }

    public void SetShootingStandingToIdleAnimation()
    {
        esk.TransitionAnimations(7, 5, 0.1f);
        esk.SetAnimationSpeed(1.5f);

    }
    public void SetRunningToIdleAnimation()
    {
        esk.TransitionAnimations(6, 5, 0.1f);
        esk.SetAnimationSpeed(1.5f);

    }
    public void SetWalkingToIdleAnimation()
    {
        esk.TransitionAnimations(9, 5, 0.1f);
        esk.SetAnimationSpeed(1.5f);

    }
    public void SetShootingRunningToRunAnimation()
    {
        esk.TransitionAnimations(8, 6, 0.1f);
        esk.SetAnimationSpeed(1.5f);

    }
    public void SetShootingStandingToShootingRunAnimation()
    {
        esk.TransitionAnimations(7, 8, 0.1f);
        esk.SetAnimationSpeed(1.5f);

    }
}