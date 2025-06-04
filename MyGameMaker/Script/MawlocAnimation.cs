using System;
using HawkEngine;

public class MawlocAnimation : MonoBehaviour
{
    private SkeletalAnimation mawlocesk;
    int animIndex = 0;
    public bool isAnimFinished = false;
    public override void Awake()
    {

    }
    public override void Start()
    {
        mawlocesk = gameObject.GetComponent<SkeletalAnimation>();
        if (mawlocesk == null)
        {
            Engineson.print("ERROR: MawlocAnimation requires a SkeletalAnimation component!");
            return;
        }
        mawlocesk?.SetAnimation(0);
    }

    public override void Update(float deltaTime)
    {
        if (mawlocesk != null)
        {
            float length = mawlocesk.GetAnimationLength();
            if (length <= 0.0f)
            {
                Engineson.print("ERROR: Animation length is 0.0f");
            }
        }

        if (mawlocesk?.GetAnimationIndex() != 3)
        {
            if (mawlocesk?.GetAnimationTime() >= mawlocesk?.GetAnimationLength() - 1.0f && !isAnimFinished)
            {
                mawlocesk?.SetAnimationPlayState(false);
                isAnimFinished = true;
                Engineson.print("Animation finished");
            }
        }
    }

    public void SetBurrowingAnimation()
    {
        mawlocesk?.SetAnimationPlayState(true);
        if (mawlocesk?.GetAnimationIndex() != 7)
        {
            mawlocesk?.SetAnimation(7);
            mawlocesk?.SetAnimationSpeed(1.0f);
            isAnimFinished = false;
        }
    }

    public void SetUnBurrowHeadAnimation()
    {
        mawlocesk?.SetAnimationPlayState(true);
        if (mawlocesk?.GetAnimationIndex() != 1)
        {
            mawlocesk?.SetAnimation(1);
            mawlocesk?.SetAnimationSpeed(1.0f);
            isAnimFinished = false;
        }
    }

    public void SetClawStrikeAnimation()
    {
        mawlocesk?.SetAnimationPlayState(true);
        if (mawlocesk?.GetAnimationIndex() != 2)
        {
            mawlocesk?.SetAnimation(2);
            mawlocesk?.SetAnimationSpeed(1.0f);
            isAnimFinished = false;
        }
    }

    public void SetDeathAnimation()
    {
        mawlocesk?.SetAnimationPlayState(true);
        if (mawlocesk?.GetAnimationIndex() != 3)
        {
            mawlocesk?.SetAnimationPlayState(true);

            mawlocesk?.PlayAnimOnceNoBlend(3); // Set to play once and not loop
            mawlocesk?.SetAnimationSpeed(1.0f);
            isAnimFinished = false;
        }
    }

    public void SetIdleAnimation()
    {
        mawlocesk?.SetAnimationPlayState(true);
        if (mawlocesk?.GetAnimationIndex() != 4)
        {
            mawlocesk?.SetAnimation(4);
            mawlocesk?.SetAnimationSpeed(1.0f);
            isAnimFinished = false;
        }
    }

    public void SetRoarAnimation()
    {
        mawlocesk?.SetAnimationPlayState(true);
        if (mawlocesk?.GetAnimationIndex() != 5)
        {
            mawlocesk?.SetAnimation(5);
            mawlocesk?.SetAnimationSpeed(1.0f);
            isAnimFinished = false;
        }
    }
    public void SetSlamAnimation()
    {
        mawlocesk?.SetAnimationPlayState(true);
        if (mawlocesk?.GetAnimationIndex() != 6)
        {
            mawlocesk?.SetAnimation(6);
            mawlocesk?.SetAnimationSpeed(1.0f);
            isAnimFinished = false;
        }
    }

    public void SetUnburrowingAnimation()
    {
        mawlocesk?.SetAnimationPlayState(true);
        if (mawlocesk?.GetAnimationIndex() != 0)
        {
            mawlocesk?.SetAnimation(0);
            mawlocesk?.SetAnimationSpeed(1.0f);
            isAnimFinished = false;
        }
    }
}