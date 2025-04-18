using HawkEngine;
using System;
using System.Collections.Generic;
using System.Numerics;


public class OptionMenu : MonoBehaviour
{
    private Audio sound;
    private string buttonClicked = "Assets/Audio/SFX/UI/UI_Click.wav";
    private GameObject pauseMenu;
    private List<string> resolutions;
    private string currentResolution;





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
        //Engineson.print("OptionMenu Start");
        sound = gameObject.GetComponent<Audio>();
        pauseMenu = GameObject.Find("Canvas_PauseMenu");
        resolutions.Add("1280x720");
        resolutions.Add("1920x1080");
        resolutions.Add("2560x1440");
        resolutions.Add("3840x2160");
        currentResolution = resolutions[1];
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

        //if (gamePlaycanvas.GetComponent<UIButton>().GetState() == ButtonState.HOVERED)
        //{
        //    gamePlaycanvas.GetComponent<UITransform>().DOScaleUI(new Vector3(1.1f, 1.1f, 1.1f), 0.2f,Modes.EASE_OUT);
        //}
    }

}
