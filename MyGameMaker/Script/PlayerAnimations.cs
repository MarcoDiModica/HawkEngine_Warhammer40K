using System;
using System.Numerics;
using HawkEngine;

public class PlayerAnimations : MonoBehaviour
{
    private SkeletalAnimation esk;
    int animIndex = 0;

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
    }

    public void SetStandardIdleAnimation()
    {

        esk.TransitionAnimations(6, 3, 0.1f);
        //esk.SetAnimation(4);
        esk.SetAnimationSpeed(2f);
    }

    public void SetIdleRandomAnimation()
    {
        if (esk.GetAnimationTime() >= esk.GetAnimationLength() - 0.2f)
        {
            Random rand = new Random();
            int idleIndex = rand.Next(2, 6);
            esk.SetAnimation(idleIndex);
            esk.SetAnimationSpeed(2f);
        }
        
    }

    public void SetShootingStandingAnimation()
    {
        esk.TransitionAnimations(3, 8, 0.1f);
        //esk.SetAnimation(8);
        esk.SetAnimationSpeed(3f);
    }

    public void SetShootingRunningAnimation()
    {
        esk.TransitionAnimations(6, 10, 0.1f);
        //esk.SetAnimation(10);
        esk.SetAnimationSpeed(4.5f);
    }

    public void SetRunAnimation()
    {
        esk.TransitionAnimations(3, 6, 0.1f);
        //esk.SetAnimation(6);
        esk.SetAnimationSpeed(4.5f);
    }
}