using HawkEngine;
using System;
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
    private string MenuSFX = "Assets/Audio/UI/Open_Menu.wav";
    private string ConfirmSFX = "Assets/Audio/UI/Confirm.wav";
    private string MainMenuMusic = "Assets/Audio/Music/MainTheme_BetaBuild2.ogg";

    //     private AudioSource sound;
    //     private string buttonHovered = "Assets/Audio/SFX/UI/UI_Hover.wav";
    //     private string buttonClicked = "Assets/Audio/SFX/UI/UI_Click.wav";
    //     private string buttonStartGame = "Assets/Audio/SFX/UI/UI_Confirm.wav"; 
    //     private AudioClip buttonHoveredFX;
    //     private AudioClip buttonClickedFX;
    //     private AudioClip buttonStartGameFX;

    private int selectedButtonIndex = -1;
    private UIButton[] buttons;
    private UITransform[] transforms;

    private long lastInputTime = 0;

    private bool isMainMenuMusicPlaying = false;
    private enum InputMethod
    {
        None,
        Joystick,
        DPad,
        Mouse
    }
    private InputMethod currentInputMethod = InputMethod.None;
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
       // sound = gameObject.GetComponent<AudioSource>();

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

//         buttonHoveredFX = new AudioClip(buttonHovered, "ButtonHoveredFX", false, false);
//         buttonClickedFX = new AudioClip(buttonClicked, "ButtonClickedFX", false, false);
//         buttonStartGameFX = new AudioClip(buttonStartGame, "ButtonStartGameFX", false, false);
//         sound.LoadAudioClip(buttonHoveredFX);
//         sound.LoadAudioClip(buttonClickedFX);
//         sound.LoadAudioClip(buttonStartGameFX);
    }

    private void NavigateMenu()
    {
        if (buttons == null || buttons.Length == 0)
        {
            Engineson.print("ERROR: Buttons array is null or empty.");
            return;
        }
        long currentTime = DateTime.Now.Ticks;

        if (currentInputMethod != InputMethod.Mouse && currentTime - lastInputTime < 2500000)
        {
            return;
        }

        Vector2 leftStick = Input.GetLeftStick();

        if (Math.Abs(leftStick.Y) > 0.75f && currentInputMethod != InputMethod.DPad && currentInputMethod != InputMethod.Mouse)
        {
            currentInputMethod = InputMethod.Joystick;

            if (leftStick.Y < -0.75f)
            {
                selectedButtonIndex = (selectedButtonIndex - 1 + buttons.Length) % buttons.Length;
                lastInputTime = currentTime;
            }
            else if (leftStick.Y > 0.75f)
            {
                selectedButtonIndex = (selectedButtonIndex + 1) % buttons.Length;
                lastInputTime = currentTime;
            }
        }
        else if ((Input.GetControllerButton(ControllerButton.DPadDown) || Input.GetControllerButton(ControllerButton.DPadUp)) && currentInputMethod != InputMethod.Joystick && currentInputMethod != InputMethod.Mouse)
        {
            currentInputMethod = InputMethod.DPad;

            if (Input.GetControllerButton(ControllerButton.DPadDown))
            {
                selectedButtonIndex = (selectedButtonIndex + 1) % buttons.Length;
                lastInputTime = currentTime;
            }
            else if (Input.GetControllerButton(ControllerButton.DPadUp))
            {
                selectedButtonIndex = (selectedButtonIndex - 1 + buttons.Length) % buttons.Length;
                lastInputTime = currentTime;
            }
        }
        else if (Math.Abs(leftStick.Y) <= 0.75f && !Input.GetControllerButton(ControllerButton.DPadDown) && !Input.GetControllerButton(ControllerButton.DPadUp))
        {
            currentInputMethod = InputMethod.None;
        }

        // Detectar si el rat�n est� sobre un bot�n
        for (int i = 0; i < buttons.Length; i++)
        {
            if (buttons[i] == null)
            {
                Engineson.print($"WARNING: Button at index {i} is null.");
                continue;
            }

            if (IsMouseOverButton(buttons[i]))
            {
                currentInputMethod = InputMethod.Mouse;
                selectedButtonIndex = i;
            }

            if (i == selectedButtonIndex)
            {
                buttons[i].SetState(ButtonState.HOVERED);
                transforms[i].DOScaleUI(new Vector3(0.22f, 0.1f, 0.5f), 0.3f, Modes.EASE_OUT);

                if (!hasPlayedHoverSound[i])
                {
                    //sound?.Play(buttonHoveredFX);
                    hasPlayedHoverSound[i] = true;
                }
            }
            else
            {
                buttons[i].SetState(ButtonState.DEFAULT);
                transforms[i].DOScaleUI(new Vector3(0.182f, 0.070f, 0.4f), 0.3f, Modes.EASE_OUT);
                hasPlayedHoverSound[i] = false;
            }
            if (currentTime - lastInputTime > 20000000) // 2 segundos en ticks (1 segundo = 10,000,000 ticks)
            {
                if (currentInputMethod == InputMethod.None)
                {
                    selectedButtonIndex = -1;
                }
            }
        }

        // Detectar clic del rat�n
        if ((Input.GetMouseButtonDown(1) && currentInputMethod == InputMethod.Mouse && selectedButtonIndex != -1)|| Input.GetControllerButtonDown(ControllerButton.A))
        {
            UIButton selectedButton = buttons[selectedButtonIndex];
            selectedButton.SetState(ButtonState.CLICKED);

            if (selectedButton == button_newGameButton)
            {
                Audio.PlayOneShot(ConfirmSFX);
                Audio.Stop(MainMenuMusic);
                SceneManager.LoadScene("BetaRelease_Week1_Lvl1");
            }
            else if (selectedButton == button_continueButton)
            {
                //sound?.Play(buttonStartGameFX);
                if (SceneManager.isLevel2)
                {
                    Audio.PlayOneShot(ConfirmSFX);
                    Audio.Stop(MainMenuMusic);
                    SceneManager.LoadSceneFromCheckpoint("BetaRelease_Week1_Lvl2");
                }
                else if (SceneManager.isBossFight)
                {
                    Audio.PlayOneShot(ConfirmSFX);
                    Audio.Stop(MainMenuMusic);
                    SceneManager.LoadSceneFromCheckpoint("BetaRelease_Week1_Bossfight");
                }
                else
                {
                    Audio.PlayOneShot(ConfirmSFX);
                    Audio.Stop(MainMenuMusic);
                    SceneManager.LoadSceneFromCheckpoint("BetaRelease_Week1_Lvl1");
                }
                
            }
            else if (selectedButton == button_optionsButton)
            {
                //sound?.Play(buttonClickedFX);
                Audio.PlayOneShot(MenuSFX);
                optionsCanvas.SetActive(true);
            }
            else if (selectedButton == button_quitButton)
            {
                Audio.PlayOneShot(ConfirmSFX);
            }
        }
        
    }

    private bool IsMouseOverButton(UIButton button)
    {
        return button.GetState() == ButtonState.HOVERED;
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

        if (!isMainMenuMusicPlaying)
        {
            Audio.Play(MainMenuMusic, true);
            isMainMenuMusicPlaying = true; 
        }
        //         if (sound == null)
        //         {
        //             Engineson.print("ERROR: Audio not found");
        //             return;
        //         }

        NavigateMenu();
    }
}

