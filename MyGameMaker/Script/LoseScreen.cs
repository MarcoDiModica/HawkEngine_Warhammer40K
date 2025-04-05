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
    private Audio sound;
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
        sound = gameObject.GetComponent<Audio>();
        button_loadLastCheckpoint = loadLastCheckpoint.GetComponent<UIButton>();
        button_mainMenuButton = mainMenuButton.GetComponent<UIButton>();
        button_quitButton = quitButton.GetComponent<UIButton>();

        if (mainMenuButton == null || quitButton == null)
        {
            Engineson.print("ERROR: No Button object found");
            return;
        }
        if (mainMenu == null)
        {
            Engineson.print("Error: No Canvas found");
            return;
        }
        transform_loadLastCheckpoint = loadLastCheckpoint.GetComponent<UITransform>();
        transform_mainMenuButton = mainMenuButton.GetComponent<UITransform>();
        transform_quitButton = quitButton.GetComponent<UITransform>();
        if (mainMenuButton == null || quitButton == null)
        {
            Engineson.print("ERROR: No Button object found");
        }

        HUD = GameObject.Find("Canvas_HUD");

        if (HUD == null)
        {
            Engineson.print("ERROR: HUD not found");
            return;
        }

        Player = GameObject.Find("Player");
        if (Player == null)
        {
            Engineson.print("ERROR: Player not found");
            return;
        }

        if (sound == null)
        {
            Engineson.print("ERROR: Audio not found");
            return;
        }

        playerData = Player.GetComponent<PlayerController>().playerData;
        if (playerData == null)
        {
            Engineson.print("ERROR: PlayerData not found");
            return;
        }
    }
    public override void Update(float deltaTime)
    {
        if (mainMenu == null || loadLastCheckpoint == null || mainMenuButton == null || quitButton == null)
        {
            Engineson.print("ERROR: No Button or object found");
            return;
        }

        if (mainMenu == null)
        {
            Engineson.print("ERROR: No Main Menu object found");
            return;
        }

        if (Player == null)
        {
            Engineson.print("ERROR: Player not found");
            return;
        }

        if (playerData == null)
        {
            Engineson.print("ERROR: PlayerData not found");
            return;
        }

        if (HUD == null)
        {
            Engineson.print("ERROR: HUD not found");
            return;
        }
        if (sound == null)
        {
            Engineson.print("ERROR: Audio not found");
            return;
        }

        if (button_loadLastCheckpoint.GetState() == ButtonState.CLICKED)
        {
            //por ahora lo hacemos asi, pero el problema es que no comienza con la vida temporal
            playerData.SetHealth(playerData.GetMaxHealth());
            sound?.LoadAudio(buttonClicked);
            sound?.Play();
            HUD.SetActive(true);
            this.gameObject.SetActive(false);
        }
        if (button_loadLastCheckpoint.GetState() == ButtonState.HOVERED)
        {
            transform_loadLastCheckpoint.DOScaleUI(new Vector3(0.3f, 0.08f, 0.5f), 0.3f, Modes.EASE_OUT);
            sound?.LoadAudio(buttonHovered);
            sound?.Play();
        }
        else if (button_loadLastCheckpoint.GetState() == ButtonState.DEFAULT)
        {
            transform_loadLastCheckpoint.DOScaleUI(new Vector3(0.261f, 0.059f, 0.4f), 0.3f, Modes.EASE_OUT);
        }

        if (button_mainMenuButton.GetState() == ButtonState.CLICKED)
        {
            sound?.LoadAudio(buttonClicked);
            sound?.Play();
            mainMenu.SetActive(true);
            this.gameObject.SetActive(false);
        }
        if (button_mainMenuButton.GetState() == ButtonState.HOVERED)
        {
            transform_mainMenuButton.DOScaleUI(new Vector3(0.2f, 0.080f, 0.5f), 0.3f, Modes.EASE_OUT);
            sound?.LoadAudio(buttonHovered);
            sound?.Play();
        }
        else if (button_mainMenuButton.GetState() == ButtonState.DEFAULT)
        {
            transform_mainMenuButton.DOScaleUI(new Vector3(0.175f, 0.058f, 0.4f), 0.3f, Modes.EASE_OUT);
        }

        if (button_quitButton.GetState() == ButtonState.CLICKED)
        {
            //Salir del juego
            sound?.LoadAudio(buttonClicked);
            sound?.Play();
        }

        if (button_quitButton.GetState() == ButtonState.HOVERED)
        {
            transform_quitButton.DOScaleUI(new Vector3(0.2f, 0.080f, 0.5f), 0.3f, Modes.EASE_OUT);
            sound?.LoadAudio(buttonHovered);
            sound?.Play();
        }
        else if (button_quitButton.GetState() == ButtonState.DEFAULT)
        {
            transform_quitButton.DOScaleUI(new Vector3(0.181f, 0.060f, 0.4f), 0.3f, Modes.EASE_OUT);
        }

    }
}