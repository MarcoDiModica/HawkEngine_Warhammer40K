using System;
using System.Numerics;
using HawkEngine;

public class LictorAnimation : MonoBehaviour
{
    private SkeletalAnimation lictorAnimation;
    private int animIndex = 0;
    public override void Awake()
    {

    }

    public override void Start()
    {
        lictorAnimation = gameObject.GetComponent<SkeletalAnimation>();
        if (lictorAnimation == null)
        {
            Engineson.print("ERROR: LictorAnimation requires a SkeletalAnimation component!");
            return;
        }
        animIndex = lictorAnimation.GetAnimationIndex();
    }
    public override void Update(float deltaTime)
    {

    }

    public void SetCrossSlashAnimation()
    {
        if (lictorAnimation.GetAnimationIndex() != 0)
        {
            lictorAnimation.PlayAnimOnce(0, 0.2f);
            animIndex = 0;
        }
    }

    public void SetDefeatAnimation()
    {
        if (lictorAnimation.GetAnimationIndex() != 1)
        {
            lictorAnimation.PlayAnimOnce(1, 0.2f);
            animIndex = 1;
        }
    }

    public void SetIdleAnimation()
    {
        if (lictorAnimation.GetAnimationIndex() != 2)
        {
            lictorAnimation.TransitionAnimations(animIndex, 2, 0.2f);
            animIndex = 2;
        }
    }

    public void SetLeapAnimation()
    {
        if (lictorAnimation.GetAnimationIndex() != 3)
        {
            lictorAnimation.PlayAnimOnce(3, 0.2f);
            animIndex = 3;
        }
    }
    public void SetPiercingAnimation()
    {
        if (lictorAnimation.GetAnimationIndex() != 4)
        {
            lictorAnimation.PlayAnimOnce(4, 0.2f);
            animIndex = 4;
        }
    }

    public void SetRoarAnimation()
    {
        if (lictorAnimation.GetAnimationIndex() != 5)
        {
            lictorAnimation.PlayAnimOnce(5, 0.2f);
            animIndex = 5;
        }
    }

    public void SetStunnedAnimation()
    {
        if (lictorAnimation.GetAnimationIndex() != 6)
        {
            lictorAnimation.PlayAnimOnce(6, 0.2f);
            animIndex = 6;
        }
    }

    public void SetWalkAroundAnimation()
    {
        if (lictorAnimation.GetAnimationIndex() != 7)
        {
            lictorAnimation.TransitionAnimations(animIndex, 7, 0.2f);
            animIndex = 7;
        }
    }

    public void SetWalkToPlayerAnimation()
    {
        if (lictorAnimation.GetAnimationIndex() != 8)
        {
            lictorAnimation.TransitionAnimations(animIndex,8, 0.2f);
            animIndex = 8;
        }
    }
}