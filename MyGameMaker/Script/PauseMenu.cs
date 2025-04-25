using HawkEngine;
using System;
using System.Numerics;

public class PauseMenu : MonoBehaviour
{
    private GameObject optionsMenu;
    private GameObject resumeButton;
    private GameObject optionsMenuButton;
    private GameObject mainMenuButton;
    private GameObject quitButton;
    private GameObject HUD;
    private HUD HUDScript;

    private UIButton button_resumeButton;
    private UIButton button_optionsMenuButton;
    private UIButton button_mainMenuButton;
    private UIButton button_quitButton;

    private UITransform transform_resumeButton;
    private UITransform transform_optionsMenuButton;
    private UITransform transform_mainMenuButton;
    private UITransform transform_quitButton;

    private Audio sound;

    private ButtonState prevState_resumeButton = ButtonState.DEFAULT;
    private ButtonState prevState_optionsMenuButton = ButtonState.DEFAULT;
    private ButtonState prevState_mainMenuButton = ButtonState.DEFAULT;
    private ButtonState prevState_quitButton = ButtonState.DEFAULT;

    private string buttonHovered = "Assets/Audio/SFX/UI/UI_Hover.wav";
    private string buttonClicked = "Assets/Audio/SFX/UI/UI_Click.wav";

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
        optionsMenu = GameObject.Find("Canvas_OptionsMenu");
        resumeButton = GameObject.Find("Resume_Button");
        optionsMenuButton = GameObject.Find("Options_Button");
        mainMenuButton = GameObject.Find("MainMenu_Button");
        quitButton = GameObject.Find("Exit_Button");
        sound = gameObject.GetComponent<Audio>();

        button_resumeButton = resumeButton.GetComponent<UIButton>();
        button_optionsMenuButton = optionsMenuButton.GetComponent<UIButton>();
        button_mainMenuButton = mainMenuButton.GetComponent<UIButton>();
        button_quitButton = quitButton.GetComponent<UIButton>();

        transform_resumeButton = resumeButton.GetComponent<UITransform>();
        transform_optionsMenuButton = optionsMenuButton.GetComponent<UITransform>();
        transform_mainMenuButton = mainMenuButton.GetComponent<UITransform>();
        transform_quitButton = quitButton.GetComponent<UITransform>();

        buttons = new UIButton[] { button_resumeButton, button_optionsMenuButton, button_mainMenuButton, button_quitButton };
        transforms = new UITransform[] { transform_resumeButton, transform_optionsMenuButton, transform_mainMenuButton, transform_quitButton };

        hasPlayedHoverSound = new bool[buttons.Length];

        if (resumeButton == null || optionsMenuButton == null || mainMenuButton == null || quitButton == null)
        {
            Engineson.print("ERROR: No Button object found");
            return;
        }

        if (optionsMenu == null)
        {
            Engineson.print("ERROR: No Canvas object found");
            return;
        }
        else
        {
            optionsMenu.SetActive(false);
        }

        HUD = GameObject.Find("Canvas_HUD");
        if (HUD == null)
        {
            Engineson.print("ERROR: HUD not found");
            return;
        }
        HUDScript = HUD.GetComponent<HUD>();
        if (HUDScript == null)
        {
            Engineson.print("ERROR: HUDScript not found");
            return;
        }
        if (sound == null)
        {
            Engineson.print("ERROR: Sound not found");
            return;
        }

        this.gameObject.SetActive(false);
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

        // Detectar si el ratón está sobre un botón
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
            if (currentTime - lastInputTime > 20000000) // 2 segundos en ticks (1 segundo = 10,000,000 ticks)
            {
                if (currentInputMethod == InputMethod.None)
                {
                    selectedButtonIndex = -1;
                }
            }
        }

        // Detectar clic del ratón
        if ((Input.GetMouseButtonDown(1) && currentInputMethod == InputMethod.Mouse && selectedButtonIndex != -1) || Input.GetControllerButtonDown(ControllerButton.A))
        {
            UIButton selectedButton = buttons[selectedButtonIndex];
            selectedButton.SetState(ButtonState.CLICKED);

            if (selectedButton == button_resumeButton)
            {
                sound?.LoadAudio(buttonClicked);
                sound?.Play();
                gameObject.SetActive(false);
            }
            else if (selectedButton == button_optionsMenuButton)
            {
                sound?.LoadAudio(buttonClicked);
                sound?.Play();
                optionsMenu.SetActive(true);
                gameObject.SetActive(false);
            }
            else if (selectedButton == button_mainMenuButton)
            {
                sound?.LoadAudio(buttonClicked);
                sound?.Play();
                SceneManager.LoadScene("MainMenu");
            }
            else if (selectedButton == button_quitButton)
            {
                sound?.LoadAudio(buttonClicked);
                sound?.Play();
                // Aquí puedes agregar la lógica para salir del juego
            }
        }
    }
    private bool IsMouseOverButton(UIButton button)
    {
        return button.GetState() == ButtonState.HOVERED;
    }
    public override void Update(float deltaTime)
    {
        if (optionsMenu == null || resumeButton == null || optionsMenuButton == null || mainMenuButton == null || quitButton == null)
        {
            Engineson.print("ERROR: No Button or Canvas object found");
            return;
        }

        if (HUD == null)
        {
            Engineson.print("ERROR: HUD not found");
            return;
        }

        if (sound == null)
        {
            Engineson.print("ERROR: Sound not found");
            return;
        }

        NavigateMenu();
    }
}
