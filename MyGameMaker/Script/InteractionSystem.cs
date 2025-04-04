using HawkEngine;
using System.Numerics;
using System;

public class InteractionSystem : MonoBehaviour
{
    public float interactionRadius = 2.0f;
    public float areaInteractionRadius = 3.0f;
    public float scanCooldown = 0.1f;

    private float scanTimer = 0f;

    private PlayerInput playerInput;
    private UIImage interactionImage;
    private bool isInteracting = false;
    private GameObject currentInteractable = null;
    private AreaTrigger currentAreaTrigger = null;
    private bool interactionImageIsEnabled = false;

    private Item cachedInteractable = null;
    private AreaTrigger cachedAreaTrigger = null;


    public override void Awake()
    {
        base.Awake();
    }
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

        scanTimer += deltaTime;
        if (scanTimer >= scanCooldown)
        {
            ScanForNearbyObjects();
            scanTimer = 0f;
        }

        if (cachedInteractable != null)
            HandleInteraction(cachedInteractable);

        if (cachedAreaTrigger != null && playerInput.IsInteracting())
            cachedAreaTrigger.Interact();
    }

    private void ScanForNearbyObjects()
    {
        var transform = gameObject.GetComponent<Transform>();
        var allNearby = Physics.OverlapSphere(transform.position, Math.Max(interactionRadius, areaInteractionRadius), "All");

        cachedInteractable = null;
        cachedAreaTrigger = null;

        foreach (var obj in allNearby)
        {
            if (cachedInteractable == null)
            {
                var item = obj.GetComponent<Item>();
                if (item != null && Vector3.Distance(transform.position, obj.GetComponent<Transform>().position) <= interactionRadius)
                {
                    cachedInteractable = item;
                    continue;
                }
            }

            if (cachedAreaTrigger == null)
            {
                var area = obj.GetComponent<AreaTrigger>();
                if (area != null && Vector3.Distance(transform.position, obj.GetComponent<Transform>().position) <= areaInteractionRadius)
                {
                    cachedAreaTrigger = area;
                }
            }

            if (cachedInteractable != null && cachedAreaTrigger != null)
                break;
        }

        HandleAreaTextVisibility();
        ShowInteractionMessage(cachedInteractable != null && !isInteracting);
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
    }

    private void HandleAreaTextVisibility()
    {
        if (cachedAreaTrigger != currentAreaTrigger)
        {
            currentAreaTrigger?.SetTextVisibility(false);
            cachedAreaTrigger?.SetTextVisibility(true);
            currentAreaTrigger = cachedAreaTrigger;
        }

        if (cachedAreaTrigger == null && currentAreaTrigger != null)
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