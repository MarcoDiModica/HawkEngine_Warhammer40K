using HawkEngine;
using System;
using System.Numerics;

public class LoseScreen : MonoBehaviour
{
    private GameObject loadLastCheckpoint;
    private GameObject mainMenuButton;
    private GameObject quitButton;
    private UIButton button_loadLastCheckpoint;
    private UIButton button_mainMenuButton;
    private UIButton button_quitButton;
    private UITransform transform_loadLastCheckpoint;
    private UITransform transform_mainMenuButton;
    private UITransform transform_quitButton;

    //private AudioSource sound;
    private string ConfirmSFX = "Assets/Audio/UI/Confirm.wav";

    private ButtonState prevState_loadLastCheckpoint = ButtonState.DEFAULT;
    private ButtonState prevState_mainMenuButton = ButtonState.DEFAULT;
    private ButtonState prevState_quitButton = ButtonState.DEFAULT;

    private string buttonHovered = "Assets/Audio/SFX/UI/UI_Hover.wav";
    private string buttonClicked = "Assets/Audio/SFX/UI/UI_Click.wav";
    private string LoseMusic = "Assets/Audio/Music/Warhammer_LoseScreen.ogg";
    //     private AudioClip buttonHoveredFX;
    //     private AudioClip buttonClickedFX;

    private int selectedButtonIndex = -1;
    private UIButton[] buttons;
    private UITransform[] transforms;
    private bool[] hasPlayedHoverSound;
    private long lastInputTime = 0;

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
        loadLastCheckpoint = GameObject.Find("LLC_button");
        mainMenuButton = GameObject.Find("MM_button");
        quitButton = GameObject.Find("QUIT_button");
        //sound = gameObject.GetComponent<AudioSource>();
        Audio.Play(LoseMusic, true);
        button_loadLastCheckpoint = loadLastCheckpoint.GetComponent<UIButton>();
        button_mainMenuButton = mainMenuButton.GetComponent<UIButton>();
        button_quitButton = quitButton.GetComponent<UIButton>();

        transform_loadLastCheckpoint = loadLastCheckpoint.GetComponent<UITransform>();
        transform_mainMenuButton = mainMenuButton.GetComponent<UITransform>();
        transform_quitButton = quitButton.GetComponent<UITransform>();

        buttons = new UIButton[] { button_loadLastCheckpoint, button_mainMenuButton, button_quitButton };
        transforms = new UITransform[] { transform_loadLastCheckpoint, transform_mainMenuButton, transform_quitButton };

        hasPlayedHoverSound = new bool[buttons.Length];

        if (loadLastCheckpoint == null || mainMenuButton == null || quitButton == null)
        {
            Engineson.print("ERROR: No Button object found");
            return;
        }

//         if (sound == null)
//         {
//             Engineson.print("ERROR: Audio not found");
//             return;
//         }
//         buttonHoveredFX = new AudioClip(buttonHovered, "ButtonHoveredFX", false, false);
//         buttonClickedFX = new AudioClip(buttonClicked, "ButtonClickedFX", false, false);
//         sound.LoadAudioClip(buttonHoveredFX);
//         sound.LoadAudioClip(buttonClickedFX);
    }

    private void NavigateMenu()
    {
        if (buttons == null || buttons.Length == 0)
        {
            Engineson.print("ERROR: Buttons array is null or empty.");
            return;
        }
        long currentTime = DateTime.Now.Ticks;

        if (currentTime - lastInputTime < 2500000)
        {
            return;
        }

        Vector2 leftStick = Input.GetLeftStick();

        if (Math.Abs(leftStick.X) > 0.75f && currentInputMethod != InputMethod.DPad)
        {
            currentInputMethod = InputMethod.Joystick;

            if (leftStick.X < -0.75f)
            {
                selectedButtonIndex = (selectedButtonIndex - 1 + buttons.Length) % buttons.Length;
                lastInputTime = currentTime;
            }
            else if (leftStick.X > 0.75f)
            {
                selectedButtonIndex = (selectedButtonIndex + 1) % buttons.Length;
                lastInputTime = currentTime;
            }
        }
        else if ((Input.GetControllerButton(ControllerButton.DPadRight) || Input.GetControllerButton(ControllerButton.DPadLeft)) && currentInputMethod != InputMethod.Joystick)
        {
            currentInputMethod = InputMethod.DPad;

            if (Input.GetControllerButton(ControllerButton.DPadRight))
            {
                selectedButtonIndex = (selectedButtonIndex + 1) % buttons.Length;
                lastInputTime = currentTime;
            }
            else if (Input.GetControllerButton(ControllerButton.DPadLeft))
            {
                selectedButtonIndex = (selectedButtonIndex - 1 + buttons.Length) % buttons.Length;
                lastInputTime = currentTime;
            }
        }
        else if (Math.Abs(leftStick.X) <= 0.75f && !Input.GetControllerButton(ControllerButton.DPadRight) && !Input.GetControllerButton(ControllerButton.DPadLeft))
        {
            currentInputMethod = InputMethod.None;
        }

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
        if ((Input.GetMouseButtonDown(1) && currentInputMethod == InputMethod.Mouse) || Input.GetControllerButtonDown(ControllerButton.A))
        {
            if (selectedButtonIndex == -1)
                return;

            UIButton selectedButton = buttons[selectedButtonIndex];
            selectedButton.SetState(ButtonState.CLICKED);

            if (selectedButton == button_loadLastCheckpoint)
            {
                Audio.PlayOneShot(ConfirmSFX);
                if (SceneManager.isLevel2)
                {
                    SceneManager.LoadSceneFromCheckpoint("BetaRelease_Week1_Lvl2");
                }
                else if (SceneManager.isBossFight)
                {
                    SceneManager.LoadSceneFromCheckpoint("BetaRelease_Week1_Bossfight");
                }
                else
                {
                    SceneManager.LoadSceneFromCheckpoint("BetaRelease_Week1_Lvl1");
                }
            }
            else if (selectedButton == button_mainMenuButton)
            {
                Audio.PlayOneShot(ConfirmSFX);
                SceneManager.LoadScene("MainMenu");
            }
            else if (selectedButton == button_quitButton)
            {
                Audio.PlayOneShot(ConfirmSFX);
                // Aqu� puedes agregar la l�gica para salir del juego
            }
        }
    }
    private bool IsMouseOverButton(UIButton button)
    {
        return button.GetState() == ButtonState.HOVERED;
    }
    public override void Update(float deltaTime)
    {
        if (loadLastCheckpoint == null || mainMenuButton == null || quitButton == null)
        {
            Engineson.print("ERROR: No Button or object found");
            return;
        }

        NavigateMenu();
    }
}
