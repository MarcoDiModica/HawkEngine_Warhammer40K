using HawkEngine;
using System;
using System.Collections.Generic;
using System.Numerics;


public class OptionMenu : MonoBehaviour
{
    private Audio sound;
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
    GameObject fullScreenTick;
    UIButton fullScreenButton;
    GameObject masterSlider;
    GameObject masterLeft;
    GameObject masterRight;
    UIButton masterLeftButton;
    UIButton masterRightButton;
    UITransform transform_masterSlider;
    int masterVolume = 100; // Default volume level
    float defaultSliderPos = 0.575f;
    float sliderScale = 0.009f;
    float sliderPos = 0.575f;




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
        sound = gameObject.GetComponent<Audio>();
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
        fullScreenTick = GameObject.Find("Fullscreen_tick");
        fullScreenButton = fullScreenCheckbox.GetComponent<UIButton>();
        masterSlider = GameObject.Find("Master_slider");
        masterLeft = GameObject.Find("Master_left");
        masterRight = GameObject.Find("Master_right");
        masterLeftButton = masterLeft.GetComponent<UIButton>();
        masterRightButton = masterRight.GetComponent<UIButton>();
        transform_masterSlider = masterSlider.GetComponent<UITransform>();
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
            sound?.LoadAudio(buttonClicked);
            sound?.Play();
        }

        if (rLeftButton.GetState() == ButtonState.CLICKED)
        {
            sound?.LoadAudio(buttonClicked);
            sound?.Play();
            PreviousResolution();
        }

        if (rRightButton.GetState() == ButtonState.CLICKED)
        {
            sound?.LoadAudio(buttonClicked);
            sound?.Play();
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
            sound?.LoadAudio(buttonClicked);
            sound?.Play();

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


        if (masterLeftButton.GetState() == ButtonState.CLICKED)
        {
            sound?.LoadAudio(buttonClicked);
            sound?.Play();
            if (masterVolume > 0)
            {
                masterVolume -= 10;
                sliderPos -= sliderScale;
                transform_masterSlider.DOMoveXUI(sliderPos, 0, Modes.LINEAR);
            }
        }

        if (masterRightButton.GetState() == ButtonState.CLICKED)
        {
            sound?.LoadAudio(buttonClicked);
            sound?.Play();
            if (masterVolume < 100)
            {
                masterVolume += 10;
                sliderPos += sliderScale;
                transform_masterSlider.DOMoveXUI(sliderPos, 0, Modes.LINEAR);
            }
        }
    }

}
