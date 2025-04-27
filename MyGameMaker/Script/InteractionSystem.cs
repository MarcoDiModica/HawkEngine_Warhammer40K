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
    private GameObject canvas;
    private Interaction interaction;
    private Transform cachedTransform;

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

        cachedTransform = gameObject.GetComponent<Transform>();
        if (cachedTransform == null)
        {
            Engineson.print("ERROR: InteractionSystem requires Transform");
            return;
        }
        canvas = GameObject.Find("Canvas_Interaction");
        if (canvas == null)
        {
            Engineson.print("ERROR: InteractionSystem requires a GameObject named Canvas_Interaction.");
            return;
        }
        interaction = canvas.GetComponent<Interaction>();   
    }

    public override void Update(float deltaTime)
    {
        if (playerInput == null) return;

        CheckForInteractions();
    }

    private void CheckForInteractions()
    {
        Vector3 position = cachedTransform.position;

        var interactable = Physics.OverlapSphere(position, interactionRadius, "Interactable")
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

        HandleAreaTriggers(position);
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

    private void HandleAreaTriggers(Vector3 position)
    {
        var areaObjects = Physics.OverlapSphere(position, areaInteractionRadius, "AreaTrigger");

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
        interaction.Spawn();
    }
}