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
        spikeAnimation.SetLoop(false);
        
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
        if (spikeAnimation.IsAnimationFinished() && !isAnimFinished)
        {
            Engineson.print("FinishAnim");
            isAnimFinished = true;
        }
    }

    public void SetBurrowAnimation()
    {
        spikeAnimation?.SetAnimationPlayState(true);
        if (spikeAnimation?.GetAnimationIndex() != 0)
        {
            spikeAnimation?.PlayAnimOnce(0, 0.1f);
            spikeAnimation?.SetAnimationSpeed(1.0f);
            isAnimFinished = false;
        }
    }

    public void SetUnburrowAnimation()
    {
        spikeAnimation?.SetAnimationPlayState(true);
        if (spikeAnimation?.GetAnimationIndex() != 1)
        {
            spikeAnimation?.PlayAnimOnce(1, 0.1f);
            spikeAnimation?.SetAnimationSpeed(1.0f);
            isAnimFinished = false;
        }
    }
}