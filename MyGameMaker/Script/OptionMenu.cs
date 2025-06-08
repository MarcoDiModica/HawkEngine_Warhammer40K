using HawkEngine;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;

public class OptionMenu : MonoBehaviour
{
    private GameObject pauseMenu;
    private List<string> resolutions = new List<string>();
    public string currentResolution;
    private string ConfirmSFX = "Assets/Audio/UI/Confirm.wav";

    private UITransform transform_masterSlider, transform_bgmSlider, transform_sfxSlider;
    private float sliderPosMaster = 0.575f, sliderPosBGM = 0.575f, sliderPosSFX = 0.575f;
    private float sliderScale = 0.009f;

    private enum InputMethod { None, Joystick, DPad, Mouse }
    private InputMethod currentInputMethod = InputMethod.None;
    private long lastInputTime = 0;

    private int selectedIndex = 0;

    private List<OptionItem> options;

    private GameObject r1280x720, r1920x1080, r2560x1440, r3840x2160;
    private GameObject fullScreenTick;

    public override void Start()
    {
        pauseMenu = GameObject.Find("Canvas_PauseMenu");
        resolutions.AddRange(new[] { "1280x720", "1920x1080", "2560x1440", "3840x2160" });
        currentResolution = resolutions[1];

        r1280x720 = GameObject.Find("Resolution_1280x720");
        r1920x1080 = GameObject.Find("Resolution_1920x1080");
        r2560x1440 = GameObject.Find("Resolution_2560x1440");
        r3840x2160 = GameObject.Find("Resolution_3840x2160");

        fullScreenTick = GameObject.Find("Fullscreen_tick");

        options = new List<OptionItem>
        {
            new OptionItem(GameObject.Find("Resolution_left").GetComponent<UIButton>(), () => ChangeResolution(-1), () => ChangeResolution(1)),
            new OptionItem(GameObject.Find("Fullscreen_checkbox").GetComponent<UIButton>(), ToggleFullscreen, ToggleFullscreen),
            new OptionItem(GameObject.Find("Master_left").GetComponent<UIButton>(), () => AdjustVolume("master", -8), () => AdjustVolume("master", 8)),
            new OptionItem(GameObject.Find("BGM_left").GetComponent<UIButton>(), () => AdjustVolume("bgm", -8), () => AdjustVolume("bgm", 8)),
            new OptionItem(GameObject.Find("SFX_left").GetComponent<UIButton>(), () => AdjustVolume("sfx", -8), () => AdjustVolume("sfx", 8)),
        };

        transform_masterSlider = GameObject.Find("Master_slider").GetComponent<UITransform>();
        transform_bgmSlider = GameObject.Find("BGM_slider").GetComponent<UITransform>();
        transform_sfxSlider = GameObject.Find("SFX_slider").GetComponent<UITransform>();
    }

    public override void Update(float deltaTime)
    {
        HandleNavigation();

        if (Input.GetKeyDown(KeyCode.ESCAPE) || Input.GetControllerButtonDown(ControllerButton.B))
        {
            pauseMenu?.SetActive(true);
            gameObject.SetActive(false);
            Audio.PlayOneShot(ConfirmSFX);
        }

        // Update resolution display
        r1280x720.SetActive(currentResolution == "1280x720");
        r1920x1080.SetActive(currentResolution == "1920x1080");
        r2560x1440.SetActive(currentResolution == "2560x1440");
        r3840x2160.SetActive(currentResolution == "3840x2160");
    }

    private void HandleNavigation()
    {
        long currentTime = DateTime.Now.Ticks;
        Vector2 stick = Input.GetLeftStick();

        if (Math.Abs(stick.Y) > 0.75f && currentTime - lastInputTime > 2500000)
        {
            selectedIndex = (selectedIndex + (stick.Y < 0 ? 1 : -1) + options.Count) % options.Count;
            lastInputTime = currentTime;
            currentInputMethod = InputMethod.Joystick;
        }
        else if ((Input.GetControllerButton(ControllerButton.DPadDown) || Input.GetControllerButton(ControllerButton.DPadUp)) && currentTime - lastInputTime > 2500000)
        {
            selectedIndex = (selectedIndex + (Input.GetControllerButton(ControllerButton.DPadDown) ? 1 : -1) + options.Count) % options.Count;
            lastInputTime = currentTime;
            currentInputMethod = InputMethod.DPad;
        }

        foreach (var (item, i) in options.Select((v, i) => (v, i)))
        {
            item.Button.SetState(i == selectedIndex ? ButtonState.HOVERED : ButtonState.DEFAULT);
        }

        if ((Math.Abs(stick.X) > 0.75f || Input.GetControllerButtonDown(ControllerButton.DPadRight) || Input.GetControllerButtonDown(ControllerButton.DPadLeft)) && currentTime - lastInputTime > 2500000)
        {
            if (stick.X > 0.75f || Input.GetControllerButtonDown(ControllerButton.DPadRight))
                options[selectedIndex].OnRight.Invoke();
            else if (stick.X < -0.75f || Input.GetControllerButtonDown(ControllerButton.DPadLeft))
                options[selectedIndex].OnLeft.Invoke();

            lastInputTime = currentTime;
        }
    }

    private void ChangeResolution(int delta)
    {
        int idx = resolutions.IndexOf(currentResolution);
        currentResolution = resolutions[(idx + delta + resolutions.Count) % resolutions.Count];
        Audio.PlayOneShot(ConfirmSFX);
    }

    private void ToggleFullscreen()
    {
        bool newState = !fullScreenTick.IsActive();
        fullScreenTick.SetActive(newState);
        Audio.PlayOneShot(ConfirmSFX);
    }

    private void AdjustVolume(string type, int delta)
    {
        switch (type)
        {
            case "master":
                Audio.MasterVolume = ClampVolume(Audio.MasterVolume + delta);
                sliderPosMaster += delta * sliderScale * 0.125f;
                transform_masterSlider.DOMoveXUI(sliderPosMaster, 0, Modes.LINEAR);
                break;
            case "bgm":
                Audio.MusicVolume = ClampVolume(Audio.MusicVolume + delta);
                sliderPosBGM += delta * sliderScale * 0.125f;
                transform_bgmSlider.DOMoveXUI(sliderPosBGM, 0, Modes.LINEAR);
                break;
            case "sfx":
                Audio.SfxVolume = ClampVolume(Audio.SfxVolume + delta);
                sliderPosSFX += delta * sliderScale * 0.125f;
                transform_sfxSlider.DOMoveXUI(sliderPosSFX, 0, Modes.LINEAR);
                break;
        }
        Audio.PlayOneShot(ConfirmSFX);
    }

    private float ClampVolume(float value)
    {
        if (value < -79) return -79;
        if (value > 1) return 1;
        return value;
    }

    private class OptionItem
    {
        public UIButton Button;
        public Action OnLeft;
        public Action OnRight;

        public OptionItem(UIButton button, Action onLeft, Action onRight)
        {
            Button = button;
            OnLeft = onLeft;
            OnRight = onRight;
        }
    }
}
