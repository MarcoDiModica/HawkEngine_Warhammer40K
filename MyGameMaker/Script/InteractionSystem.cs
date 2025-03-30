using HawkEngine;
using System;
using System.Numerics;
using System.Linq;

public class InteractionSystem : MonoBehaviour
{
    public float interactionRadius = 2.0f;
    private PlayerInput playerInput;
    private GameObject interactionMessage;
    private UIImage interactionImage;
    private bool isShowingMessage = false;

    public override void Start()
    {
        playerInput = gameObject.GetComponent<PlayerInput>();
        if (playerInput == null)
        {
            Engineson.print("ERROR: InteractionSystem requires PlayerInput");
            return;
        }

        interactionMessage = GameObject.Find("InteractText");
        if (interactionMessage != null)
        {
            interactionImage = interactionMessage.GetComponent<UIImage>();
            if (interactionImage != null)
            {
                isShowingMessage = false;
                interactionImage.SetImageEnabled(false);
            }
            else
            {
                Engineson.print("ERROR: InteractionMessage does not have a UIImage component.");
            }
        }
        else
        {
            Engineson.print("ERROR: InteractionSystem requires a GameObject named InteractText.");
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

        Item interactable = null;
        foreach (var obj in overlappedObjects)
        {
            interactable = obj.GetComponent<Item>();
            if (interactable != null) break;
        }

        bool shouldShowMessage = interactable != null;

        if (interactionImage != null && shouldShowMessage != isShowingMessage)
        {
            isShowingMessage = shouldShowMessage;
            interactionImage.SetImageEnabled(isShowingMessage);
        }

        if (interactable != null && playerInput.IsInteracting())
        {
            interactable.Interact();
        }
    }
}