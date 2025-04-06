using System;
using System.Numerics;
using HawkEngine;

public class BoltgunPickUp : PickUp
{
    private Transform Transform;
    private Vector3 startPos;
    private float elapsedTime = 0f;
    public float floatSpeed = 1f;
    public float floatHeight = 0.5f;
    public float rotationSpeed = 50f;
    public override void Awake()
    {

    }
    public override void Start()
    {
        Transform = gameObject.GetComponent<Transform>();
        startPos = Transform.position;
    }

    public override void Update(float deltaTime)
    {
        elapsedTime += deltaTime;
        PowerUpMovment(elapsedTime, deltaTime);
    }

    public void PowerUpMovment(float time, float dt)
    {
        float newY = startPos.Y + (float)Math.Sin(time * floatSpeed) * floatHeight;
        Transform.position = new Vector3(Transform.position.X, newY, Transform.position.Z);
        Transform.Rotate((rotationSpeed * dt) * ((float)Math.PI / 180f), Vector3.UnitY);
    }

    public override void OnPickUp(PlayerController playerController)
    {
        if (!playerController.playerShooting.hasBoltgun)
        {
            Engineson.print("Boltgun Picked Up");
            playerController.playerShooting.hasBoltgun = true;
        }
        else
        {
            Engineson.print("Player already has Boltgun");
            
        }
    }
}
