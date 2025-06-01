using HawkEngine;

public class SpikeAnimation : MonoBehaviour
{
    private SkeletalAnimation spikeAnimation;
    public bool isAnimFinished = false;
    public override void Awake()
    {
    }
    public override void Start()
    {
        spikeAnimation = gameObject.GetComponent<SkeletalAnimation>();
        if (spikeAnimation == null)
        {
            Engineson.print("ERROR: SpikeAnimation requires a SkeletalAnimation component!");
            return;
        }
        spikeAnimation.SetAnimationPlayState(false);
    }

    public override void Update(float deltaTime)
    {
        if (spikeAnimation != null)
        {
            float length = spikeAnimation.GetAnimationLength();
            if (length <= 0.0f)
            {
                Engineson.print("ERROR: Animation length is 0.0f");
            }
        }
        if (spikeAnimation?.GetAnimationIndex() >= spikeAnimation?.GetAnimationLength() - 0.25f && !isAnimFinished)
        {
            spikeAnimation?.SetAnimationPlayState(false);
            spikeAnimation?.SetLoop(false);
            isAnimFinished = true;
        }
    }

    public void SetBurrowAnimation()
    {
        spikeAnimation?.SetAnimationPlayState(true);
        if (spikeAnimation?.GetAnimationIndex() != 0)
        {
            spikeAnimation?.SetAnimation(0);
            spikeAnimation?.SetAnimationSpeed(1.0f);
            isAnimFinished = false;
        }
    }

    public void SetUnburrowAnimation()
    {
        spikeAnimation?.SetAnimationPlayState(true);
        if (spikeAnimation?.GetAnimationIndex() != 1)
        {
            spikeAnimation?.SetAnimation(1);
            spikeAnimation?.SetAnimationSpeed(1.0f);
            isAnimFinished = false;
        }
    }
}