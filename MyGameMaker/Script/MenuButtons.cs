using HawkEngine;
using System;
using System.Numerics;

public class MenuButtons : MonoBehaviour
{
    private UIButton newGameButton;
    private UIButton continueButton;
    private UIButton optionsButton;
    private UIButton creditsButton;
    private UIButton quitButton;

    //private Audio sound;
    private Audio sound;
    private string buttonHovered = "Assets/Audio/SFX/UI/ButtonSelected.wav";
    private string buttonClicked = "Assets/Audio/SFX/UI/ButtonPressed.wav";

    public override void Awake()
    {
        newGameButton = GameObject.Find("NewGameButton").GetComponent<UIButton>();
        continueButton = GameObject.Find("ContinueButton").GetComponent<UIButton>();
        optionsButton = GameObject.Find("OptionsButton").GetComponent<UIButton>();
        creditsButton = GameObject.Find("CreditsButton").GetComponent<UIButton>();
        quitButton = GameObject.Find("QuitButton").GetComponent<UIButton>();
        sound = gameObject.GetComponent<Audio>();
    }

    public override void Start()
    {
        if (newGameButton == null || optionsButton == null || creditsButton == null || quitButton == null)
        {
            Engineson.print("ERROR: No Button object found");
        }

        sound?.LoadAudio(buttonClicked);
    }

    public override void Update(float deltaTime)
    {
        if (newGameButton.GetState() == ButtonState.CLICKED)
        {
            sound?.Play();
            SceneManager.LoadScene("DefaultScene");
        }

        if (continueButton.GetState() == ButtonState.CLICKED)
        {
            sound?.Play();
            Engineson.print("Continue is Clicked");
        }

        if (optionsButton.GetState() == ButtonState.CLICKED)
        {
            sound?.Play();
            Engineson.print("Options is Clicked");
        }

        if (creditsButton.GetState() == ButtonState.CLICKED)
        {
            sound?.Play();
            Engineson.print("Credits is Clicked");
        }

        if (quitButton.GetState() == ButtonState.CLICKED)
        {
            sound?.Play();
            Engineson.print("quit is Clicked");
        }

    }
}

