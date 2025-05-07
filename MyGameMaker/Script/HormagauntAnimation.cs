using System;
using System.Numerics;
using HawkEngine;

public class HormagauntAnimation : MonoBehaviour
{
    private SkeletalAnimation hormagauntesk;
    int animIndex = 0;
    public bool isAnimFinished = false;

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
        hormagauntesk?.SetAnimation(10);

    }

    public override void Update(float deltaTime)
    {
        if (hormagauntesk?.GetAnimationIndex() != 10)
        {
            if (hormagauntesk?.GetAnimationTime() >= hormagauntesk?.GetAnimationLength() - 1.0f && !isAnimFinished)
            {
                hormagauntesk?.SetAnimationPlayState(false);
                isAnimFinished = true;
            }
        }
    }

    public void SetStandardIdleAnimation()
    {
        hormagauntesk?.SetAnimationPlayState(true);
        if (hormagauntesk?.GetAnimationIndex() != 10)
        {
            hormagauntesk?.SetAnimation(10);
            hormagauntesk?.SetAnimationSpeed(1.0f);
            isAnimFinished = false;
        }
    }

    public void SetRandomAttackAnimation()
    {
        hormagauntesk?.SetAnimationPlayState(true);
        Random rand = new Random();
        int attackIndex = rand.Next(1, 4);
        isAnimFinished = false;
        switch (attackIndex)
        {
            case 1:
                hormagauntesk?.SetAnimation(0);
                //hormagauntesk?.TransitionAnimations(15, 1, 0.1f);
                hormagauntesk?.SetAnimationSpeed(1.0f);
                break;
            case 2:
                hormagauntesk?.SetAnimation(1);
                //hormagauntesk?.TransitionAnimations(15, 2, 0.1f);
                hormagauntesk?.SetAnimationSpeed(1.0f);
                break;
            case 3:
                hormagauntesk?.SetAnimation(2);
                //hormagauntesk?.TransitionAnimations(15, 3, 0.1f);
                hormagauntesk?.SetAnimationSpeed(1.0f);
                break;
        }
    }

    public void SetDeathAnimation()
    {
        if (hormagauntesk?.GetAnimationIndex() != 5)
        {
            hormagauntesk?.SetAnimationPlayState(true);
            hormagauntesk?.SetAnimation(5);
            hormagauntesk?.SetAnimationSpeed(0.9f);
            isAnimFinished = false;
        }
    }

    public void SetRunningAnimation()
    {
        hormagauntesk?.SetAnimationPlayState(true);
        if (hormagauntesk?.GetAnimationIndex() != 15)
        {
            hormagauntesk?.SetAnimation(15);
            hormagauntesk?.SetAnimationSpeed(1.0f);
            isAnimFinished = false;
        }
    }

    public void SetStartLeapAnimation()
    {
        hormagauntesk?.SetAnimationPlayState(true);
        if (hormagauntesk?.GetAnimationIndex() != 12)
        {
            hormagauntesk?.SetAnimation(12);
            hormagauntesk?.SetAnimationSpeed(1.0f);
            isAnimFinished = false;
        }
    }

    public void SetFlyingLeapAnimation()
    {
        hormagauntesk?.SetAnimationPlayState(true);
        if (hormagauntesk?.GetAnimationIndex() != 13)
        {
            hormagauntesk?.SetAnimation(13);
            hormagauntesk?.SetAnimationSpeed(1.0f);
            isAnimFinished = false;
        }
    }

    public void SetLandingLeapAnimation()
    {
        hormagauntesk?.SetAnimationPlayState(true);
        if (hormagauntesk?.GetAnimationIndex() != 14)
        {
            hormagauntesk?.SetAnimation(14);
            hormagauntesk?.SetAnimationSpeed(1.0f);
            isAnimFinished = false;
        }
    }

    public void SetWholeLeapAnimation()
    {
        hormagauntesk?.SetAnimationPlayState(true);
        if (hormagauntesk?.GetAnimationIndex() != 11)
        {
            hormagauntesk?.SetAnimation(11);
            hormagauntesk?.SetAnimationSpeed(1.5f);
            isAnimFinished = false;
        }
    }
    public void SetHitAnimation()
    {
        hormagauntesk?.SetAnimationPlayState(true);
        Random rand = new Random();
        int hitIndex = rand.Next(1, 4);
        isAnimFinished = false;
        switch (hitIndex)
        {
            case 1:
                hormagauntesk?.SetAnimation(7);
                //hormagauntesk?.TransitionAnimations(15, 1, 0.1f);
                hormagauntesk?.SetAnimationSpeed(1.0f);
                break;
            case 2:
                hormagauntesk?.SetAnimation(8);
                //hormagauntesk?.TransitionAnimations(15, 2, 0.1f);
                hormagauntesk?.SetAnimationSpeed(1.0f);
                break;
            case 3:
                hormagauntesk?.SetAnimation(9);
                //hormagauntesk?.TransitionAnimations(15, 3, 0.1f);
                hormagauntesk?.SetAnimationSpeed(1.0f);
                break;
        }
    }
}