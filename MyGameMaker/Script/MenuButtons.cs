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
    private string buttonHovered = "Assets/Audio/SFX/UI/ButtonSelected.wav";
    private string buttonClicked = "Assets/Audio/SFX/UI/ButtonPressed.wav";

    public override void Awake()
    {

    }
    public override void Start()
    {
        newGameButton = GameObject.Find("NewGameButton");
        continueButton = GameObject.Find("ContinueButton");
        optionsButton = GameObject.Find("OptionsButton");
        creditsButton = GameObject.Find("CreditsButton");
        quitButton = GameObject.Find("QuitButton");

        button_newGameButton = newGameButton.GetComponent<UIButton>();
        button_continueButton = continueButton.GetComponent<UIButton>();
        button_optionsButton = optionsButton.GetComponent<UIButton>();
        button_creditsButton = creditsButton.GetComponent<UIButton>();
        button_quitButton = quitButton.GetComponent<UIButton>();

        transform_newGameButton = newGameButton.GetComponent<UITransform>();
        transform_continueButton = continueButton.GetComponent<UITransform>();
        transform_optionsButton = optionsButton.GetComponent<UITransform>();
        transform_creditsButton = creditsButton.GetComponent<UITransform>();
        transform_quitButton = quitButton.GetComponent<UITransform>();

        //sound = gameObject.GetComponent<Audio>();

        if (newGameButton == null || optionsButton == null || creditsButton == null || quitButton == null)
        {
            Engineson.print("ERROR: No Button object found");
        }
    }

    public override void Update(float deltaTime)
    {
        if (button_newGameButton.GetState() == ButtonState.CLICKED)
        {
            
            SceneManager.LoadScene("DefaultScene");
        }

        //if (continueButton.GetState() == ButtonState.CLICKED)
        //{
        //    EJEMPLO PARA HACER UN TWEEN!!
        //    transform_continueButton.DOScaleUI(new Vector3(1.0f, 1.0f, 1.0f), 0.2f, Modes.EASE_IN_OUT);
        //    
        //    
        //    Engineson.print("Continue is Clicked");
        //}

        //if (optionsButton.GetState() == ButtonState.CLICKED)
        //{
        //    sound?.LoadAudio(buttonClicked);
        //    sound?.Play();
        //    Engineson.print("Options is Clicked");
        //}

        //if (creditsButton.GetState() == ButtonState.CLICKED)
        //{
        //    sound?.LoadAudio(buttonClicked);
        //    sound?.Play();
        //    Engineson.print("Credits is Clicked");
        //}

        //if (quitButton.GetState() == ButtonState.CLICKED)
        //{
        //    sound?.LoadAudio(buttonClicked);
        //    sound?.Play();
        //    Engineson.print("quit is Clicked");
        //}

    }
}

