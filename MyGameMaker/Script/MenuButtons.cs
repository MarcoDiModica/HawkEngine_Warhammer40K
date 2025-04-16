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


    private AudioSource sound;
    //private Audio sound;
    private string buttonHovered = "Assets/Audio/SFX/UI/UI_Hover.wav";
    private string buttonClicked = "Assets/Audio/SFX/UI/UI_Click.wav";
    private string buttonStartGame = "Assets/Audio/SFX/UI/UI_Confirm.wav";
    private AudioClip hoveredFX;
    private AudioClip clickedFX;
    private AudioClip startGameFX;

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
        sound = gameObject.GetComponent<AudioSource>();

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

        AudioSource emptyMusic = GameObject.Find("EmptyMusic").GetComponent<AudioSource>();
        if (emptyMusic != null)
        {
            //emptyMusic.Play();
        }

        hoveredFX = new AudioClip(buttonHovered, "HoveredFX", false, false);
        clickedFX = new AudioClip(buttonClicked, "ClickedFX", false, false);
        startGameFX = new AudioClip(buttonStartGame, "StartGameFX", false, false);
        sound?.LoadAudioClip(hoveredFX);
        sound?.LoadAudioClip(clickedFX);
        sound?.LoadAudioClip(startGameFX);

    }

    private void HandleHoveredState(UIButton button, UITransform transform, ref ButtonState prevState)
    {
        if (button.GetState() == ButtonState.HOVERED && prevState != ButtonState.HOVERED)
        {
            transform.DOScaleUI(new Vector3(0.25f, 0.1f, 0.5f), 0.3f, Modes.EASE_OUT);
            sound?.Play(hoveredFX);
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
            sound?.Play(startGameFX);
            SceneManager.LoadScene("Mortis_Level1");
        }

        HandleHoveredState(button_newGameButton, transform_newGameButton, ref prevState_newGameButton);

        if (button_continueButton.GetState() == ButtonState.CLICKED)
        {
            sound?.Play(startGameFX);
            SceneManager.LoadScene("SpaceShip");
        }

        HandleHoveredState(button_continueButton, transform_continueButton, ref prevState_continueButton);

        if (button_optionsButton.GetState() == ButtonState.CLICKED)
        {
            sound?.Play(clickedFX);
            optionsCanvas.SetActive(true);
        }

        HandleHoveredState(button_optionsButton, transform_optionsButton, ref prevState_optionsButton);

        if (button_quitButton.GetState() == ButtonState.CLICKED)
        {
            //Salir del juego
            sound?.Play(clickedFX);
        }

        HandleHoveredState(button_quitButton, transform_quitButton, ref prevState_quitButton);
    }
}