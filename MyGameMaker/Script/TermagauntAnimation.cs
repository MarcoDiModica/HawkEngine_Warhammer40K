using System;
using HawkEngine;

public class TermagauntAnimation : MonoBehaviour
{
    private SkeletalAnimation termagauntesk;
    int animIndex = 0;
    public bool isAnimFinished = false;

    public override void Awake()
    {

    }

    public override void Start()
    {
        termagauntesk = gameObject.GetComponent<SkeletalAnimation>();
        if (termagauntesk == null)
        {
            Engineson.print("ERROR: PlayerAnimation requires a SkeletalAnimation component!");
            return;
        }
    }

    public override void Update(float deltaTime)
    {
        if (SceneManager.isPaused)
        {
            if (termagauntesk?.GetAnimationIndex() != 8)
            {
                termagauntesk?.SetAnimation(8);
                termagauntesk?.SetAnimationSpeed(0.0f);
            }
            return;
        }

        if (termagauntesk != null)
        {
            float length = termagauntesk.GetAnimationLength();
            if (length <= 0.0f)
            {
                Engineson.print("ERROR: Animation length is 0.0f");
            }
        }

        if (termagauntesk?.GetAnimationIndex() != 8)
        {
            if (termagauntesk?.GetAnimationTime() >= termagauntesk?.GetAnimationLength() - 1.0f && !isAnimFinished)
            {
                termagauntesk?.SetAnimationPlayState(false);
                isAnimFinished = true;
            }
        }
    }

    public void SetStandardIdleAnimation()
    {
        termagauntesk?.SetAnimationPlayState(true);
        if (termagauntesk?.GetAnimationIndex() != 8)
        {
            termagauntesk?.SetAnimation(8);
            termagauntesk?.SetAnimationSpeed(1.0f);
            isAnimFinished = false;
        }
    }

    public void SetAttackAnimation()
    {
        termagauntesk?.SetAnimationPlayState(true);
        if (termagauntesk?.GetAnimationIndex() != 1)
        {
            termagauntesk?.SetAnimation(1);
            termagauntesk?.SetAnimationSpeed(1.0f);
            isAnimFinished = false;
        }
    }

    public void SetDeathAnimation()
    {
        termagauntesk?.SetAnimationPlayState(true);
        if (termagauntesk?.GetAnimationIndex() != 4)
        {
            termagauntesk?.SetAnimation(4);
            termagauntesk?.SetAnimationSpeed(1.0f);
            isAnimFinished = false;
        }
    }

    public void SetRunningAnimation()
    {
        termagauntesk?.SetAnimationPlayState(true);
        if (termagauntesk?.GetAnimationIndex() != 9)
        {
            termagauntesk?.SetAnimation(9);
            termagauntesk?.SetAnimationSpeed(1.0f);
            isAnimFinished = false;
        }
    }

    public void SetHitAnimation()
    {
        termagauntesk?.SetAnimationPlayState(true);
        if (termagauntesk?.GetAnimationIndex() != 5)
        {
            termagauntesk?.SetAnimation(5);
            termagauntesk?.SetAnimationSpeed(1.0f);
            isAnimFinished = false;
        }
    }
}