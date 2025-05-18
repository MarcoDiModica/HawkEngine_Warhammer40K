using HawkEngine;
using System;
using System.Numerics;

public class Item : MonoBehaviour, IInteractable
{
    public bool hasInteracted { get; set; }

    private GameObject itemTextMessage;
    public bool isTextVisible = false;
    public string text= "hola";

    public override void Start()
    {
    }
    public override void Awake() { }

    public override void Update(float deltaTime) { }

    public void Interact()
    {
        if (!isTextVisible)
        {
            isTextVisible = true;
        }
        else
        {
            isTextVisible = false;
        }
    }
}