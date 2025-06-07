using HawkEngine;

public class DoorAnimation : MonoBehaviour
{
    private SkeletalAnimation doorAnimation;
    public bool isAnimFinished = false;
    public override void Awake()
    {
    }
    public override void Start()
    {
        doorAnimation = gameObject.GetComponent<SkeletalAnimation>();
        if (doorAnimation == null)
        {
            Engineson.print("ERROR: SpikeAnimation requires a SkeletalAnimation component!");
            return;
        }
        doorAnimation.SetLoop(false);

    }

    public override void Update(float deltaTime)
    {
        if (doorAnimation != null)
        {
            float length = doorAnimation.GetAnimationLength();
            if (length <= 0.0f)
            {
                Engineson.print("ERROR: Animation length is 0.0f");
            }
        }
        if (doorAnimation.IsAnimationFinished() && !isAnimFinished)
        {
            Engineson.print("FinishAnim");
            isAnimFinished = true;
        }
    }

    public void SetCloseAnimation()
    {
        doorAnimation?.SetAnimationPlayState(true);
        if (doorAnimation?.GetAnimationIndex() != 0)
        {
            doorAnimation?.PlayAnimOnce(0, 0.1f);
            doorAnimation?.SetAnimationSpeed(1.0f);
            isAnimFinished = false;
        }
    }

    public void SetOpenAnimation()
    {
        doorAnimation?.SetAnimationPlayState(true);
        if (doorAnimation?.GetAnimationIndex() != 1)
        {
            doorAnimation?.PlayAnimOnce(1, 0.1f);
            doorAnimation?.SetAnimationSpeed(1.0f);
            isAnimFinished = false;
        }
    }
}