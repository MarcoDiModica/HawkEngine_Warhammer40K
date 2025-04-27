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
        lictorAnimation.SetAnimation(2);
    }
    public override void Update(float deltaTime)
    {

    }

    public void SetCrossSlashAnimation()
    {
        if (lictorAnimation.GetAnimationIndex() != 0)
        {
            lictorAnimation.PlayAnimOnce(0, 1.0f);
        }
    }

    public void SetDefeatAnimation()
    {
        if (lictorAnimation.GetAnimationIndex() != 1)
        {
            lictorAnimation.PlayAnimOnce(1, 1.0f);
        }
    }

    public void SetIdleAnimation()
    {
        if (lictorAnimation.GetAnimationIndex() != 2)
        {
            lictorAnimation.SetAnimation(2);
            lictorAnimation.SetAnimationSpeed(1.0f);
        }
    }

    public void SetLeapAnimation()
    {
        if (lictorAnimation.GetAnimationIndex() != 3)
        {
            lictorAnimation.PlayAnimOnce(3, 1.0f);
        }
    }
    public void SetPiercingAnimation()
    {
        if (lictorAnimation.GetAnimationIndex() != 4)
        {
            lictorAnimation.PlayAnimOnce(4, 1.0f);
        }
    }

    public void SetRoarAnimation()
    {
        if (lictorAnimation.GetAnimationIndex() != 5)
        {
            lictorAnimation.PlayAnimOnce(5, 1.0f);
        }
    }

    public void SetStunnedAnimation()
    {
        if (lictorAnimation.GetAnimationIndex() != 6)
        {
            lictorAnimation.PlayAnimOnce(6, 1.0f);
        }
    }

    public void SetWalkAroundAnimation()
    {
        if (lictorAnimation.GetAnimationIndex() != 7)
        {
            lictorAnimation.SetAnimation(7);
            lictorAnimation.SetAnimationSpeed(1.0f);
        }
    }

    public void SetWalkToPlayerAnimation()
    {
        if (lictorAnimation.GetAnimationIndex() != 8)
        {
            lictorAnimation.SetAnimation(8);
            lictorAnimation.SetAnimationSpeed(1.0f);
        }
    }
}