using HawkEngine;
using System;
using System.Collections.Generic;
using System.Numerics;


public class OptionMenu : MonoBehaviour
{
    //private Audio sound;
    private string buttonClicked = "Assets/Audio/SFX/UI/UI_Click.wav";
    private GameObject pauseMenu;
    private List<string> resolutions = new List<string>();
    public string currentResolution;
    GameObject r1280x720;
    GameObject r1920x1080;
    GameObject r2560x1440;
    GameObject r3840x2160;
    GameObject rLeft;
    GameObject rRight;
    UIButton rLeftButton;
    UIButton rRightButton;

    GameObject fullScreenCheckbox;
    GameObject fullScreenCheckboxHover;
    GameObject fullScreenTick;
    UIButton fullScreenButton;

    float defaultSliderPos = 0.575f;

    GameObject masterSlider;
    GameObject masterLeft;
    GameObject masterRight;
    GameObject masterLeftHover;
    GameObject masterRightHover;
    UIButton masterLeftButton;
    UIButton masterRightButton;
    UITransform transform_masterSlider;
    int masterVolume = 100; // Default volume level
    float sliderScale = 0.009f;
    float sliderPosMaster = 0.575f;

    GameObject bgmSlider;
    GameObject bgmLeft;
    GameObject bgmRight;
    GameObject bgmLeftHover;
    GameObject bgmRightHover;
    UIButton bgmLeftButton;
    UIButton bgmRightButton;
    UITransform transform_bgmSlider;
    int bgmVolume = 100; // Default volume level
    float sliderPosBGM = 0.575f;

    private string ConfirmSFX = "Assets/Audio/UI/Confirm.wav";
    GameObject sfxSlider;
    GameObject sfxLeft;
    GameObject sfxRight;
    GameObject sfxLeftHover;
    GameObject sfxRightHover;
    UIButton sfxLeftButton;
    UIButton sfxRightButton;
    UITransform transform_sfxSlider;
    int sfxVolume = 100; // Default volume level
    float sliderPosSFX = 0.575f;




    private void NextResolution()
    {
        int currentIndex = resolutions.IndexOf(currentResolution);
        int nextIndex = (currentIndex + 1) % resolutions.Count;
        currentResolution = resolutions[nextIndex];
        // Set the resolution in the game engine
        // Example: GameEngine.SetResolution(currentResolution);
    }

    private void PreviousResolution()
    {
        int currentIndex = resolutions.IndexOf(currentResolution);
        int previousIndex = (currentIndex - 1 + resolutions.Count) % resolutions.Count;
        currentResolution = resolutions[previousIndex];
        // Set the resolution in the game engine
        // Example: GameEngine.SetResolution(currentResolution);
    }


    public override void Awake()
    {
        //Engineson.print("OptionMenu Awake");
    }
    public override void Start()
    {
        Engineson.print("OptionMenu Start");
        //sound = gameObject.GetComponent<Audio>();
        pauseMenu = GameObject.Find("Canvas_PauseMenu");
        resolutions.Add("1280x720");
        resolutions.Add("1920x1080");
        resolutions.Add("2560x1440");
        resolutions.Add("3840x2160");
        currentResolution = resolutions[1];

        r1280x720 = GameObject.Find("Resolution_1280x720");
        r1920x1080 = GameObject.Find("Resolution_1920x1080");
        r2560x1440 = GameObject.Find("Resolution_2560x1440");
        r3840x2160 = GameObject.Find("Resolution_3840x2160");
        rLeft = GameObject.Find("Resolution_left");
        rRight = GameObject.Find("Resolution_right");
        rLeftButton = rLeft.GetComponent<UIButton>();
        rRightButton = rRight.GetComponent<UIButton>();
        fullScreenCheckbox = GameObject.Find("Fullscreen_checkbox");
        fullScreenCheckboxHover = GameObject.Find("Fullscreen_checkboxhover");
        fullScreenTick = GameObject.Find("Fullscreen_tick");
        fullScreenButton = fullScreenCheckbox.GetComponent<UIButton>();


        masterSlider = GameObject.Find("Master_slider");
        masterLeft = GameObject.Find("Master_left");
        masterRight = GameObject.Find("Master_right");
        masterLeftHover = GameObject.Find("Master_lefthover");
        masterRightHover = GameObject.Find("Master_righthover");
        masterLeftButton = masterLeft.GetComponent<UIButton>();
        masterRightButton = masterRight.GetComponent<UIButton>();
        transform_masterSlider = masterSlider.GetComponent<UITransform>();

        bgmSlider = GameObject.Find("BGM_slider");
        bgmLeft = GameObject.Find("BGM_left");
        bgmRight = GameObject.Find("BGM_right");
        bgmLeftHover = GameObject.Find("BGM_lefthover");
        bgmRightHover = GameObject.Find("BGM_righthover");
        bgmLeftButton = bgmLeft.GetComponent<UIButton>();
        bgmRightButton = bgmRight.GetComponent<UIButton>();
        transform_bgmSlider = bgmSlider.GetComponent<UITransform>();

        sfxSlider = GameObject.Find("SFX_slider");
        sfxLeft = GameObject.Find("SFX_left");
        sfxRight = GameObject.Find("SFX_right");
        sfxLeftHover = GameObject.Find("SFX_lefthover");
        sfxRightHover = GameObject.Find("SFX_righthover");
        sfxLeftButton = sfxLeft.GetComponent<UIButton>();
        sfxRightButton = sfxRight.GetComponent<UIButton>();
        transform_sfxSlider = sfxSlider.GetComponent<UITransform>();

    }


