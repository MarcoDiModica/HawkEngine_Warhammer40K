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

        if (mawlocTailesk?.GetAnimationIndex() >= mawlocTailesk?.GetAnimationLength() - 0.25f && !isAnimFinished)
        {
            mawlocTailesk?.SetAnimationPlayState(false);
            isAnimFinished = true;
        }
    }

    public void SetBurrowingAnimation()
    {
        if (mawlocTailesk?.GetAnimationIndex() != 3)
        {
            mawlocTailesk?.SetAnimationPlayState(true);
            mawlocTailesk?.SetAnimation(3);
            mawlocTailesk?.SetAnimationSpeed(1.0f);
            isAnimFinished = false;
        }
    }

    public void SetUnburrowingAnimation()
    {
        if (mawlocTailesk?.GetAnimationIndex() != 0)
        {
            mawlocTailesk?.SetAnimationPlayState(true);
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
}