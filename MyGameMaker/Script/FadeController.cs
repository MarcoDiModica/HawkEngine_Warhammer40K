using HawkEngine;
using System;
using System.Numerics;

public class FadeController : MonoBehaviour
{
    private UIImage fadeImage;
    private float timer = 0f;
    private float duration = 1f;
    private bool fading = false;
    private bool fadeIn = true;
    private Action onComplete;

    private bool holding = false;
    private float holdDuration = 0f;
    private float holdTimer = 0f;
    private bool doubleFade = false;

    private bool hasCompletedDoubleFade = false;

    public override void Start()
    {
        fadeImage = gameObject.GetComponent<UIImage>();

        if (fadeImage == null)
        {
            Engineson.print("ERROR: FadeController needs a UIImage on the same GameObject.");
            return;
        }

        SetAlpha(0f);
    }

    public override void Update(float deltaTime)
    {
        //debug
        //if (Input.GetKey(KeyCode.F11))
        //{
        //    SceneManager.LoadSceneWithFade("BetaRelease_Week1_Lvl2");
        //}
        //if (Input.GetKey(KeyCode.F10))
        //{
        //    FadeToBlackHoldAndBack(1f, 2f);
        //}

        if (!fading) return;

        timer += deltaTime;
        float t = Mathf.Clamp01(timer / duration);
        float alpha = fadeIn ? t : 1f - t;
        SetAlpha(alpha);

        if (t >= 1f)
        {
            if (doubleFade && !holding && !hasCompletedDoubleFade)
            {
                holding = true;
                holdTimer = 0f;
                return;
            }

            if (holding)
            {
                holdTimer += deltaTime;
                if (holdTimer >= holdDuration)
                {
                    // Start fade back in (black to white)
                    fadeIn = false;
                    timer = 0f;
                    holding = false;
                    hasCompletedDoubleFade = true;
                    return;
                }
                return;
            }

            fading = false;
            doubleFade = false;
            hasCompletedDoubleFade = false;
            onComplete?.Invoke();
        }
    }


    public void StartFade(float duration, bool fadeIn, Action onComplete = null)
    {
        this.duration = duration;
        this.fadeIn = fadeIn;
        this.onComplete = onComplete;
        this.timer = 0f;
        this.fading = true;
        this.holding = false;
        this.doubleFade = false;

        fadeImage.SetImageEnabled(true);
    }

    public void FadeOut(float duration, Action onComplete = null)
    {
        SetAlpha(1f); // start full black
        StartFade(duration, false, onComplete);
    }

    public void FadeToBlackHoldAndBack(float duration, float holdTime, Action onComplete = null)
    {
        SetAlpha(1f);
        this.duration = duration;
        this.holdDuration = holdTime;
        this.fadeIn = true;
        this.onComplete = onComplete;
        this.timer = 0f;
        this.fading = true;
        this.holding = false;
        this.doubleFade = true;
        this.hasCompletedDoubleFade = false;

        fadeImage.SetImageEnabled(true);
    }

    private void SetAlpha(float alpha)
    {
        fadeImage.SetImageColor(new Vector4(0f, 0f, 0f, Mathf.Clamp01(alpha)));
    }
}
