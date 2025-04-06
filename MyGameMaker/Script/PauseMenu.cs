using HawkEngine;
using System;
using System.Numerics;


public class PauseMenu : MonoBehaviour
{
    private GameObject optionsMenu;
    private GameObject resumeButton;
    private GameObject optionsMenuButton;
    private GameObject mainMenuButton;
    private GameObject quitButton;
    private bool isOptionsMenuActive = false;
    private GameObject HUD;
    private HUD HUDScript;

    private UIButton button_resumeButton;
    private UIButton button_optionsMenuButton;
    private UIButton button_mainMenuButton;
    private UIButton button_quitButton;

    private UITransform transform_resumeButton;
    private UITransform transform_optionsMenuButton;
    private UITransform transform_mainMenuButton;
    private UITransform transform_quitButton;

    private Audio sound;

    private ButtonState prevState_resumeButton = ButtonState.DEFAULT;
    private ButtonState prevState_optionsMenuButton = ButtonState.DEFAULT;
    private ButtonState prevState_mainMenuButton = ButtonState.DEFAULT;
    private ButtonState prevState_quitButton = ButtonState.DEFAULT;

    private string buttonHovered = "Assets/Audio/SFX/UI/ButtonSelected.wav";
    private string buttonClicked = "Assets/Audio/SFX/UI/ButtonPressed.wav";

    public override void Awake()
    {
        //Engineson.print("OptionMenu Awake");
    }
    public override void Start()
    {
        //Engineson.print("OptionMenu Start");
        optionsMenu = GameObject.Find("Canvas_OptionsMenu");
        resumeButton = GameObject.Find("Resume_Button");
        optionsMenuButton = GameObject.Find("Options_Button");
        mainMenuButton = GameObject.Find("MainMenu_Button");
        quitButton = GameObject.Find("Exit_Button");
        sound = gameObject.GetComponent<Audio>();

        button_resumeButton = resumeButton.GetComponent<UIButton>();
        button_optionsMenuButton = optionsMenuButton.GetComponent<UIButton>();
        button_mainMenuButton = mainMenuButton.GetComponent<UIButton>();
        button_quitButton = quitButton.GetComponent<UIButton>();

        transform_resumeButton = resumeButton.GetComponent<UITransform>();
        transform_optionsMenuButton = optionsMenuButton.GetComponent<UITransform>();
        transform_mainMenuButton = mainMenuButton.GetComponent<UITransform>();
        transform_quitButton = quitButton.GetComponent<UITransform>();

        if (resumeButton == null || optionsMenuButton == null || mainMenuButton == null || quitButton == null)
        {
            Engineson.print("ERROR: No Button object found");
            return;
        }

        if (optionsMenu == null)
        {
            Engineson.print("ERROR: No Canvas object found");
            return;
        }
        else
        {
            optionsMenu.SetActive(false);
        }
        //sound = gameObject.GetComponent<Audio>();

        HUD = GameObject.Find("Canvas_HUD");
        if (HUD == null)
        {
            Engineson.print("ERROR: HUD not found");
            return;
        }
        HUDScript = HUD.GetComponent<HUD>();
        if (HUDScript == null)
        {
            Engineson.print("ERROR: HUDScript not found");
            return;
        }
        if (sound == null)
        {
            Engineson.print("ERROR: Sound not found");
            return;
        }

        this.gameObject.SetActive(false);
    }

    private void HandleHoveredState(UIButton button, UITransform transform, ref ButtonState prevState)
    {
        if (button.GetState() == ButtonState.HOVERED && prevState != ButtonState.HOVERED)
        {
            transform.DOScaleUI(new Vector3(0.2f, 0.1f, 0.5f), 0.3f, Modes.EASE_OUT);
            sound?.LoadAudio(buttonHovered);
            sound?.Play();
        }
        else if (button.GetState() == ButtonState.DEFAULT)
        {
            transform.DOScaleUI(new Vector3(0.148f, 0.083f, 0.5f), 0.3f, Modes.EASE_OUT);
        }
        prevState = button.GetState();
    }

    public override void Update(float deltaTime)
    {
        if (optionsMenu == null || resumeButton == null || optionsMenuButton == null || mainMenuButton == null || quitButton == null)
        {
            Engineson.print("ERROR: No Button or Canvas object found");
            return;
        }

        if (HUD == null)
        {
            Engineson.print("ERROR: HUD not found");
            return;
        }

        if (sound == null)
        {
            Engineson.print("ERROR: Sound not found");
            return;
        }

        //Engineson.print("OptionMenu Update");
        if (Input.GetKeyDown(KeyCode.ESCAPE) || Input.GetControllerButtonDown(ControllerButton.B) || button_resumeButton.GetState() == ButtonState.CLICKED)
        {
            sound?.LoadAudio(buttonClicked);
            sound?.Play();

            if (isOptionsMenuActive)
            {
                optionsMenu.SetActive(false);
                isOptionsMenuActive = false;
            }
            else
            {
                HUDScript.isPaused = false;
                gameObject.SetActive(false);
            }
        }

        HandleHoveredState(button_resumeButton, transform_resumeButton, ref prevState_resumeButton);

        if (button_optionsMenuButton.GetState() == ButtonState.CLICKED)
        {
            sound?.LoadAudio(buttonClicked);
            sound?.Play();
            optionsMenu.SetActive(true);
            isOptionsMenuActive = true;
        }

        HandleHoveredState(button_optionsMenuButton, transform_optionsMenuButton, ref prevState_optionsMenuButton);

        if (button_mainMenuButton.GetState() == ButtonState.CLICKED)
        {
            //SceneManager.LoadScene("MainMenu");
            sound?.LoadAudio(buttonClicked);
            sound?.Play();
            SceneManager.LoadScene("MainMenu");
        }

        HandleHoveredState(button_mainMenuButton, transform_mainMenuButton, ref prevState_mainMenuButton);

        if (button_quitButton.GetState() == ButtonState.CLICKED)
        {
            //Salir del juego
            sound?.LoadAudio(buttonClicked);
            sound?.Play();
        }

        HandleHoveredState(button_quitButton, transform_quitButton, ref prevState_quitButton);

        if (Input.GetKeyDown(KeyCode.P) || Input.GetControllerButtonDown(ControllerButton.Start))
        {
            if (HUDScript.openedPause)
            {
                HUDScript.openedPause = false;
                Engineson.print("Avoided closure");
            }
            else
            {
                optionsMenu.SetActive(false);
                isOptionsMenuActive = false;
                Engineson.print("Closing menu");
                HUDScript.isPaused = false;
                gameObject.SetActive(false);
            }
        }
    }
}