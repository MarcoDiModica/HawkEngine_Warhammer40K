using HawkEngine;
using System;

public interface IInteractable
{
    bool hasInteracted { get; set; }
    void Interact();
}
