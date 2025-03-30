using HawkEngine;
using System;
using System.Numerics;
using System.Linq;

public class InteractionSystem : MonoBehaviour
{
    public float interactionRadius = 2.0f;
    private PlayerInput playerInput;
    private GameObject interactionMessage;
    private bool isShowingMessage = false; // Estado actual de la imagen

    public override void Start()
    {
        playerInput = gameObject.GetComponent<PlayerInput>();
        if (playerInput == null)
        {
            Engineson.print("ERROR: InteractionSystem requires PlayerInput");
        }

        interactionMessage = GameObject.Find("InteractText");
        if (interactionMessage == null)
        {
            Engineson.print("ERROR: InteractionSystem requires a GameObject named InteractText");
        }
        else
        {
            interactionMessage.GetComponent<UIImage>().SetImage("");
            isShowingMessage = false;
        }
    }

    public override void Update(float deltaTime)
    {
        if (playerInput == null) return;
        CheckForInteractions();
    }

    private void CheckForInteractions()
    {
        Transform transform = gameObject.GetComponent<Transform>();
        GameObject[] overlappedObjects = Physics.OverlapSphere(transform.position, interactionRadius, "Interactable");

        Item interactable = overlappedObjects
            .Select(obj => obj.GetComponent<Item>())
            .FirstOrDefault(item => item != null);

        if (interactionMessage != null)
        {
            if (interactable != null && !isShowingMessage)
            {
                interactionMessage.GetComponent<UIImage>().SetImage("Assets/Textures/PressE.png");
                isShowingMessage = true;
            }
            else if (interactable == null && isShowingMessage)
            {
                interactionMessage.GetComponent<UIImage>().SetImage("");
                isShowingMessage = false;
            }
        }

        if (interactable != null && playerInput.IsInteracting())
        {
            interactable.Interact();
        }
    }
}