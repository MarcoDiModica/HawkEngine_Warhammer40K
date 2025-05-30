using HawkEngine;
using System;
using System.Numerics;

public class CinematicManager : MonoBehaviour
{
    public FadeController fadeController;
    public float imageCooldown = 0.03f;

    private UIImage uiImage;
    private int currentFrame = 1;
    private float timer = 0f;
    private bool isPlaying = false;

    private int totalFrames = 255;

    private Action onCinematicEndAction;

    public override void Start()
    {
        GameObject fadeObj = GameObject.Find("FadeController");
        fadeController = fadeObj?.GetComponent<FadeController>();
        if (fadeController == null)
        {
            Engineson.print("ERROR: Assign fadeController in the editor.");
            return;
        }

        uiImage = gameObject.GetComponent<UIImage>();
        if (uiImage == null)
        {
            Engineson.print("ERROR: UIImage not found on imageObject.");
            return;
        }

        gameObject.SetActive(false);
    }

    public override void Update(float deltaTime)
    {
        if (!isPlaying) return;

        timer += deltaTime;
        if (timer >= imageCooldown)
        {
            timer = 0f;
            currentFrame++;

            if (currentFrame > totalFrames)
            {
                isPlaying = false;
                onCinematicEndAction.Invoke();
                return;
            }

            if (ShouldFade(currentFrame))
            {
                fadeController.FadeToBlackHoldAndBack(0.5f, 0.4f);
            }
            if (currentFrame == 135)
            {
                currentFrame++; // Skip frame 135 because there is no image 
            }
            SetFrame(currentFrame);
        }
    }

    private void SetFrame(int frame)
    {
        string path = $"Assets/Textures/Animacion/{frame}.png";
        uiImage.SetImage(path);
    }

    private bool ShouldFade(int frame)
    {
        return frame == 160 || frame == 220;
    }

    private void SetAlpha(float alpha)
    {
        uiImage.SetImageColor(new Vector4(1f, 1f, 1f, Mathf.Clamp01(alpha)));
    }

    public void StartCinematic(Action onCinematicEndAction = null)
    {
        this.onCinematicEndAction = onCinematicEndAction;
        currentFrame = 1;
        gameObject.SetActive(true);
        uiImage.SetImageEnabled(true);
        SetAlpha(1f);
        SetFrame(currentFrame);
        isPlaying = true;
        timer = 0f;
    }
}
