using HawkEngine;
using System;
using System.Numerics;

public class Item : MonoBehaviour, IInteractable
{
    public override void Awake() { }
    public override void Start() { }

    public override void Update(float deltaTime) { }

    public void Interact()
    {
        Engineson.print("Item picked up");
        Engineson.Destroy(gameObject);       
    }
}