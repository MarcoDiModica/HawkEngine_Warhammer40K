using System;
using System.Numerics;
using HawkEngine;

public class HormagauntAnimation : MonoBehaviour
{
    private SkeletalAnimation hormagauntesk;
    int animIndex = 0;

    public override void Awake()
    {
        
    }
    public override void Start()
    {
        hormagauntesk = gameObject.GetComponent<SkeletalAnimation>();
        if (hormagauntesk == null)
        {
            Engineson.print("ERROR: PlayerAnimation requires a SkeletalAnimation component!");
            return;
        }
        hormagauntesk.SetAnimation(14);

    }

    public override void Update(float deltaTime)
    {
        if (hormagauntesk.GetAnimationIndex() != 14 || hormagauntesk.GetAnimationIndex() != 19)
        {
            if (hormagauntesk.GetAnimationTime() >= hormagauntesk.GetAnimationLength() - 1.0f)
            {
                hormagauntesk.SetAnimationPlayState(false);
            }
        }
    }

    public void SetStandardIdleAnimation()
    {
        hormagauntesk.SetAnimationPlayState(true);
        if (hormagauntesk.GetAnimationIndex() != 14)
        {
            hormagauntesk.SetAnimation(14);
            hormagauntesk.SetAnimationSpeed(1.0f);
        }
    }

    public void SetRandomAttackAnimation()
    {
        hormagauntesk.SetAnimationPlayState(true);
        Random rand = new Random();
        int attackIndex = rand.Next(1, 4);
        switch (attackIndex)
        {
            case 1:
                hormagauntesk.SetAnimation(1);
                //hormagauntesk.TransitionAnimations(15, 1, 0.1f);
                hormagauntesk.SetAnimationSpeed(1.0f);
                break;
            case 2:
                hormagauntesk.SetAnimation(2);
                //hormagauntesk.TransitionAnimations(15, 2, 0.1f);
                hormagauntesk.SetAnimationSpeed(1.0f);
                break;
            case 3:
                hormagauntesk.SetAnimation(3);
                //hormagauntesk.TransitionAnimations(15, 3, 0.1f);
                hormagauntesk.SetAnimationSpeed(1.0f);
                break;
        }
    }

    public void SetDeathAnimation()
    {
        if (hormagauntesk.GetAnimationIndex() != 8)
        {
            hormagauntesk.SetAnimationPlayState(true);
            hormagauntesk.SetAnimation(8);
            hormagauntesk.SetAnimationSpeed(0.9f);
        }
    }

    public void SetRunningAnimation()
    {
        hormagauntesk.SetAnimationPlayState(true);
        if (hormagauntesk.GetAnimationIndex() != 19)
        {
            hormagauntesk.SetAnimation(19);
            hormagauntesk.SetAnimationSpeed(1.0f);
        }
    }

    public void SetStartLeapAnimation()
    {
        hormagauntesk.SetAnimationPlayState(true);
        if (hormagauntesk.GetAnimationIndex() != 16)
        {
            hormagauntesk.SetAnimation(16);
            hormagauntesk.SetAnimationSpeed(1.0f);
        }
    }

    public void SetFlyingLeapAnimation()
    {
        hormagauntesk.SetAnimationPlayState(true);
        if (hormagauntesk.GetAnimationIndex() != 17)
        {
            hormagauntesk.SetAnimation(17);
            hormagauntesk.SetAnimationSpeed(1.0f);
        }
    }

    public void SetLandingLeapAnimation()
    {
        hormagauntesk.SetAnimationPlayState(true);
        if (hormagauntesk.GetAnimationIndex() != 18)
        {
            hormagauntesk.SetAnimation(18);
            hormagauntesk.SetAnimationSpeed(1.0f);
        }
    }

    public void SetWholeLeapAnimation()
    {
        hormagauntesk.SetAnimationPlayState(true);
        if (hormagauntesk.GetAnimationIndex() != 15)
        {
            hormagauntesk.SetAnimation(15);
            hormagauntesk.SetAnimationSpeed(1.5f);
        }
    }
    public void SetHitAnimation()
    {
        hormagauntesk.SetAnimationPlayState(true);
        Random rand = new Random();
        int hitIndex = rand.Next(1, 4);
        switch (hitIndex)
        {
            case 1:
                hormagauntesk.SetAnimation(11);
                //hormagauntesk.TransitionAnimations(15, 1, 0.1f);
                hormagauntesk.SetAnimationSpeed(1.0f);
                break;
            case 2:
                hormagauntesk.SetAnimation(12);
                //hormagauntesk.TransitionAnimations(15, 2, 0.1f);
                hormagauntesk.SetAnimationSpeed(1.0f);
                break;
            case 3:
                hormagauntesk.SetAnimation(13);
                //hormagauntesk.TransitionAnimations(15, 3, 0.1f);
                hormagauntesk.SetAnimationSpeed(1.0f);
                break;
        }
    }
}