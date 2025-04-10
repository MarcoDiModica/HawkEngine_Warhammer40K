using HawkEngine;
using System.Numerics;
using System.Runtime.InteropServices;

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

    private bool[] hasPlayedHoverSound;

    private Audio sound;
    private string buttonHovered = "Assets/Audio/SFX/UI/UI_Hover.wav";
    private string buttonClicked = "Assets/Audio/SFX/UI/UI_Click.wav";
    private string buttonStartGame = "Assets/Audio/SFX/UI/UI_Confirm.wav";

    private int selectedButtonIndex = 0;
    private UIButton[] buttons;
    private UITransform[] transforms;

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

        buttons = new UIButton[] { button_newGameButton, button_continueButton, button_optionsButton, button_quitButton };
        transforms = new UITransform[] { transform_newGameButton, transform_continueButton, transform_optionsButton, transform_quitButton };

        hasPlayedHoverSound = new bool[buttons.Length];

        if (newGameButton == null || optionsButton == null || creditsButton == null || quitButton == null)
        {
            Engineson.print("ERROR: No Button object found");
        }

        if (optionsCanvas != null)
        {
            optionsCanvas.SetActive(false);
        }

        Audio emptyMusic = GameObject.Find("EmptyMusic").GetComponent<Audio>();
        if (emptyMusic != null)
        {
            emptyMusic.Play();
        }
    }

    private void NavigateMenu()
    {
        if (Input.GetControllerButtonDown(ControllerButton.DPadDown))
        {
            selectedButtonIndex = (selectedButtonIndex + 1) % buttons.Length;
        }
        else if (Input.GetControllerButtonDown(ControllerButton.DPadUp))
        {
            selectedButtonIndex = (selectedButtonIndex - 1 + buttons.Length) % buttons.Length;
        }

        for (int i = 0; i < buttons.Length; i++)
        {
            if (buttons[i] == null)
            {
                Engineson.print($"WARNING: Button at index {i} is null.");
                continue;
            }

            if (i == selectedButtonIndex)
            {
                buttons[i].SetState(ButtonState.HOVERED);
                transforms[i].DOScaleUI(new Vector3(0.25f, 0.1f, 0.5f), 0.3f, Modes.EASE_OUT);

                if (!hasPlayedHoverSound[i])
                {
                    sound?.LoadAudio(buttonHovered);
                    sound?.Play();
                    hasPlayedHoverSound[i] = true;
                }
            }
            else
            {
                buttons[i].SetState(ButtonState.DEFAULT);
                transforms[i].DOScaleUI(new Vector3(0.182f, 0.070f, 0.4f), 0.3f, Modes.EASE_OUT);
                hasPlayedHoverSound[i] = false;
            }
        }

        if (Input.GetControllerButtonDown(ControllerButton.A))
        {
            UIButton selectedButton = buttons[selectedButtonIndex];
            selectedButton.SetState(ButtonState.CLICKED);

            if (selectedButton == button_newGameButton)
            {
                sound?.LoadAudio(buttonStartGame);
                sound?.Play();
                SceneManager.LoadScene("Mortis_Level1");
            }
            else if (selectedButton == button_continueButton)
            {
                sound?.LoadAudio(buttonStartGame);
                sound?.Play();
                SceneManager.LoadScene("SpaceShip");
            }
            else if (selectedButton == button_optionsButton)
            {
                sound?.LoadAudio(buttonClicked);
                sound?.Play();
                optionsCanvas.SetActive(true);
            }
            else if (selectedButton == button_quitButton)
            {
                sound?.LoadAudio(buttonClicked);
                sound?.Play();
                // Aquí puedes agregar la lógica para salir del juego
            }
        }
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

        NavigateMenu();
    }
}

