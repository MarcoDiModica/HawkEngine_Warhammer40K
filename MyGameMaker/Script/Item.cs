using HawkEngine;
using System;
using System.Numerics;

public class Item : MonoBehaviour, IInteractable
{
    public bool hasInteracted { get; set; }

    private GameObject itemTextMessage;
    private UIImage itemTextImage;
    private bool isTextVisible = false;

    public override void Start()
    {
        itemTextMessage = GameObject.Find("dialogueText"); 
        if (itemTextMessage != null)
        {
            itemTextImage = itemTextMessage.GetComponent<UIImage>();
            if (itemTextImage != null)
            {
                itemTextImage.SetImageEnabled(false);
            }
            else
            {
                Engineson.print("ERROR: Text does not have a UIImage component.");
            }
        }
        else
        {
            Engineson.print("ERROR: InteractionSystem requires a GameObject named dialogueText.");
        }
    }
    public override void Awake() { }

    public override void Update(float deltaTime) { }

    public void Interact()
    {
        if (!isTextVisible)
        {
            isTextVisible = true;
            itemTextImage?.SetImageEnabled(true);
        }
        else
        {
            isTextVisible = false;
            itemTextImage?.SetImageEnabled(false);
        }
    }
}