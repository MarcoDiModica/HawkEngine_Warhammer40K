using HawkEngine;
using System;
using System.Numerics;
using System.Runtime.InteropServices;

public class MenuButtons : MonoBehaviour
{
    private GameObject newGameButton;
    private GameObject continueButton;
    private GameObject optionsButton;
    private GameObject creditsButton;
    private GameObject quitButton;
    private GameObject optionsCanvas;
    private GameObject creditsCanvas;

    private UIButton button_newGameButton;
    private UIButton button_continueButton;
    private UIButton button_optionsButton;
    private UIButton button_creditsButton;
    private UIButton button_quitButton;

    private UIImage image_newGameButton;
    private UIImage image_continueButton;
    private UIImage image_optionsButton;
    private UIImage image_creditsButton;
    private UIImage image_quitButton;

    private UITransform transform_newGameButton;
    private UITransform transform_continueButton;
    private UITransform transform_optionsButton;
    private UITransform transform_creditsButton;
    private UITransform transform_quitButton;

    private ButtonState prevState_newGameButton = ButtonState.DEFAULT;
    private ButtonState prevState_continueButton = ButtonState.DEFAULT;
    private ButtonState prevState_optionsButton = ButtonState.DEFAULT;
    private ButtonState prevState_creditsButton = ButtonState.DEFAULT;
    private ButtonState prevState_quitButton = ButtonState.DEFAULT;

    private bool[] hasPlayedHoverSound;
    private string MenuSFX = "Assets/Audio/UI/Open_Menu.wav";
    private string ConfirmSFX = "Assets/Audio/UI/Confirm.wav";
    private string HoveredSFX = "Assets/Audio/UI/Hovered.wav";
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
    private UIImage[] images;
    private UITransform[] transforms;

    private long lastInputTime = 0;
    private ButtonState[] previousStates;
    private string[] previousImagePaths;
    private bool isMainMenuMusicPlaying = false;
    private GameObject creditsBG;
    private UITransform creditsBGTransform;
    private float scrollY = 0.0f;
    private float scrollSpeed = 1.0f;
    private float minScrollY = 1.0f;
    private float maxScrollY = 2.0f;
    private enum InputMethod
    {
        None,
        Joystick,
        DPad,
        Mouse
    }
    private InputMethod currentInputMethod = InputMethod.None;

    private float Clamp(float value, float min, float max)
    {
        if (value<min) return min;
        if (value > max) return max;
        return value;
    }

    public override void Awake()
    {

    }

    public override void Start()
    {
        newGameButton = GameObject.Find("new_game_button");
        continueButton = GameObject.Find("continue_button");
        optionsButton = GameObject.Find("options_button");
        quitButton = GameObject.Find("exit_button");
        creditsButton = GameObject.Find("credits_button");
        optionsCanvas = GameObject.Find("Canvas_OptionsMenu");
        creditsCanvas = GameObject.Find("Canvas_CreditMenu");
        creditsBG = GameObject.Find("Credits_BG");
        // sound = gameObject.GetComponent<AudioSource>();

        button_newGameButton = newGameButton.GetComponent<UIButton>();
        button_continueButton = continueButton.GetComponent<UIButton>();
        button_optionsButton = optionsButton.GetComponent<UIButton>();
        button_quitButton = quitButton.GetComponent<UIButton>();
        button_creditsButton = creditsButton.GetComponent<UIButton>();

        image_newGameButton = newGameButton.GetComponent<UIImage>();
        image_continueButton = continueButton.GetComponent<UIImage>();
        image_optionsButton = optionsButton.GetComponent<UIImage>();
        image_quitButton = quitButton.GetComponent<UIImage>();
        image_creditsButton = creditsButton.GetComponent<UIImage>();

        transform_newGameButton = newGameButton.GetComponent<UITransform>();
        transform_continueButton = continueButton.GetComponent<UITransform>();
        transform_optionsButton = optionsButton.GetComponent<UITransform>();
        transform_quitButton = quitButton.GetComponent<UITransform>();
        transform_creditsButton = creditsButton.GetComponent<UITransform>();
        creditsBGTransform = creditsBG.GetComponent<UITransform>();

        buttons = new UIButton[] { button_newGameButton, button_continueButton, button_optionsButton, button_creditsButton, button_quitButton };
        images = new UIImage[] { image_newGameButton, image_continueButton, image_optionsButton, image_creditsButton, image_quitButton };
        transforms = new UITransform[] { transform_newGameButton, transform_continueButton, transform_optionsButton, transform_creditsButton, transform_quitButton };

        hasPlayedHoverSound = new bool[buttons.Length];

        if (newGameButton == null || optionsButton == null || creditsButton == null || quitButton == null)
        {
            Engineson.print("ERROR: No Button object found");
        }

        if (optionsCanvas != null)
        {
            optionsCanvas.SetActive(false);
        }

        if (creditsCanvas != null)
        {
            creditsCanvas.SetActive(false);
        }

        previousStates = new ButtonState[buttons.Length];
        previousImagePaths = new string[buttons.Length];

        //         buttonHoveredFX = new AudioClip(buttonHovered, "ButtonHoveredFX", false, false);
        //         buttonClickedFX = new AudioClip(buttonClicked, "ButtonClickedFX", false, false);
        //         buttonStartGameFX = new AudioClip(buttonStartGame, "ButtonStartGameFX", false, false);
        //         sound.LoadAudioClip(buttonHoveredFX);
        //         sound.LoadAudioClip(buttonClickedFX);
        //         sound.LoadAudioClip(buttonStartGameFX);
    }

