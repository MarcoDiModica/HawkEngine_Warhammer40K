using System;
using HawkEngine;

public class  TermagauntAnimation : MonoBehaviour
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
        if (termagauntesk != null)
        {
            float length = termagauntesk.GetAnimationLength();
            if (length <= 0.0f)
            {
                Engineson.print("ERROR: Animation length is 0.0f");
            }
        }

        if (termagauntesk?.GetAnimationIndex() != 10)
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
        if (termagauntesk?.GetAnimationIndex() != 10)
        {
            termagauntesk?.SetAnimation(10);
            termagauntesk?.SetAnimationSpeed(1.0f);
            isAnimFinished = false;
        }
    }
}