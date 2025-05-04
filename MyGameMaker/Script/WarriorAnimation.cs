using HawkEngine;

public class WarriorAnimation : MonoBehaviour
{
    private SkeletalAnimation warriorAnimation;
    private int animIndex = 0;

    public override void Awake()
    {

    }

    public override void Start()
    {
        warriorAnimation = gameObject.GetComponent<SkeletalAnimation>();
        if (warriorAnimation == null)
        {
            Engineson.print("ERROR: WarriorAnimation requires a SkeletalAnimation component!");
            return;
        }
        animIndex = warriorAnimation.GetAnimationIndex();
    }

    public override void Update(float deltaTime)
    {
        if (warriorAnimation == null)
        {
            Engineson.print("Warning: WarriorAnimation lost reference to SkeletalAnimation during Update.");
            return;
        }
    }

    public void SetIdleAnimation()
    {
        if (warriorAnimation.GetAnimationIndex() != 0)
        {
            warriorAnimation.TransitionAnimations(animIndex, 0, 0.2f);
            animIndex = 0;
        }
    }

    public void SetDeathAnimation()
    {
        if (warriorAnimation.GetAnimationIndex() != 1)
        {
            warriorAnimation.PlayAnimOnce(1, 0.2f);
            animIndex = 1;
        }
    }

    public void SetHurtAnimation()
    {
        if (warriorAnimation.GetAnimationIndex() != 2)
        {
            warriorAnimation.PlayAnimOnce(2, 0.2f);
            animIndex = 2;
        }
    }

    public void SetRunAnimation()
    {
        if (warriorAnimation.GetAnimationIndex() != 3)
        {
            warriorAnimation.TransitionAnimations(animIndex, 3, 0.2f);
            animIndex = 3;
        }
    }

    public void SetMeleeAnimation()
    {
        if (warriorAnimation.GetAnimationIndex() != 4)
        {
            warriorAnimation.PlayAnimOnce(4, 0.2f);
            animIndex = 4;
        }
    }

    public void SetShootAnimation()
    {
        if (warriorAnimation.GetAnimationIndex() != 5)
        {
            warriorAnimation.PlayAnimOnce(5, 0.2f);
            animIndex = 5;
        }
    }

    public bool IsAnimationFinished()
    {
        return warriorAnimation.IsAnimationFinished();
    }
}