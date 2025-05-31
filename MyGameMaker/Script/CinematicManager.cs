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

    private bool hasTransitioned = false;

    public override void Start()
    {
        imageCooldown = 0.041f;
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

        if (Input.GetKeyDown(KeyCode.RETURN) || Input.GetControllerButtonDown(ControllerButton.Start))
        {
            if(currentFrame < totalFrames - 45)
            {
                isPlaying = false;
                onCinematicEndAction.Invoke();
            }       
        }

        timer += deltaTime;
        if (timer >= imageCooldown)
        {
            timer = 0f;
            currentFrame++;

            if (currentFrame > totalFrames-15)
            {
                if (currentFrame > totalFrames)
                {
                    isPlaying = false;
                   
                    return;
                }
                // load fadeToBlack
                if (!hasTransitioned)
                {
                    onCinematicEndAction.Invoke();
                    hasTransitioned = true;
                }
            }

            if (ShouldFade(currentFrame))
            {
                fadeController.FadeToBlackHoldAndBack(0.4f, 0.2f);
            }
            if (currentFrame == 135 || currentFrame == 227)
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
