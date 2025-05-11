using System;
using HawkEngine;

public class MawlocTailAnimation : MonoBehaviour
{
    private SkeletalAnimation mawlocTailesk;
    int animIndex = 0;
    public bool isAnimFinished = false;
    public override void Awake()
    {
    }
    public override void Start()
    {
        mawlocTailesk = gameObject.GetComponent<SkeletalAnimation>();
        if (mawlocTailesk == null)
        {
            Engineson.print("ERROR: MawlocTailAnimation requires a SkeletalAnimation component!");
            return;
        }
        mawlocTailesk?.SetAnimation(0);
    }

    public override void Update(float deltaTime)
    {
        if (mawlocTailesk != null)
        {
            float length = mawlocTailesk.GetAnimationLength();
            if (length <= 0.0f)
            {
                Engineson.print("ERROR: Animation length is 0.0f");
            }
        }

        //if (mawlocTailesk?.GetAnimationIndex() != 5)
        //{
        //    mawlocTailesk?.SetAnimationPlayState(false);
        //    isAnimFinished = true;
        //    Engineson.print("Animation finished");
        //}
    }

    public void SetBurrowingAnimation()
    {
        mawlocTailesk?.SetAnimationPlayState(true);
        if (mawlocTailesk?.GetAnimationIndex() != 3)
        {
            mawlocTailesk?.SetAnimation(3);
            mawlocTailesk?.SetAnimationSpeed(1.0f);
            isAnimFinished = false;
        }
    }

    public void SetUnburrowingAnimation()
    {
        mawlocTailesk?.SetAnimationPlayState(true);
        if (mawlocTailesk?.GetAnimationIndex() != 0)
        {
            mawlocTailesk?.SetAnimation(0);
            mawlocTailesk?.SetAnimationSpeed(1.0f);
            isAnimFinished = false;
        }
    }

    public void SetSlashAnimation()
    {
        mawlocTailesk?.SetAnimationPlayState(true);
        if (mawlocTailesk?.GetAnimationIndex() != 1)
        {
            mawlocTailesk?.SetAnimation(1);
            mawlocTailesk?.SetAnimationSpeed(1.0f);
            isAnimFinished = false;
        }
    }

    public void SetStabAnimation()
    {
        mawlocTailesk?.SetAnimationPlayState(true);
        if (mawlocTailesk?.GetAnimationIndex() != 2)
        {
            mawlocTailesk?.SetAnimation(2);
            mawlocTailesk?.SetAnimationSpeed(1.0f);
            isAnimFinished = false;
        }
    }

    public void SetDeathAnimation()
    {
        mawlocTailesk?.SetAnimationPlayState(true);
        if (mawlocTailesk?.GetAnimationIndex() != 4)
        {
            mawlocTailesk?.SetAnimation(4);
            mawlocTailesk?.SetAnimationSpeed(1.0f);
            isAnimFinished = false;
        }
    }

    public void SetIdleAnimation()
    {
        mawlocTailesk?.SetAnimationPlayState(true);
        if (mawlocTailesk?.GetAnimationIndex() != 5)
        {
            mawlocTailesk?.SetAnimation(5);
            mawlocTailesk?.SetAnimationSpeed(1.0f);
            isAnimFinished = false;
        }
    }
}