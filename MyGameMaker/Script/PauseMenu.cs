using HawkEngine;
using System;
using System.Numerics;


public class PauseMenu : MonoBehaviour
{
    private GameObject optionsMenu;
    private GameObject mainMenu;
    private GameObject resumeButton;
    private GameObject optionsMenuButton;
    private GameObject mainMenuButton;
    private GameObject quitButton;
    private bool isOptionsMenuActive = false;

    

    private UIButton button_resumeButton;
    private UIButton button_optionsMenuButton;
    private UIButton button_mainMenuButton;
    private UIButton button_quitButton;

    private UITransform transform_resumeButton;
    private UITransform transform_optionsMenuButton;
    private UITransform transform_mainMenuButton;
    private UITransform transform_quitButton;

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
        mainMenu = GameObject.Find("Canvas_Main_Menu");
        resumeButton = GameObject.Find("Resume_Button");
        optionsMenuButton = GameObject.Find("Options_Button");
        mainMenuButton = GameObject.Find("MainMenu_Button");
        quitButton = GameObject.Find("Exit_Button");

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
        }

        //sound = gameObject.GetComponent<Audio>();


    }

    public override void Update(float deltaTime)
    {
        //Engineson.print("OptionMenu Update");
        if (Input.GetKeyDown(KeyCode.ESCAPE) || Input.GetControllerButtonDown(ControllerButton.B) || button_resumeButton.GetState() == ButtonState.CLICKED)
        {
            if(isOptionsMenuActive)
            {
                optionsMenu.SetActive(false);
                isOptionsMenuActive = false;
            }
            else
            {
                this.gameObject.SetActive(false);
            }
        }

        if (button_resumeButton.GetState() == ButtonState.HOVERED)
        {
            transform_resumeButton.DOScaleUI(new Vector3(0.2f, 0.1f, 0.5f), 0.3f, Modes.EASE_OUT);
        }
        else if (button_resumeButton.GetState() == ButtonState.DEFAULT)
        {
            transform_resumeButton.DOScaleUI(new Vector3(0.148f, 0.083f, 0.5f), 0.3f, Modes.EASE_OUT);
        }

        if (button_optionsMenuButton.GetState() == ButtonState.CLICKED)
        {
            optionsMenu.SetActive(true);
            isOptionsMenuActive = true;
        }
        if (button_optionsMenuButton.GetState() == ButtonState.HOVERED)
        {
            transform_optionsMenuButton.DOScaleUI(new Vector3(0.2f, 0.1f, 0.5f), 0.3f, Modes.EASE_OUT);
        }
        else if (button_optionsMenuButton.GetState() == ButtonState.DEFAULT)
        {
            transform_optionsMenuButton.DOScaleUI(new Vector3(0.148f, 0.083f, 0.5f), 0.3f, Modes.EASE_OUT);
        }

        if (button_mainMenuButton.GetState() == ButtonState.CLICKED)
        {
            //SceneManager.LoadScene("MainMenu");
            mainMenu.SetActive(true);
            this.gameObject.SetActive(false);
        }
        if (button_mainMenuButton.GetState() == ButtonState.HOVERED)
        {
            transform_mainMenuButton.DOScaleUI(new Vector3(0.2f, 0.1f, 0.5f), 0.3f, Modes.EASE_OUT);
        }
        else if (button_mainMenuButton.GetState() == ButtonState.DEFAULT)
        {
            transform_mainMenuButton.DOScaleUI(new Vector3(0.148f, 0.083f, 0.5f), 0.3f, Modes.EASE_OUT);
        }

        if (button_quitButton.GetState() == ButtonState.CLICKED)
        {
            //Salir del juego
        }

        if (button_quitButton.GetState() == ButtonState.HOVERED)
        {
            transform_quitButton.DOScaleUI(new Vector3(0.2f, 0.1f, 0.5f), 0.3f, Modes.EASE_OUT);
        }
        else if (button_quitButton.GetState() == ButtonState.DEFAULT)
        {
            transform_quitButton.DOScaleUI(new Vector3(0.148f, 0.083f, 0.5f), 0.3f, Modes.EASE_OUT);
        }

        //Esto lo tengo que mover al HUD por ahora y luego ya al script del gameplay o lo que sea
        if (Input.GetKeyDown(KeyCode.P) || Input.GetControllerButtonDown(ControllerButton.Start))
        {
            this.gameObject.SetActive(true);
        }
    }

}