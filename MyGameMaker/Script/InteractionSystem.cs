using HawkEngine;
using System;
using System.Numerics;
using System.Collections.Generic;

public class InteractionSystem : MonoBehaviour
{
    public float interactionRadius = 2.0f;
    private PlayerInput playerInput;

    public override void Start()
    {
        playerInput = gameObject.GetComponent<PlayerInput>();
        if (playerInput == null)
        {
  
        }
    }

    public override void Update(float deltaTime)
    {
        if (playerInput == null) return;

        if (Input.GetKeyDown(KeyCode.E) || Input.GetControllerButtonDown(ControllerButton.B))
        {
            CheckForInteractions();
        }
    }

    private void CheckForInteractions()
    {
        Vector3 position = gameObject.GetComponent<Transform>().position;
        GameObject[] overlappedObjects = Physics.OverlapSphere(position, interactionRadius, "Interactable");

        List<Item> interactables = new List<Item>();

        foreach (var obj in overlappedObjects)
        {
            Item interactable = obj.GetComponent<Item>();
            if (interactable != null)
            {
                interactables.Add(interactable);
            }
        }

        if (interactables.Count > 0)
        {
            HandleInteraction(interactables);
        }
    }

    private void HandleInteraction(List<Item> interactables)
    {
        interactables[0].Interact();
    }
}