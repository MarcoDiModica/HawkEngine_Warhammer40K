using HawkEngine;
using System;
using System.Numerics;

public class WinScreen : MonoBehaviour
{
    private GameObject mainMenuButton;
    private GameObject quitButton;
    private UIButton button_mainMenuButton;
    private UIButton button_quitButton;
    private UITransform transform_mainMenuButton;
    private UITransform transform_quitButton;
    private AudioSource sound;
    private ButtonState prevState_mainMenuButton = ButtonState.DEFAULT;
    private ButtonState prevState_quitButton = ButtonState.DEFAULT;

    private string buttonHovered = "Assets/Audio/SFX/UI/UI_Hover.wav";
    private string buttonClicked = "Assets/Audio/SFX/UI/UI_Click.wav";
    private AudioClip buttonHoveredFX;
    private AudioClip buttonClickedFX;
    public override void Awake()
    {
        //Engineson.print("WinScreen Awake");
    }
    public override void Start()
    {
        mainMenuButton = GameObject.Find("Menu_Button");
        quitButton = GameObject.Find("Quit_Button");
        sound = gameObject.GetComponent<AudioSource>();
        button_mainMenuButton = mainMenuButton.GetComponent<UIButton>();
        button_quitButton = quitButton.GetComponent<UIButton>();

        if (mainMenuButton == null || quitButton == null)
        {
            Engineson.print("ERROR: No Button object found");
            return;
        }

        transform_mainMenuButton = mainMenuButton.GetComponent<UITransform>();
        transform_quitButton = quitButton.GetComponent<UITransform>();
        if (mainMenuButton == null || quitButton == null)
        {
            Engineson.print("ERROR: No Button object found");
        }

        if (sound == null)
        {
            Engineson.print("ERROR: No Audio object found");
        }

        buttonHoveredFX = new AudioClip(buttonHovered, "ButtonHoveredFX", false, false);
        buttonClickedFX = new AudioClip(buttonClicked, "ButtonClickedFX", false, false);
        sound.LoadAudioClip(buttonHoveredFX);
        sound.LoadAudioClip(buttonClickedFX);
    }
    private void HandleHoveredState(UIButton button, UITransform transform, ref ButtonState prevState)
    {
        if (button.GetState() == ButtonState.HOVERED && prevState != ButtonState.HOVERED)
        {
            transform.DOScaleUI(new Vector3(0.25f, 0.1f, 0.5f), 0.3f, Modes.EASE_OUT);

            sound?.Play(buttonHoveredFX);
        }
        else if (button.GetState() == ButtonState.DEFAULT)
        {
            transform.DOScaleUI(new Vector3(0.182f, 0.070f, 0.4f), 0.3f, Modes.EASE_OUT);
        }
        prevState = button.GetState();
    }

    public override void Update(float deltaTime)
    {
        if (mainMenuButton == null || quitButton == null)
        {
            Engineson.print("ERROR: No Button or object found");
            return;
        }
        if (sound == null)
        {
            Engineson.print("ERROR: No Audio object found");
            return;
        }

        if (button_mainMenuButton.GetState() == ButtonState.CLICKED)
        {
            sound?.Play(buttonClickedFX);
            SceneManager.LoadScene("MainMenu");
        }

        HandleHoveredState(button_mainMenuButton, transform_mainMenuButton, ref prevState_mainMenuButton);

        if (button_quitButton.GetState() == ButtonState.CLICKED)
        {
            //Salir del juego
            sound?.Play(buttonClickedFX);
        }

        HandleHoveredState(button_quitButton, transform_quitButton, ref prevState_quitButton);
    }
}