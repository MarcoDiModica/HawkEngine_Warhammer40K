using HawkEngine;
using System;
using System.Numerics;
using System.Linq;

public class InteractionSystem : MonoBehaviour
{
    public float interactionRadius = 2.0f;
    public float areaInteractionRadius = 3.0f;

    private PlayerInput playerInput;
    private PlayerMovement playerMovement;
    private UIImage interactionImage;
    private bool isInteracting = false;
    private GameObject currentInteractable = null;
    private AreaTrigger currentAreaTrigger = null;
    private bool interactionImageIsEnabled = false;

    public override void Awake()
    {
    }
    public override void Start()
    {
        playerInput = gameObject.GetComponent<PlayerInput>();
        if (playerInput == null)
        {
            Engineson.print("ERROR: InteractionSystem requires PlayerInput");
            return;
        }
        playerMovement = gameObject.GetComponent<PlayerMovement>();
        if (playerMovement == null)
        {
            Engineson.print("ERROR: InteractionSystem requires PlayerMovement");
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
        if(playerMovement.moveSpeed == 0) return; // Prevent interaction when player is not moving

        CheckForInteractions();
    }

    private void CheckForInteractions()
    {
        var transform = gameObject.GetComponent<Transform>();
        if (transform == null)
        {
            Engineson.print("InteractionSystem: Transform is missing.");
            return;
        }

        Vector3 position = transform.position;
        if (float.IsNaN(position.X) || float.IsNaN(position.Y) || float.IsNaN(position.Z))
        {
            Engineson.print("InteractionSystem: Invalid transform.position.");
            return;
        }

        var interactable = Physics.OverlapSphere(position, interactionRadius, "Interactable")
                                  .Select(obj => obj.GetComponent<Item>())
                                  .FirstOrDefault(i => i != null);

        //if (interactable != null)
        //{
        //    //HandleInteraction(interactable);
        //}
        //else
        //{
        //    ShowInteractionMessage(false);
        //}

        //HandleAreaTriggers(transform);
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