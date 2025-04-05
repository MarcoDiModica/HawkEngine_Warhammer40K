using HawkEngine;
using System;
using System.Numerics;

public class LoseScreen : MonoBehaviour
{
    private GameObject mainMenu;
    private GameObject loadLastCheckpoint;
    private GameObject mainMenuButton;
    private GameObject quitButton;
    private UIButton button_loadLastCheckpoint;
    private UIButton button_mainMenuButton;
    private UIButton button_quitButton;
    private UITransform transform_loadLastCheckpoint;
    private UITransform transform_mainMenuButton;
    private UITransform transform_quitButton;
    private GameObject HUD;
    private GameObject Player;
    private PlayerData playerData;
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
        loadLastCheckpoint = GameObject.Find("LLC_button");
        mainMenuButton = GameObject.Find("MM_button");
        quitButton = GameObject.Find("QUIT_button");
        button_loadLastCheckpoint = loadLastCheckpoint.GetComponent<UIButton>();
        button_mainMenuButton = mainMenuButton.GetComponent<UIButton>();
        button_quitButton = quitButton.GetComponent<UIButton>();

        if (mainMenuButton == null || quitButton == null)
        {
            Engineson.print("ERROR: No Button object found");
        }
        transform_loadLastCheckpoint = loadLastCheckpoint.GetComponent<UITransform>();
        transform_mainMenuButton = mainMenuButton.GetComponent<UITransform>();
        transform_quitButton = quitButton.GetComponent<UITransform>();
        if (mainMenuButton == null || quitButton == null)
        {
            Engineson.print("ERROR: No Button object found");
        }

        HUD = GameObject.Find("Canvas_HUD");

        Player = GameObject.Find("Player");
        if (Player == null)
        {
            Engineson.print("ERROR: Player not found");
        }

        playerData = Player.GetComponent<PlayerController>().playerData;
        if (playerData == null)
        {
            Engineson.print("ERROR: PlayerData not found");
        }
    }
    public override void Update(float deltaTime)
    {


        if (button_loadLastCheckpoint.GetState() == ButtonState.CLICKED)
        {
            //por ahora lo hacemos asi, pero el problema es que no comienza con la vida temporal
            playerData.SetHealth(playerData.GetMaxHealth());
            HUD.SetActive(true);
            this.gameObject.SetActive(false);
        }
        if (button_loadLastCheckpoint.GetState() == ButtonState.HOVERED)
        {
            transform_loadLastCheckpoint.DOScaleUI(new Vector3(0.3f, 0.08f, 0.5f), 0.3f, Modes.EASE_OUT);
        }
        else if (button_loadLastCheckpoint.GetState() == ButtonState.DEFAULT)
        {
            transform_loadLastCheckpoint.DOScaleUI(new Vector3(0.261f, 0.059f, 0.4f), 0.3f, Modes.EASE_OUT);
        }

        if (button_mainMenuButton.GetState() == ButtonState.CLICKED)
        {
            mainMenu.SetActive(true);
            this.gameObject.SetActive(false);
        }
        if (button_mainMenuButton.GetState() == ButtonState.HOVERED)
        {
            transform_mainMenuButton.DOScaleUI(new Vector3(0.2f, 0.080f, 0.5f), 0.3f, Modes.EASE_OUT);
        }
        else if (button_mainMenuButton.GetState() == ButtonState.DEFAULT)
        {
            transform_mainMenuButton.DOScaleUI(new Vector3(0.175f, 0.058f, 0.4f), 0.3f, Modes.EASE_OUT);
        }

        if (button_quitButton.GetState() == ButtonState.CLICKED)
        {
            //Salir del juego
        }

        if (button_quitButton.GetState() == ButtonState.HOVERED)
        {
            transform_quitButton.DOScaleUI(new Vector3(0.2f, 0.080f, 0.5f), 0.3f, Modes.EASE_OUT);
        }
        else if (button_quitButton.GetState() == ButtonState.DEFAULT)
        {
            transform_quitButton.DOScaleUI(new Vector3(0.181f, 0.060f, 0.4f), 0.3f, Modes.EASE_OUT);
        }

    }
}