    private bool IsBlockingMenuActive()
    {
        return (optionsCanvas != null && optionsCanvas.IsActive()) ||
               (creditsCanvas != null && creditsCanvas.IsActive());
    }
    private void NavigateMenu()
    {
        if (buttons == null || buttons.Length == 0)
        {
            Engineson.print("ERROR: Buttons array is null or empty.");
            return;
        }

        if (IsBlockingMenuActive()) return;

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

        // Detectar si el rat n est  sobre un bot n
        for (int i = 0; i < buttons.Length; i++)
        {
            if (IsMouseOverButton(buttons[i]))
            {
                currentInputMethod = InputMethod.Mouse;
                selectedButtonIndex = i;
            }

            ButtonState newState = (i == selectedButtonIndex) ? ButtonState.HOVERED : ButtonState.DEFAULT;

            if (previousStates[i] != newState)
            {
                buttons[i].SetState(newState);
                previousStates[i] = newState;

                Vector3 targetScale = (newState == ButtonState.HOVERED)
                    ? new Vector3(0.22f, 0.22f, 0.5f)
                    : new Vector3(0.182f, 0.187f, 0.4f);

                transforms[i].DOScaleUI(targetScale, 0.3f, Modes.EASE_OUT);
            }

            string imagePath = "";
            if (buttons[i] == button_newGameButton)
                imagePath = (newState == ButtonState.HOVERED) ? "Library/Textures/UI/menu_new game selected.png" : "Library/Textures/UI/menu_new game.png";
            else if (buttons[i] == button_continueButton)
                imagePath = (newState == ButtonState.HOVERED) ? "Library/Textures/UI/menu_continue selected.png" : "Library/Textures/UI/menu_continue.png";
            else if (buttons[i] == button_optionsButton)
                imagePath = (newState == ButtonState.HOVERED) ? "Library/Textures/UI/menu_options selected.png" : "Library/Textures/UI/menu_options.png";
            else if (buttons[i] == button_creditsButton)
                imagePath = (newState == ButtonState.HOVERED) ? "Library/Textures/UI/menu_credits selected.png" : "Library/Textures/UI/menu_credits.png";
            else if (buttons[i] == button_quitButton)
                imagePath = (newState == ButtonState.HOVERED) ? "Library/Textures/UI/menu_exit selected.png" : "Library/Textures/UI/menu_exit.png";

            if (previousImagePaths[i] != imagePath)
            {
                images[i].SetImage(imagePath);
                previousImagePaths[i] = imagePath;
            }

            if (newState == ButtonState.HOVERED)
            {
                if (!hasPlayedHoverSound[i])
                {
                   Audio.Play(HoveredSFX);
                    hasPlayedHoverSound[i] = true;
                }
            }
            else
            {
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

        // Detectar clic del rat n
        if ((Input.GetMouseButtonDown(1) && currentInputMethod == InputMethod.Mouse) || Input.GetControllerButtonDown(ControllerButton.A))
        {
            if (selectedButtonIndex == -1)
                return;

            UIButton selectedButton = buttons[selectedButtonIndex];
            selectedButton.SetState(ButtonState.CLICKED);

            if (selectedButton == button_newGameButton && creditsCanvas.IsActive() == false)
            {
                Audio.PlayOneShot(ConfirmSFX);


                GameObject cinematicObj = GameObject.Find("CinematicManager");
                CinematicManager manager = cinematicObj?.GetComponent<CinematicManager>();
                FadeController fadeController = GameObject.Find("FadeController")?.GetComponent<FadeController>();
                SceneManager.isNewGame = true;
                SceneManager.isLevel2 = false;
                SceneManager.isBossFight = false;
                if (manager != null && fadeController != null)
                {
                    fadeController.FadeIn(1.0f, () =>
                    {
                        manager.StartCinematic(LoadLevel1);
                        fadeController.FadeOut(0.5f);
                    });
                }
                else
                {
                    Audio.Stop(MainMenuMusic);
                    SceneManager.LoadScene("BetaRelease_Week1_Lvl1");
                }


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
            else if (selectedButton == button_creditsButton)
            {
                Audio.PlayOneShot(MenuSFX);
                creditsCanvas.SetActive(true);

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
        if (creditsCanvas == null)
        {
            Engineson.print("ERROR: No Canvas object found");
            return;
        }

        if (creditsCanvas.IsActive() == true && creditsBGTransform != null)
        {
            float scrollInput = 0f;
            Vector2 leftStick = Input.GetLeftStick();

            if (Math.Abs(leftStick.Y) > 0.1f)
                scrollInput = leftStick.Y;
            else if (Input.GetControllerButton(ControllerButton.DPadUp))
                scrollInput = 1f;
            else if (Input.GetControllerButton(ControllerButton.DPadDown))
                scrollInput = -1f;

            if (scrollInput != 0f)
            {
                scrollY += scrollInput * scrollSpeed * deltaTime;
                scrollY = Clamp(scrollY, minScrollY, maxScrollY);

                creditsBGTransform.DOMoveYUI(scrollY, 0.1f, Modes.EASE_OUT);
            }

            if (Input.GetKeyDown(KeyCode.ESCAPE) || Input.GetControllerButtonDown(ControllerButton.B))
            {
                creditsCanvas.SetActive(false);
                Audio.PlayOneShot(ConfirmSFX);
                scrollY = 0f;
            }
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

    private void LoadLevel1()
    {

        FadeController fadeController = GameObject.Find("FadeController")?.GetComponent<FadeController>();
        fadeController.FadeIn(2.0f, () =>
        {
            fadeController.SetAlpha(0.0f);
            Audio.Stop(MainMenuMusic);
            SceneManager.LoadScene("BetaRelease_Week1_Lvl1");
        });
    }
}

