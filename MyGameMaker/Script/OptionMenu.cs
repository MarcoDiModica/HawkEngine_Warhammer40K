using HawkEngine;
using System;
using System.Numerics;


public class OptionMenu : MonoBehaviour
{
    private AudioSource sound;
    private string buttonClicked = "Assets/Audio/SFX/UI/UI_Click.wav";
    private AudioClip buttonClickedFX;
    private GameObject pauseMenu;
    public override void Awake()
    {
        //Engineson.print("OptionMenu Awake");
    }
    public override void Start()
    {
        //Engineson.print("OptionMenu Start");
        sound = gameObject.GetComponent<AudioSource>();
        pauseMenu = GameObject.Find("Canvas_PauseMenu");

        if (pauseMenu == null)
        {
            Engineson.print("ERROR: No PauseMenu object found");
            return;
        }

        if (sound == null)
        {
            Engineson.print("ERROR: Audio not found");
            return;
        }

        buttonClickedFX = new AudioClip(buttonClicked, "ButtonClickedFX", false, false);
        sound.LoadAudioClip(buttonClickedFX);

    }

    public override void Update(float deltaTime)
    {
        
        if (pauseMenu != null)
        {
            pauseMenu.SetActive(false);
        }
        //Engineson.print("OptionMenu Update");
        if (Input.GetKeyDown(KeyCode.ESCAPE) || Input.GetControllerButtonDown(ControllerButton.B))
        {
            if (pauseMenu != null)
            {
                pauseMenu.SetActive(true);
            }
            gameObject.SetActive(false);
            sound?.Play(buttonClickedFX);
        }

        //if (gamePlaycanvas.GetComponent<UIButton>().GetState() == ButtonState.HOVERED)
        //{
        //    gamePlaycanvas.GetComponent<UITransform>().DOScaleUI(new Vector3(1.1f, 1.1f, 1.1f), 0.2f,Modes.EASE_OUT);
        //}
    }

}
