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
        animIndex = 14;

    }

    public override void Update(float deltaTime)
    {

    }

    public void SetStandardIdleAnimation()
    {
        hormagauntesk.SetAnimationPlayState(true);
        if (hormagauntesk.GetAnimationIndex() != 12)
        {
            hormagauntesk.TransitionAnimations(animIndex, 12, 0.2f);
            animIndex = 12;
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
                hormagauntesk.PlayAnimOnce(0,0.2f);
                animIndex = 0;
                break;
            case 2:
                hormagauntesk.PlayAnimOnce(1,0.2f);
                animIndex = 1;
                break;
            case 3:
                hormagauntesk.PlayAnimOnce(2,0.2f);
                animIndex = 2;
                break;
        }
    }

    public void SetDeathAnimation()
    {
        if (hormagauntesk.GetAnimationIndex() != 5)
        {
            hormagauntesk.PlayAnimOnce(5, 0.2f);
            animIndex = 5;
        }
    }

    public void SetRunningAnimation()
    {
        hormagauntesk.SetAnimationPlayState(true);
        if (hormagauntesk.GetAnimationIndex() != 15)
        {
            hormagauntesk.TransitionAnimations(animIndex, 15, 0.2f);
            animIndex = 15;
        }
    }

    public void SetStartLeapAnimation()
    {
        hormagauntesk.SetAnimationPlayState(true);
        if (hormagauntesk.GetAnimationIndex() != 12)
        {
            hormagauntesk.PlayAnimOnce(12, 0.2f);
            animIndex = 12;
        }
    }

    public void SetFlyingLeapAnimation()
    {
        hormagauntesk.SetAnimationPlayState(true);
        if (hormagauntesk.GetAnimationIndex() != 13)
        {
            hormagauntesk.PlayAnimOnce(13, 0.2f);
            animIndex = 13;
        }
    }

    public void SetLandingLeapAnimation()
    {
        hormagauntesk.SetAnimationPlayState(true);
        if (hormagauntesk.GetAnimationIndex() != 14)
        {
            hormagauntesk.PlayAnimOnce(14, 0.2f);
            animIndex = 14;
        }
    }

    public void SetWholeLeapAnimation()
    {
        hormagauntesk.SetAnimationPlayState(true);
        if (hormagauntesk.GetAnimationIndex() != 11)
        {
            hormagauntesk.PlayAnimOnce(11, 0.2f);
            animIndex = 11;
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
                hormagauntesk.PlayAnimOnce(7, 0.2f);
                animIndex = 7;
                break;
            case 2:
                hormagauntesk.PlayAnimOnce(8, 0.2f);
                animIndex = 8;
                break;
            case 3:
                hormagauntesk.PlayAnimOnce(9, 0.2f);
                animIndex = 9;
                break;
        }
    }
}