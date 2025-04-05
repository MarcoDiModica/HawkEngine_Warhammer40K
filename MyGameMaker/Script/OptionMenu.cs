using HawkEngine;
using System;
using System.Numerics;


public class OptionMenu : MonoBehaviour
{
    public override void Awake()
    {
        //Engineson.print("OptionMenu Awake");
    }
    public override void Start()
    {
        //Engineson.print("OptionMenu Start");

    }

    public override void Update(float deltaTime)
    {
        //Engineson.print("OptionMenu Update");
        if (Input.GetKeyDown(KeyCode.ESCAPE) || Input.GetControllerButtonDown(ControllerButton.B))
        {
            gameObject.SetActive(false);
        }

        //if (gamePlaycanvas.GetComponent<UIButton>().GetState() == ButtonState.HOVERED)
        //{
        //    gamePlaycanvas.GetComponent<UITransform>().DOScaleUI(new Vector3(1.1f, 1.1f, 1.1f), 0.2f,Modes.EASE_OUT);
        //}
    }

}
