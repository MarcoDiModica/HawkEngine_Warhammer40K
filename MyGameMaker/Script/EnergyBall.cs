using System.Collections.Generic;
using System.Numerics;
using HawkEngine;
using static BaseWeapon;

public class EnergyBall : BaseAbilities
{

    public string name;
    public bool enabled;
    public float cooldown;

    public GameObject energyBall;
    Rigidbody rigidbody;
    BoxCollider collider;
    public bool canThrow = true;


    private float abilityCooldown = 3.0f; // Cooldown de la habilidad
    private float abilityTimer = 0.0f;    // Contador del cooldown
    private float time = 0.0f;

    private bool energyBallActive = false;
    private float deathtimer = 3.0f;
    private float deathTimerPrevention = 0.0f;

    // private AudioSource sound;
    private string energyBallLaunch = "Assets/Audio/SFX/Weapons/Railgun/EnergyBallLaunch.wav";
    private string energyBallAudio= "Assets/Audio/SFX/Weapons/Railgun/EnergyBallMoving.wav";
    //private AudioClip launchFX;
    //private AudioClip energyFX;


    public override void Awake()
    {

    }
    public override void Start()
    {
        
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

        if (energyBallActive)
        {
            deathTimerPrevention += deltaTime;
            if (deathTimerPrevention >= deathtimer)
            {
                if (energyBall != null)
                {
                    energyBallActive = false;
                    Engineson.Destroy(energyBall);
                    Audio.Stop(energyBallAudio);
                    deathTimerPrevention = 0.0f;
                }
            }
        }
    }



    public override void TriggerAbility()
    {
        if (canThrow)
        {
            Engineson.print("Lanzando granada...");
            Audio.PlayOneShot(energyBallLaunch);
            energyBall = Engineson.CreateGameObject("energyBall", null);

            if (energyBall == null)
            {
                Engineson.print("ERROR: No se pudo crear la granada.");
                return;
            }
            energyBall.AddScript("Ball");
            energyBall.GetComponent<Ball>().Init(gameObject.GetComponent<Transform>().GetPosition(), gameObject.GetComponent<Transform>().forward);
            
            Audio.Play(energyBallAudio, true);
            canThrow = false;
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

