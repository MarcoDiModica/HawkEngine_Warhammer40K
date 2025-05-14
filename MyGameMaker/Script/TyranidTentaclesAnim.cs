using HawkEngine;

public class TyranidTentaclesAnim : MonoBehaviour
{
    private SkeletalAnimation skeletalAnim;
    private bool isPlaying = false;
    private float animationTimer = 0.0f;
    private int currentAnim = -1;

    private float stunDuration = 0.4f; // stun anim duration
    private float hideDuration = 1f; // hide anim duration

    public override void Start()
    {
        skeletalAnim = gameObject.GetComponent<SkeletalAnimation>();
        if (skeletalAnim == null)
        {
            Engineson.print("ERROR: SkeletalAnimation not found on tentacles.");
            return;
        }

        skeletalAnim.SetAnimationPlayState(false);
    }

    public override void Update(float deltaTime)
    {
        if (!isPlaying || skeletalAnim == null)
            return;

        animationTimer += deltaTime;

        if (currentAnim == 0 && animationTimer >= stunDuration)
        {
            skeletalAnim.SetAnimation(1); // esconderse
            skeletalAnim.SetAnimationSpeed(1.0f);
            skeletalAnim.SetAnimationPlayState(false);
        }
        else if (currentAnim == 1 && animationTimer >= hideDuration)
        {
            skeletalAnim.SetAnimationPlayState(false);
            isPlaying = false;
            currentAnim = -1;
        }
    }

    public void PlayStunAnim()
    {
        if (skeletalAnim == null || currentAnim == 0) return;

        skeletalAnim.SetAnimation(0); // show/stun
        skeletalAnim.SetAnimationSpeed(1.0f);
        skeletalAnim.SetAnimationPlayState(true);
        isPlaying = true;
        animationTimer = 0.0f;
        currentAnim = 0;
    }

    public void PlayHideAnim()
    {
        if (skeletalAnim == null || currentAnim == 1) return;

        skeletalAnim.SetAnimation(1); // hide
        skeletalAnim.SetAnimationSpeed(1.0f);
        skeletalAnim.SetAnimationPlayState(true);
        isPlaying = true;
        animationTimer = 0.0f;
        currentAnim = 1;
    }
}
