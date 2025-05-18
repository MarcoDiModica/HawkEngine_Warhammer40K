using System;
using HawkEngine;

public class WarriorAnimation : MonoBehaviour
{
    private SkeletalAnimation warrioresk;
    int animIndex = 0;
    public bool isAnimFinished = false;
    public override void Awake()
    {
    }
    public override void Start()
    {
        warrioresk = gameObject.GetComponent<SkeletalAnimation>();
        if (warrioresk == null)
        {
            Engineson.print("ERROR: WarriorAnimation requires a SkeletalAnimation component!");
            return;
        }
        warrioresk?.SetAnimation(0);
    }

    public override void Update(float deltaTime)
    {
        if (warrioresk != null)
        {
            float length = warrioresk.GetAnimationLength();
            if (length <= 0.0f)
            {
                Engineson.print("ERROR: Animation length is 0.0f");
            }
        }
        if (warrioresk?.GetAnimationIndex() != 3)
        {
            if (warrioresk?.GetAnimationTime() >= warrioresk?.GetAnimationLength() - 1.0f && !isAnimFinished)
            {
                warrioresk?.SetAnimationPlayState(false);
                isAnimFinished = true;
            }
        }
    }

    public void SetIdleAnimation()
    {
        warrioresk?.SetAnimationPlayState(true);
        if (warrioresk?.GetAnimationIndex() != 0)
        {
            warrioresk?.SetAnimation(0);
            warrioresk?.SetAnimationSpeed(1.0f);
            isAnimFinished = false;
        }
    }

    public void SetDeathAnimation()
    {
        if (warrioresk?.GetAnimationIndex() != 1)
        {
            warrioresk?.SetAnimationPlayState(true);
            warrioresk?.SetAnimation(1);
            warrioresk?.SetAnimationSpeed(1.0f);
            isAnimFinished = false;
        }
    }

    public void SetHurtAnimation()
    {
        warrioresk?.SetAnimationPlayState(true);
        if (warrioresk?.GetAnimationIndex() != 2)
        {
            warrioresk?.SetAnimation(2);
            warrioresk?.SetAnimationSpeed(1.0f);
            isAnimFinished = false;
        }
    }

    public void SetRunAnimation()
    {
        warrioresk?.SetAnimationPlayState(true);
        if (warrioresk?.GetAnimationIndex() != 3)
        {
            warrioresk?.SetAnimation(3);
            warrioresk?.SetAnimationSpeed(1.0f);
            isAnimFinished = false;
        }
    }

    public void SetMeleeAnimation()
    {
        warrioresk?.SetAnimationPlayState(true);
        if (warrioresk?.GetAnimationIndex() != 4)
        {
            warrioresk?.SetAnimation(4);
            warrioresk?.SetAnimationSpeed(1.0f);
            isAnimFinished = false;
        }
    }

    public void SetShootAnimation()
    {
        warrioresk?.SetAnimationPlayState(true);
        if (warrioresk?.GetAnimationIndex() != 5)
        {
            warrioresk?.SetAnimation(5);
            warrioresk?.SetAnimationSpeed(1.0f);
            isAnimFinished = false;
        }
    }
}