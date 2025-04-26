using System.Collections.Generic;
using System.Numerics;
using HawkEngine;
using static BaseWeapon;

public class HookShot : BaseAbilities
{
    public string name;
    public bool enabled;
    public float cooldown;
    GameObject tp;

    private float yHeight = 0.0f;
    private float timer = 0;
    private bool exploded = false;
    GameObject grenade;
    Rigidbody rigidbody;
    BoxCollider collider;
    public bool canThrow = true;

    private float explosionCooldown = 1.0f;
    private float explosionTimer = 0.0f;

    private float abilityCooldown = 3.0f; // Cooldown de la habilidad
    private float abilityTimer = 0.0f;    // Contador del cooldown

   // private AudioSource sound;
    private string hookHit = "Assets/Audio/SFX/Weapons/Shotgun/HookLaunch.wav";
   // private AudioClip hookFX;


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

        //hookFX = new AudioClip(hookHit, "HookFX", false, false);
        //sound.LoadAudioClip(hookFX);
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

        if (rigidbody != null && !exploded && collider != null)
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
        if (!canThrow)
        {
            Engineson.print("Habilidad en cooldown. Espera...");
            return;
        }

        Engineson.print("Lanzando hook...");
       // sound.Play(hookFX);
        grenade = Engineson.CreateGameObject("Hook", null);

        if (grenade == null)
        {
            Engineson.print("ERROR: No se pudo crear el hook.");
            return;
        }

        grenade.AddScript("Hook");
        
        grenade.GetComponent<Hook>().Init(gameObject.GetComponent<Transform>().GetPosition(), gameObject.GetComponent<Transform>().forward);
        grenade.AddComponent<AudioSource>();
        grenade.GetComponent<Hook>().Start();
        

        canThrow = false;
        abilityTimer = 0.0f;
    }

    

    public override void ResetCooldowns()
    {
        canThrow = true;
    }
}
