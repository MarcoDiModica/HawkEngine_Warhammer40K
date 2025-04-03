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
        button_mainMenuButton = mainMenuButton.GetComponent<UIButton>();
        button_quitButton = quitButton.GetComponent<UIButton>();
        if (mainMenuButton == null || quitButton == null)
        {
            Engineson.print("ERROR: No Button object found");
        }
    }
    public override void Update(float deltaTime)
    {
        if (button_mainMenuButton.GetState() == ButtonState.CLICKED)
        {
            mainMenu.SetActive(true);
            this.gameObject.SetActive(false);
        }

        if(button_quitButton.GetState() == ButtonState.CLICKED)
        {
            //Salir del juego
        }

    }
}