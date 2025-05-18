using HawkEngine;
using System;
using System.Numerics;

public class Interaction : MonoBehaviour
{
    private GameObject Box;
    private GameObject Text;
    private GameObject interactText;
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

    public override void Awake()
    {

    }

    public override void Start()
    {
        Box = GameObject.Find("Box");
        Text = GameObject.Find("Text");
        interactText = GameObject.Find("InteractText");
    }

    public override void Update(float deltaTime)
    {
        timer += deltaTime;
        if (Input.GetKeyDown(KeyCode.X))
        {
            Audio.PlayOneShot(TextSFX);
            SpawnDialogueText(true);
        }

    }

    public bool isCanvasActive()
    {
        return Box != null && Text != null && Box.IsActive() && Text.IsActive();
    }
}
