using System.Collections.Generic;
using System.Numerics;
using HawkEngine;
using static BaseWeapon;

public class EnergyBall : BaseAbilities
{

    public string name;
    public bool enabled;
    public float cooldown;


    private float yHeight = 0.0f;
    private float timer = 0;

    public GameObject grenade;
    Rigidbody rigidbody;
    BoxCollider collider;
    public bool canThrow = true;

    private float explosionCooldown = 1.0f;
    private float explosionTimer = 0.0f;

    private float abilityCooldown = 3.0f; // Cooldown de la habilidad
    private float abilityTimer = 0.0f;    // Contador del cooldown
    private float time = 0.0f;

    private AudioSource sound;
    private string energyBallLaunch = "Assets/Audio/SFX/Weapons/Railgun/EnergyBallLaunch.wav";
    private string energyBall= "Assets/Audio/SFX/Weapons/Railgun/EnergyBallMoving.wav";
    private AudioClip launchFX;
    private AudioClip energyFX;


    public override void Awake()
    {

    }
    public override void Start()
    {
        sound = gameObject.GetComponent<AudioSource>();
        if (sound == null)
        {
            Engineson.print("PlayerShooting: Audio component not found");
        }

        launchFX = new AudioClip(energyBallLaunch, "EnergyBallLaunchFX", false, false);
        energyFX = new AudioClip(energyBall, "EnergyBallFX", false, false);
        sound.LoadAudioClip(launchFX);
        sound.LoadAudioClip(energyFX);
    }

    public override void Update(float deltaTime)
    {
        // Manejo del cooldown de la habilidad
        if (!canThrow)
        {
            abilityTimer += deltaTime;
            Engineson.print("Cooldown: " + abilityTimer + " / " + abilityCooldown);

            if (abilityTimer >= abilityCooldown)
            {
                canThrow = true;
                abilityTimer = 0.0f;
                Engineson.print("Cooldown terminado. Habilidad lista.");
            }
        }

        if (rigidbody != null && collider != null)
        {
            timer += deltaTime;

            if (rigidbody.GetVelocity() != null && grenade != null && grenade.GetComponent<Transform>() != null)
            {
                float grenadeY = grenade.GetComponent<Transform>().GetPosition().Y;

                if (rigidbody.GetVelocity().Y <= 0.1f && timer > 0.1f && yHeight > grenadeY)
                {

                }
            }
        }


    }



    public override void TriggerAbility()
    {
        if (canThrow)
        {
            Engineson.print("Lanzando granada...");
            sound.Play(launchFX);
            grenade = Engineson.CreateGameObject("energyBall", null);

            if (grenade == null)
            {
                Engineson.print("ERROR: No se pudo crear la granada.");
                return;
            }

            grenade.AddScript("Ball");
            grenade.GetComponent<Ball>().Init(gameObject.GetComponent<Transform>().GetPosition(), gameObject.GetComponent<Transform>().forward);
            sound.Play(energyFX);

            canThrow = false; // Inicia el cooldown
            abilityTimer = 0.0f;
        }
        else
        {
            Engineson.print("Habilidad en cooldown. Espera...");
        }

        if (!canThrow)
        {
            abilityTimer += time;
            Engineson.print("Cooldown: " + abilityTimer + " / " + abilityCooldown);

            if (abilityTimer >= abilityCooldown)
            {
                canThrow = true;
                abilityTimer = 0.0f;
                Engineson.print("Cooldown terminado. Habilidad lista.");
            }
        }

    }

    public override void ResetCooldowns()
    {
        canThrow = true;
    }


}

