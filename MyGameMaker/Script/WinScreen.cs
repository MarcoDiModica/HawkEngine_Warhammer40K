using HawkEngine;
using System;
using System.Numerics;

public class WinScreen : MonoBehaviour
{
    private GameObject mainMenu;
    private GameObject mainMenuButton;
    private GameObject quitButton;
    private UIButton button_mainMenuButton;
    private UIButton button_quitButton;
    private UITransform transform_mainMenuButton;
    private UITransform transform_quitButton;
    private Audio sound;
    private ButtonState prevState_mainMenuButton = ButtonState.DEFAULT;
    private ButtonState prevState_quitButton = ButtonState.DEFAULT;

    private string buttonHovered = "Assets/Audio/SFX/UI/ButtonSelected.wav";
    private string buttonClicked = "Assets/Audio/SFX/UI/ButtonPressed.wav";
    public override void Awake()
    {
        //Engineson.print("WinScreen Awake");
    }
    public override void Start()
    {
        //Engineson.print("WinScreen Start");
        mainMenu = GameObject.Find("Canvas_Main_Menu");
        mainMenuButton = GameObject.Find("Menu_Button");
        quitButton = GameObject.Find("Quit_Button");
        sound = gameObject.GetComponent<Audio>();
        button_mainMenuButton = mainMenuButton.GetComponent<UIButton>();
        button_quitButton = quitButton.GetComponent<UIButton>();
        if (mainMenuButton == null || quitButton == null)
        {
            Engineson.print("ERROR: No Button object found");
            return;
        }

        if (mainMenu == null)
        {
            Engineson.print("ERROR: No Main Menu object found");
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
    }
    private void HandleHoveredState(UIButton button, UITransform transform, ref ButtonState prevState)
    {
        if (button.GetState() == ButtonState.HOVERED && prevState != ButtonState.HOVERED)
        {
            transform.DOScaleUI(new Vector3(0.25f, 0.1f, 0.5f), 0.3f, Modes.EASE_OUT);
            sound?.LoadAudio(buttonHovered);
            sound?.Play();
        }
        else if (button.GetState() == ButtonState.DEFAULT)
        {
            transform.DOScaleUI(new Vector3(0.182f, 0.070f, 0.4f), 0.3f, Modes.EASE_OUT);
        }
        prevState = button.GetState();
    }

    public override void Update(float deltaTime)
    {
        if (mainMenu == null || mainMenuButton == null || quitButton == null)
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
            mainMenu.SetActive(true);
            this.gameObject.SetActive(false);
            sound?.LoadAudio(buttonClicked);
            sound?.Play();
        }

        HandleHoveredState(button_mainMenuButton, transform_mainMenuButton, ref prevState_mainMenuButton);

        if (button_quitButton.GetState() == ButtonState.CLICKED)
        {
            //Salir del juego
            sound?.LoadAudio(buttonClicked);
            sound?.Play();
        }

        HandleHoveredState(button_quitButton, transform_quitButton, ref prevState_quitButton);
    }
}