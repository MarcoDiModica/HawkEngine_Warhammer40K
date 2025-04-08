using HawkEngine;
using System;
using System.Numerics;


public class OptionMenu : MonoBehaviour
{
    private Audio sound;
    private string buttonClicked = "Assets/Audio/SFX/UI/UI_Click.wav";
    private GameObject pauseMenu;
    public override void Awake()
    {
        //Engineson.print("OptionMenu Awake");
    }
    public override void Start()
    {
        //Engineson.print("OptionMenu Start");
        sound = gameObject.GetComponent<Audio>();
        pauseMenu = GameObject.Find("Canvas_PauseMenu");

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
            sound?.LoadAudio(buttonClicked);
            sound?.Play();
        }

        //if (gamePlaycanvas.GetComponent<UIButton>().GetState() == ButtonState.HOVERED)
        //{
        //    gamePlaycanvas.GetComponent<UITransform>().DOScaleUI(new Vector3(1.1f, 1.1f, 1.1f), 0.2f,Modes.EASE_OUT);
        //}
    }

}
