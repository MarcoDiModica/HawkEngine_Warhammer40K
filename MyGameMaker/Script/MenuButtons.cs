using HawkEngine;
using System;
using System.Numerics;

public class MenuButtons : MonoBehaviour
{
    private UIButton newGameButton;
    private UIButton continueButton;
    private UIButton optionsButton;
    private UIButton creditsButton;
    private UIButton quitButton;

    public override void Start()
    {
        newGameButton = GameObject.Find("NewGameButton").GetComponent<UIButton>();
        continueButton = GameObject.Find("ContinueButton").GetComponent<UIButton>();
        optionsButton = GameObject.Find("OptionsButton").GetComponent<UIButton>();
        creditsButton = GameObject.Find("CreditsButton").GetComponent<UIButton>();
        quitButton = GameObject.Find("QuitButton").GetComponent<UIButton>();

        if (newGameButton == null || optionsButton == null || creditsButton == null || quitButton == null)
        {
            Engineson.print("ERROR: No Button object found");
        }
    }

    public override void Update(float deltaTime)
    {
        if (newGameButton.GetState () == ButtonState.CLICKED)
        {
            Engineson.print("NewGame is Clicked");
        }

        if (continueButton.GetState() == ButtonState.CLICKED)
        {
            Engineson.print("Continue is Clicked");
        }

        if (optionsButton.GetState() == ButtonState.CLICKED)
        {
            Engineson.print("Options is Clicked");
        }

        if (creditsButton.GetState() == ButtonState.CLICKED)
        {
            Engineson.print("Credits is Clicked");
        }

        if (quitButton.GetState() == ButtonState.CLICKED)
        {
            Engineson.print("Quit is Clicked");
        }
    }
}