    public override void Update(float deltaTime)
    {
        //Engineson.print("OptionMenu Update");
        if (Input.GetKeyDown(KeyCode.ESCAPE) || Input.GetControllerButtonDown(ControllerButton.B))
        {
            if (pauseMenu != null)
            {
                pauseMenu.SetActive(true);
            }
            gameObject.SetActive(false);
            Audio.PlayOneShot(ConfirmSFX);
        }

        if (rLeftButton.GetState() == ButtonState.CLICKED)
        {
            Audio.PlayOneShot(ConfirmSFX);
            PreviousResolution();
        }

        if (rRightButton.GetState() == ButtonState.CLICKED)
        {
            Audio.PlayOneShot(ConfirmSFX);
            NextResolution();
        }

        switch (currentResolution)
        {
            case "1280x720":
                r1280x720.SetActive(true);
                r1920x1080.SetActive(false);
                r2560x1440.SetActive(false);
                r3840x2160.SetActive(false);
                break;
            case "1920x1080":
                r1280x720.SetActive(false);
                r1920x1080.SetActive(true);
                r2560x1440.SetActive(false);
                r3840x2160.SetActive(false);
                break;
            case "2560x1440":
                r1280x720.SetActive(false);
                r1920x1080.SetActive(false);
                r2560x1440.SetActive(true);
                r3840x2160.SetActive(false);
                break;
            case "3840x2160":
                r1280x720.SetActive(false);
                r1920x1080.SetActive(false);
                r2560x1440.SetActive(false);
                r3840x2160.SetActive(true);
                break;
        }

        if (fullScreenButton.GetState() == ButtonState.CLICKED)
        {
            Audio.PlayOneShot(ConfirmSFX);

            if (fullScreenTick.IsActive())
            {
                fullScreenTick.SetActive(false);
                // Set windowed mode
                // Example: GameEngine.SetFullscreen(false);
            }
            else
            {
                fullScreenTick.SetActive(true);
                // Set fullscreen mode
                // Example: GameEngine.SetFullscreen(true);
            }
        }

        if (fullScreenButton.GetState() == ButtonState.HOVERED)
        {
            fullScreenCheckboxHover.SetActive(true);
        }
        else
        {
            fullScreenCheckboxHover.SetActive(false);
        }


        if (masterLeftButton.GetState() == ButtonState.CLICKED)
        {
            Audio.PlayOneShot(ConfirmSFX);

            if (Audio.MasterVolume > -79)
            {
                Audio.MasterVolume -= 8.0f;
                sliderPosMaster -= sliderScale;
                transform_masterSlider.DOMoveXUI(sliderPosMaster, 0, Modes.LINEAR);
            }
        }


        if(masterLeftButton.GetState() == ButtonState.HOVERED)
        {
            masterLeftHover.SetActive(true);
        }
        else
        {
            masterLeftHover.SetActive(false);
        }

        if (masterRightButton.GetState() == ButtonState.CLICKED)
        {
            Audio.PlayOneShot(ConfirmSFX);

            if (Audio.MasterVolume < 1)
            {
                Audio.MasterVolume += 8.0f;
                sliderPosMaster += sliderScale;
                transform_masterSlider.DOMoveXUI(sliderPosMaster, 0, Modes.LINEAR);
            }
        }


        if(masterRightButton.GetState() == ButtonState.HOVERED)
        {
            masterRightHover.SetActive(true);
        }
        else
        {
            masterRightHover.SetActive(false);
        }

        if (bgmLeftButton.GetState() == ButtonState.CLICKED)
        {
            Audio.PlayOneShot(ConfirmSFX);

            if (Audio.MusicVolume > -79)
            {
                Audio.MusicVolume -= 8.0f;
                sliderPosBGM -= sliderScale;
                transform_bgmSlider.DOMoveXUI(sliderPosBGM, 0, Modes.LINEAR);
            }
        }

        if (bgmRightButton.GetState() == ButtonState.CLICKED)
        {
            Audio.PlayOneShot(ConfirmSFX);

            if (Audio.MusicVolume < 1)
            {
                Audio.MusicVolume += 8.0f;
                sliderPosBGM += sliderScale;
                transform_bgmSlider.DOMoveXUI(sliderPosBGM, 0, Modes.LINEAR);
            }
        }

        if (bgmLeftButton.GetState() == ButtonState.HOVERED)
        {
            bgmLeftHover.SetActive(true);
        }
        else
        {
            bgmLeftHover.SetActive(false);
        }


        if (bgmRightButton.GetState() == ButtonState.HOVERED)
        {
            bgmRightHover.SetActive(true);
        }
        else
        {
            bgmRightHover.SetActive(false);
        }

        if (sfxLeftButton.GetState() == ButtonState.CLICKED)
        {
            Audio.PlayOneShot(ConfirmSFX);

            if (Audio.SfxVolume > -79)
            {
                Audio.SfxVolume -= 8.0f;
                sliderPosSFX -= sliderScale;
                transform_sfxSlider.DOMoveXUI(sliderPosSFX, 0, Modes.LINEAR);
            }
        }

        if (sfxRightButton.GetState() == ButtonState.CLICKED)
        {
            Audio.PlayOneShot(ConfirmSFX);

            if (Audio.SfxVolume < 1)
            {
                Audio.SfxVolume += 8.0f;
                sliderPosSFX += sliderScale;
                transform_sfxSlider.DOMoveXUI(sliderPosSFX, 0, Modes.LINEAR);
            }
        }

        if (sfxLeftButton.GetState() == ButtonState.HOVERED)
        {
            sfxLeftHover.SetActive(true);
        }
        else
        {
            sfxLeftHover.SetActive(false);
        }

        if (sfxRightButton.GetState() == ButtonState.HOVERED)
        {
            sfxRightHover.SetActive(true);
        }
        else
        {
            sfxRightHover.SetActive(false);
        }

    }

}
