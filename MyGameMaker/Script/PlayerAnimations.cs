using System;
using System.Numerics;
using HawkEngine;

public class PlayerAnimations : MonoBehaviour
{
    private SkeletalAnimation esk;
    int animIndex = 0;

    public override void Awake()
    {

    }
    public override void Start()
    {
        esk = gameObject.GetComponent<SkeletalAnimation>();
        if (esk == null)
        {
            Engineson.print("ERROR: PlayerAnimation requires a SkeletalAnimation component!");
            return;
        }

        animIndex = esk.GetAnimationIndex();
    }

    public override void Update(float deltaTime)
    {
        if (Input.GetKeyDown(KeyCode.B))
        {
            animIndex += 1;
            esk.TransitionAnimations(0,5,0.5f);
        }
        //if (esk.GetAnimationTime() >= esk.GetAnimationLength())
        //{
        //    SetRunAnimation();
        //}
    }

    public void SetStandardIdleAnimation()
    {

        esk.TransitionAnimations(5, 5, 0.1f);
        //esk.SetAnimation(4);
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
        //esk.SetAnimation(8);
        esk.SetAnimationSpeed(5f);
    }

    public void SetRunningToShootRunningAnimation()
    {
        esk.TransitionAnimations(6, 8, 0.1f);
        //esk.SetAnimation(10);
        esk.SetAnimationSpeed(1f);
    }
    public void SetWalkAnimation()
    {
        esk.TransitionAnimations(5, 9, 0.1f);
        //esk.SetAnimation(6);
        esk.SetAnimationSpeed(1.5f);

    }
    public void SetRunAnimation()
    {
        esk.TransitionAnimations(9, 6, 0.1f);
        //esk.SetAnimation(6);
        esk.SetAnimationSpeed(1.5f);
    }

    public void SetDashAnimation()
    {
        esk.TransitionAnimations(6, 0, 0.1f);
        //esk.SetAnimation(6);
        esk.SetAnimationSpeed(1.5f);
        
    }

    public void SetShootingStandingToIdleAnimation()
    {
        esk.TransitionAnimations(7, 5, 0.1f);
        //esk.SetAnimation(6);
        esk.SetAnimationSpeed(1.5f);

    }
    public void SetRunningToIdleAnimation()
    {
        esk.TransitionAnimations(6, 5, 0.1f);
        //esk.SetAnimation(6);
        esk.SetAnimationSpeed(1.5f);

    }
    public void SetWalkingToIdleAnimation()
    {
        esk.TransitionAnimations(9, 5, 0.1f);
        //esk.SetAnimation(6);
        esk.SetAnimationSpeed(1.5f);

    }
    public void SetShootingRunningToRunAnimation()
    {
        esk.TransitionAnimations(8, 6, 0.1f);
        //esk.SetAnimation(6);
        esk.SetAnimationSpeed(1.5f);

    }
    public void SetShootingStandingToShootingRunAnimation()
    {
        esk.TransitionAnimations(7, 6, 0.1f);
        //esk.SetAnimation(6);
        esk.SetAnimationSpeed(1.5f);

    }
}