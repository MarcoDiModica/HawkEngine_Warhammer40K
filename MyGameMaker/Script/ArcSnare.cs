using System.Collections.Generic;
using System.Numerics;
using HawkEngine;
using static BaseWeapon;

public class ArcSnare : BaseAbilities
{

    public string name;
    public bool enabled;
    public float cooldown;
    private bool exploded = false;
    GameObject arcSnare;
    GameObject grenade;
    public bool canThrow = true;

    private float explosionCooldown = 1.0f;
    private float explosionTimer = 0.0f;

    private float abilityCooldown = 3.0f; // Cooldown de la habilidad
    private float abilityTimer = 0.0f;    // Contador del cooldown
    private float time = 0.0f;

    //private AudioSource sound;
    private string arcLaunch = "Assets/Audio/SFX/Weapons/Boltgun/ArcSnareThrow.wav";
    //AudioClip arcFX;


    public override void Awake()
    {

    }
    public override void Start()
    {
        //sound = gameObject.GetComponent<AudioSource>();
        //if (sound == null)
        //{
        //    Engineson.print("PlayerShooting: Audio component not found");
        //}

        //arcFX = new AudioClip(arcLaunch, "ArcLaunchFX", false, false);
        //sound.LoadAudioClip(arcFX);
    }

    public override void Update(float deltaTime)
    {
        // Manejo del cooldown de la habilidad
        if (!canThrow)
        {
            abilityTimer += deltaTime;
           // Engineson.print("Cooldown: " + abilityTimer + " / " + abilityCooldown);

            if (abilityTimer >= abilityCooldown)
            {
                Engineson.Destroy(arcSnare);
                canThrow = true;
                abilityTimer = 0.0f;
               // Engineson.print("Cooldown terminado. Habilidad lista.");
            }
        }


        // Manejo de la explosión (pero sin destruir la explosión)
        if (exploded)
        {
            explosionTimer += deltaTime;

            if (explosionTimer >= explosionCooldown)
            {
                exploded = false;
                explosionTimer = 0f;
            }
        }
    }



    public override void TriggerAbility()
    {
        if (canThrow)
        {
            Engineson.print("Lanzando granada...");
           // sound.Play(arcFX);
            grenade = Engineson.CreateGameObject("Arc", null);

            if (arcSnare == null)
            {
                return;
            }

            grenade.AddScript("Arc");
            grenade.GetComponent<Arc>().Init(gameObject.GetComponent<Transform>().GetPosition(), gameObject.GetComponent<Transform>().forward);
            grenade.AddComponent<AudioSource>();
            grenade.GetComponent<Arc>().Start();


            canThrow = false; // Inicia el cooldown
            abilityTimer = 0.0f;
        }

        if (!canThrow)
        {
            abilityTimer += time;

            if (abilityTimer >= abilityCooldown)
            {
                canThrow = true;
                abilityTimer = 0.0f;
            }
        }

    }

    public override void ResetCooldowns()
    {
        canThrow = true;
    }
}

