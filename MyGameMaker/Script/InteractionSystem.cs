using HawkEngine;
using System;
using System.Numerics;
using System.Linq;

public class InteractionSystem : MonoBehaviour
{
    public float interactionRadius = 2.0f;
    public float areaInteractionRadius = 3.0f;

    private PlayerInput playerInput;
    private UIImage interactionImage;
    private bool isInteracting = false;
    private GameObject currentInteractable = null;
    private AreaTrigger currentAreaTrigger = null;
    private bool interactionImageIsEnabled = false;

    public override void Start()
    {
        playerInput = gameObject.GetComponent<PlayerInput>();
        if (playerInput == null)
        {
            Engineson.print("ERROR: InteractionSystem requires PlayerInput");
            return;
        }

        var interactionMessage = GameObject.Find("InteractText");
        if (interactionMessage != null)
        {
            interactionImage = interactionMessage.GetComponent<UIImage>();
            interactionImage?.SetImageEnabled(false);
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
        var transform = gameObject.GetComponent<Transform>();

        var interactable = Physics.OverlapSphere(transform.position, interactionRadius, "Interactable")
                                  .Select(obj => obj.GetComponent<Item>())
                                  .FirstOrDefault(i => i != null);

        if (interactable != null)
        {
            HandleInteraction(interactable);
        }
        else
        {
            ShowInteractionMessage(false);
        }

        HandleAreaTriggers(transform);
    }

    private void HandleInteraction(Item interactable)
    {
        if (playerInput.IsInteracting())
        {
            if (!isInteracting)
            {
                isInteracting = true;
                ShowInteractionMessage(false);
                currentInteractable = interactable.gameObject;
                playerInput.BlockMovement();
                interactable.Interact();
            }
            else
            {
                isInteracting = false;
                playerInput.UnblockMovement();
                interactable.Interact();
            }
        }
        else if (!isInteracting)
        {
            ShowInteractionMessage(true);
        }
    }

    private void HandleAreaTriggers(Transform transform)
    {
        var areaObjects = Physics.OverlapSphere(transform.position, areaInteractionRadius, "AreaTrigger");

        if (areaObjects.Length > 0)
        {
            var newAreaTrigger = areaObjects[0].GetComponent<AreaTrigger>();
            if (currentAreaTrigger != newAreaTrigger)
            {
                currentAreaTrigger?.SetTextVisibility(false);
                newAreaTrigger?.SetTextVisibility(true);
                currentAreaTrigger = newAreaTrigger;
            }

            if (playerInput.IsInteracting() && currentAreaTrigger != null)
            {
                currentAreaTrigger.Interact();
            }
        }
        else if (currentAreaTrigger != null)
        {
            currentAreaTrigger.SetTextVisibility(false);
            currentAreaTrigger = null;
        }
    }

    private void ShowInteractionMessage(bool show)
    {
        if (interactionImage != null && show != interactionImageIsEnabled)
        {
            interactionImage.SetImageEnabled(show);
            interactionImageIsEnabled = show;
        }
    }
}