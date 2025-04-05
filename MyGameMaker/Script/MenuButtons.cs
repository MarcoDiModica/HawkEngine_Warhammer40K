using HawkEngine;
using System;
using System.Numerics;

public class MenuButtons : MonoBehaviour
{
    //Referencia el boton y los componentes que quieras usar aqui

    private GameObject newGameButton;
    private GameObject continueButton;
    private GameObject optionsButton;
    private GameObject creditsButton;
    private GameObject quitButton;
    private GameObject optionsCanvas;
    private GameObject HUD;
    private GameObject Player;
    private PlayerData playerData;

    private UIButton button_newGameButton;
    private UIButton button_continueButton;
    private UIButton button_optionsButton;
    private UIButton button_creditsButton;
    private UIButton button_quitButton;

    private UITransform transform_newGameButton;
    private UITransform transform_continueButton;
    private UITransform transform_optionsButton;
    private UITransform transform_creditsButton;
    private UITransform transform_quitButton;
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
        newGameButton = GameObject.Find("new_game_button");
        continueButton = GameObject.Find("continue_button");
        optionsButton = GameObject.Find("options_button");
        //creditsButton = GameObject.Find("CreditsButton");
        quitButton = GameObject.Find("exit_button");
        optionsCanvas = GameObject.Find("Canvas_OptionsMenu");

        button_newGameButton = newGameButton.GetComponent<UIButton>();
        button_continueButton = continueButton.GetComponent<UIButton>();
        button_optionsButton = optionsButton.GetComponent<UIButton>();
        //button_creditsButton = creditsButton.GetComponent<UIButton>();
        button_quitButton = quitButton.GetComponent<UIButton>();

        transform_newGameButton = newGameButton.GetComponent<UITransform>();
        transform_continueButton = continueButton.GetComponent<UITransform>();
        transform_optionsButton = optionsButton.GetComponent<UITransform>();
        //transform_creditsButton = creditsButton.GetComponent<UITransform>();
        transform_quitButton = quitButton.GetComponent<UITransform>();

        //sound = gameObject.GetComponent<Audio>();
        
        if (newGameButton == null || optionsButton == null || creditsButton == null || quitButton == null)
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
        if (button_newGameButton.GetState() == ButtonState.CLICKED)
        {
            //SceneManager.LoadScene("DefaultScene");
            //por ahora lo hacemos asi, pero el problema es que no comienza con la vida temporal
            playerData.SetHealth(playerData.GetMaxHealth());
            HUD.SetActive(true);
            gameObject.SetActive(false);
        }

        if (button_newGameButton.GetState() == ButtonState.HOVERED)
        {
            transform_newGameButton.DOScaleUI(new Vector3(0.25f, 0.1f, 0.5f), 0.3f, Modes.EASE_OUT);
        }
        else if (button_newGameButton.GetState() == ButtonState.DEFAULT)
        {
            transform_newGameButton.DOScaleUI(new Vector3(0.182f, 0.070f, 0.4f), 0.3f, Modes.EASE_OUT);
        }

        if (button_continueButton.GetState() == ButtonState.CLICKED)
        {
            //Cargar el juego
            //SceneManager.LoadScene("DefaultScene");
            //por ahora lo hacemos asi, pero el problema es que no comienza con la vida temporal
            playerData.SetHealth(playerData.GetMaxHealth());
            HUD.SetActive(true);
            gameObject.SetActive(false);
        }
        if(button_continueButton.GetState() == ButtonState.HOVERED)
        {
            transform_continueButton.DOScaleUI(new Vector3(0.25f, 0.1f, 0.5f), 0.3f, Modes.EASE_OUT);
        }
        else if (button_continueButton.GetState() == ButtonState.DEFAULT)
        {
            transform_continueButton.DOScaleUI(new Vector3(0.182f, 0.070f, 0.4f), 0.3f, Modes.EASE_OUT);
        }


        if (button_optionsButton.GetState() == ButtonState.CLICKED)
        {
            optionsCanvas.SetActive(true);
            
        }

        if (button_optionsButton.GetState() == ButtonState.HOVERED)
        {
            transform_optionsButton.DOScaleUI(new Vector3(0.25f, 0.1f, 0.5f), 0.3f, Modes.EASE_OUT);
        }
        else if (button_optionsButton.GetState() == ButtonState.DEFAULT)
        {
            transform_optionsButton.DOScaleUI(new Vector3(0.182f, 0.070f, 0.4f), 0.3f, Modes.EASE_OUT);
        }

        if (button_quitButton.GetState() == ButtonState.CLICKED)
        {
            //Salir del juego
        }

        if (button_quitButton.GetState() == ButtonState.HOVERED)
        {
            transform_quitButton.DOScaleUI(new Vector3(0.25f, 0.1f, 0.5f), 0.3f, Modes.EASE_OUT);
        }
        else if (button_quitButton.GetState() == ButtonState.DEFAULT)
        {
            transform_quitButton.DOScaleUI(new Vector3(0.182f, 0.070f, 0.4f), 0.3f, Modes.EASE_OUT);
        }

        //if (continueButton.GetState() == ButtonState.CLICKED)
        //{
        //    EJEMPLO PARA HACER UN TWEEN!!
        //    transform_continueButton.DOScaleUI(new Vector3(1.0f, 1.0f, 1.0f), 0.2f, Modes.EASE_IN_OUT);
        //    
        //    
        //    Engineson.print("Continue is Clicked");
        //}

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

