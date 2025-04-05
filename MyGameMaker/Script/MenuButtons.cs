using HawkEngine;
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

    private ButtonState prevState_newGameButton = ButtonState.DEFAULT;
    private ButtonState prevState_continueButton = ButtonState.DEFAULT;
    private ButtonState prevState_optionsButton = ButtonState.DEFAULT;
    private ButtonState prevState_quitButton = ButtonState.DEFAULT;


    private Audio sound;
    //private Audio sound;
    private string buttonHovered = "Assets/Audio/SFX/UI/ButtonSelected.wav";
    private string buttonClicked = "Assets/Audio/SFX/UI/ButtonPressed.wav";

    public override void Awake()
    {

    }
    public override void Start()
    {
        newGameButton = GameObject.Find("new_game_button");
        continueButton = GameObject.Find("continue_button");
        optionsButton = GameObject.Find("options_button");
        quitButton = GameObject.Find("exit_button");
        optionsCanvas = GameObject.Find("Canvas_OptionsMenu");
        sound = gameObject.GetComponent<Audio>();

        button_newGameButton = newGameButton.GetComponent<UIButton>();
        button_continueButton = continueButton.GetComponent<UIButton>();
        button_optionsButton = optionsButton.GetComponent<UIButton>();
        button_quitButton = quitButton.GetComponent<UIButton>();

        transform_newGameButton = newGameButton.GetComponent<UITransform>();
        transform_continueButton = continueButton.GetComponent<UITransform>();
        transform_optionsButton = optionsButton.GetComponent<UITransform>();
        transform_quitButton = quitButton.GetComponent<UITransform>();

        if (newGameButton == null || optionsButton == null || creditsButton == null || quitButton == null)
        {
            Engineson.print("ERROR: No Button object found");
        }

        if (optionsCanvas != null)
        {
            optionsCanvas.SetActive(false);
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
        if (button_newGameButton == null || button_continueButton == null || button_optionsButton == null || button_quitButton == null)
        {
            Engineson.print("ERROR: No Button object found");
            return;
        }
        if (optionsCanvas == null)
        {
            Engineson.print("ERROR: No Canvas object found");
            return;
        }
        if (sound == null)
        {
            Engineson.print("ERROR: Audio not found");
            return;
        }

        if (button_newGameButton.GetState() == ButtonState.CLICKED)
        {
            sound?.LoadAudio(buttonClicked);
            sound?.Play();
            SceneManager.LoadScene("SpaceShip");
            //por ahora lo hacemos asi, pero el problema es que no comienza con la vida temporal
        }

        HandleHoveredState(button_newGameButton, transform_newGameButton, ref prevState_newGameButton);
        if (button_newGameButton.GetState() == ButtonState.HOVERED)
        {
            transform_newGameButton.DOScaleUI(new Vector3(0.25f, 0.1f, 0.5f), 0.3f, Modes.EASE_OUT);
            //sound?.LoadAudio(buttonHovered);
            //sound?.Play();
        }
        else if (button_newGameButton.GetState() == ButtonState.DEFAULT)
        {
            transform_newGameButton.DOScaleUI(new Vector3(0.182f, 0.070f, 0.4f), 0.3f, Modes.EASE_OUT);
        }

        if (button_continueButton.GetState() == ButtonState.CLICKED)
        {
            //Cargar el juego
            SceneManager.LoadScene("Mortis_Level1");
            //por ahora lo hacemos asi, pero el problema es que no comienza con la vida temporal
            sound?.LoadAudio(buttonClicked);
            sound?.Play();
            gameObject.SetActive(false);
        }
        if (button_continueButton.GetState() == ButtonState.HOVERED)
        {
            transform_continueButton.DOScaleUI(new Vector3(0.25f, 0.1f, 0.5f), 0.3f, Modes.EASE_OUT);
            //sound?.LoadAudio(buttonHovered);
            //sound?.Play();
        }
        else if (button_continueButton.GetState() == ButtonState.DEFAULT)
        {
            transform_continueButton.DOScaleUI(new Vector3(0.182f, 0.070f, 0.4f), 0.3f, Modes.EASE_OUT);
        }

        HandleHoveredState(button_continueButton, transform_continueButton, ref prevState_continueButton);

        if (button_optionsButton.GetState() == ButtonState.CLICKED)
        {
            sound?.LoadAudio(buttonClicked);
            sound?.Play();
            optionsCanvas.SetActive(true);
        }

        HandleHoveredState(button_optionsButton, transform_optionsButton, ref prevState_optionsButton);
        if (button_optionsButton.GetState() == ButtonState.HOVERED)
        {
            transform_optionsButton.DOScaleUI(new Vector3(0.25f, 0.1f, 0.5f), 0.3f, Modes.EASE_OUT);
            //sound?.LoadAudio(buttonHovered);
            //sound?.Play();
        }
        else if (button_optionsButton.GetState() == ButtonState.DEFAULT)
        {
            transform_optionsButton.DOScaleUI(new Vector3(0.182f, 0.070f, 0.4f), 0.3f, Modes.EASE_OUT);
        }

        if (button_quitButton.GetState() == ButtonState.CLICKED)
        {
            //Salir del juego
            sound?.LoadAudio(buttonClicked);
            sound?.Play();
        }

        HandleHoveredState(button_quitButton, transform_quitButton, ref prevState_quitButton);
        if (button_quitButton.GetState() == ButtonState.HOVERED)
        {
            transform_quitButton.DOScaleUI(new Vector3(0.25f, 0.1f, 0.5f), 0.3f, Modes.EASE_OUT);
            //sound?.LoadAudio(buttonHovered);
            //sound?.Play();
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