using HawkEngine;
using System;
using System.Numerics;

public class Interaction : MonoBehaviour
{
    private GameObject Box;
    private GameObject Text;
    private GameObject interactText;
    private GameObject dialogueTextGo;
    private UIText dialogueText;
    private GameObject choice1;
    private UIButton choice1Button;
    private GameObject choice1Hover;
    private GameObject choice1TextGO;
    private UIText choice1Text;
    private GameObject choice2;
    private UIButton choice2Button;
    private GameObject choice2Hover;
    private GameObject choice2TextGO;
    private UIText choice2Text;
    bool hasChoices = false;
    private float timer = 0f;
    private string TextSFX = "Assets/Audio/UI/Dialog_Beeps.wav";

    public void SpawnDialogueText(bool condition)
    {
        if(condition)
        {
            Box.SetActive(true);
            Text.SetActive(true);
        }
        else
        {
            Box.SetActive(false);
            Text.SetActive(false);
        }
    }
    
    public void SpawnInteractText(bool condition)
    {
        if(condition)
        {
            interactText.SetActive(true);
        }
        else
        {
            interactText.SetActive(false);
        }
    }

    public void SetDialogueText(string text)
    {
        dialogueText.SetText(text);
    }

    public void SetChoice1Text(string text)
    {
        choice1Text.SetText(text);
    }

    public void SetChoice2Text(string text)
    {
        choice2Text.SetText(text);
    }


    public override void Awake()
    {

    }

    public override void Start()
    {
        Box = GameObject.Find("Box");
        Text = GameObject.Find("Text");
        interactText = GameObject.Find("InteractText");
        dialogueTextGo = GameObject.Find("DialogueText");
        dialogueText = dialogueTextGo.GetComponent<UIText>();
        choice1 = GameObject.Find("Choice1");
        choice1Button = choice1.GetComponent<UIButton>();
        choice1Hover = GameObject.Find("Choice1Hover");
        choice1TextGO = GameObject.Find("Choice1Text");
        choice1Text = choice1TextGO.GetComponent<UIText>();
        choice2 = GameObject.Find("Choice2");
        choice2Button = choice2.GetComponent<UIButton>();
        choice2Hover = GameObject.Find("Choice2Hover");
        choice2TextGO = GameObject.Find("Choice2Text");
        choice2Text = choice2TextGO.GetComponent<UIText>();

    }

    public override void Update(float deltaTime)
    {
        timer += deltaTime;
        if (Input.GetKeyDown(KeyCode.X))
        {
            Audio.PlayOneShot(TextSFX);
            SpawnDialogueText(true);
        }

        if (hasChoices)
        {
            choice1.SetActive(true);
            choice1TextGO.SetActive(true);
            choice2.SetActive(true);
            choice2TextGO.SetActive(true);
        }
        else
        {
            choice1.SetActive(false);
            choice1TextGO.SetActive(false);
            choice2.SetActive(false);
            choice2TextGO.SetActive(false);
        }

        if (choice1Button.GetState() == ButtonState.CLICKED)
        {
            Audio.PlayOneShot(TextSFX);
            choice1Button.SetState(ButtonState.DEFAULT);
            Engineson.print("Choice 1 clicked");
            //interaction.Choice1();
        }
        if (choice2Button.GetState() == ButtonState.CLICKED)
        {
            Audio.PlayOneShot(TextSFX);
            choice2Button.SetState(ButtonState.DEFAULT);
            Engineson.print("Choice 2 clicked");
            //interaction.Choice2();
        }
        if(choice1Button.GetState() == ButtonState.HOVERED)
        {
            choice1Hover.SetActive(true);
            choice2Hover.SetActive(false);
        }
        else if (choice2Button.GetState() == ButtonState.HOVERED)
        {
            choice2Hover.SetActive(true);
            choice1Hover.SetActive(false);
        }
        else
        {
            choice1Hover.SetActive(false);
            choice2Hover.SetActive(false);
        }
    }

    public bool isCanvasActive()
    {
        return Box != null && Text != null && Box.IsActive() && Text.IsActive();
    }
}
