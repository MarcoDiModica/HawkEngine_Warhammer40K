using System;
using System.Numerics;
using HawkEngine;

public class LictorAnimation : MonoBehaviour
{
    private SkeletalAnimation lictorAnimation;
    private int animIndex = 0;
    public bool isAnimFinished = false;
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
    }
    public override void Update(float deltaTime)
    {
        if (lictorAnimation.GetAnimationIndex() != 2)
        {
            if (lictorAnimation.GetAnimationTime() >= lictorAnimation.GetAnimationLength() - 1.0f)
            {
                lictorAnimation.SetAnimationPlayState(false);
                isAnimFinished = true;
            }
        }
    }

    public void SetCrossSlashAnimation()
    {
        lictorAnimation.SetAnimationPlayState(true);
        isAnimFinished = false;
        if (lictorAnimation.GetAnimationIndex() != 0)
        {
            lictorAnimation.SetAnimation(0);
            lictorAnimation.SetAnimationSpeed(1.0f);
        }
    }

    public void SetDefeatAnimation()
    {
        isAnimFinished = false;
        if (lictorAnimation.GetAnimationIndex() != 1)
        {
            lictorAnimation.SetAnimationPlayState(true);
            lictorAnimation.SetAnimation(1);
            lictorAnimation.SetAnimationSpeed(1.0f);
        }
    }

    public void SetIdleAnimation()
    {
        lictorAnimation.SetAnimationPlayState(true);
        isAnimFinished = false;
        if (lictorAnimation.GetAnimationIndex() != 2)
        {
            lictorAnimation.SetAnimation(2);
            lictorAnimation.SetAnimationSpeed(1.0f);
        }
    }

    public void SetLeapAnimation()
    {
        lictorAnimation.SetAnimationPlayState(true);
        isAnimFinished = false;
        if (lictorAnimation.GetAnimationIndex() != 3)
        {
            lictorAnimation.SetAnimation(3);
            lictorAnimation.SetAnimationSpeed(1.0f);
        }
    }
    public void SetPiercingAnimation()
    {
        lictorAnimation.SetAnimationPlayState(true);
        isAnimFinished = false;
        if (lictorAnimation.GetAnimationIndex() != 4)
        {
            lictorAnimation.SetAnimation(4);
            lictorAnimation.SetAnimationSpeed(1.0f);
        }
    }

    public void SetRoarAnimation()
    {
        lictorAnimation.SetAnimationPlayState(true);
        if (lictorAnimation.GetAnimationIndex() != 5)
        {
            lictorAnimation.SetAnimation(5);
            lictorAnimation.SetAnimationSpeed(1.0f);
        }
    }

    public void SetStunnedAnimation()
    {
        lictorAnimation.SetAnimationPlayState(true);
        if (lictorAnimation.GetAnimationIndex() != 6)
        {
            lictorAnimation.SetAnimation(6);
            lictorAnimation.SetAnimationSpeed(1.0f);
        }
    }

    public void SetWalkAroundAnimation()
    {
        lictorAnimation.SetAnimationPlayState(true);
        if (lictorAnimation.GetAnimationIndex() != 7)
        {
            lictorAnimation.SetAnimation(7);
            lictorAnimation.SetAnimationSpeed(1.0f);
        }
    }

    public void SetWalkToPlayerAnimation()
    {
        lictorAnimation.SetAnimationPlayState(true);
        if (lictorAnimation.GetAnimationIndex() != 8)
        {
            lictorAnimation.SetAnimation(8);
            lictorAnimation.SetAnimationSpeed(1.0f);
        }
    }
}