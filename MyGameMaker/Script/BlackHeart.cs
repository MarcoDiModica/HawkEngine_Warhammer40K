using System;
using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public class BlackHeart : PickUp
{
    private Transform Transform;
    private Vector3 startPos;
    private float elapsedTime = 0f;
    public float floatSpeed = 1f;
    public float floatHeight = 0.5f;
    public float rotationSpeed = 50f;
    public float lifeTime = 10f;
    private const string PowerUpDown = "Assets/Audio/PowerUps/PowerUpDown.wav";
    private bool hasPlayedSound = false;
    public override void Awake()
    {

    }

    public override void Start()
    {
        Transform = gameObject.GetComponent<Transform>();
        startPos = Transform.position;

        rotationSpeed = (float)(new Random().NextDouble() * (100.0f - 30.0f) + 30.0f);

        floatSpeed = (float)(new Random().NextDouble() * (2.0f - 0.5f) + 0.5f);
    }

    public override void Update(float deltaTime)
    {
        elapsedTime += deltaTime;
        PowerUpMovment(elapsedTime, deltaTime);
        if (elapsedTime >= lifeTime && !hasPlayedSound)
        {
            Audio.PlayOneShot(PowerUpDown);
            hasPlayedSound = true;
            //Destroy();
        }
    }
   // public void Destroy()
   // {
   //     GameObject player = GameObject.Find("Player");
   //     Transform.position = new Vector3(0, -100, 0);
   //     //player.GetComponent<PickUpManager>().DestroyPickUp(gameObject);
   // }

    public void PowerUpMovment(float time, float dt)
    {
        float newY = startPos.Y + (float)Math.Sin(time * floatSpeed) * floatHeight;
        Transform.position = new Vector3(Transform.position.X, newY, Transform.position.Z);

        // Rotación del objeto
        Transform.Rotate((rotationSpeed * dt) * ((float)Math.PI / 180f), Vector3.UnitY);
    }
    public override void OnPickUp(PlayerController playerController)
    {
        Engineson.print("BlackHeart PowerUp applied");
        playerController.playerData.FullHealth();
        
    }
}