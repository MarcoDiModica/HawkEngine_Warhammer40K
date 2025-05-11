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
    }
}