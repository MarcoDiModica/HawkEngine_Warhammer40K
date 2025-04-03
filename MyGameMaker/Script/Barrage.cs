using System.Collections.Generic;
using System.Numerics;
using HawkEngine;
using static BaseWeapon;

public class Barrage : BaseAbilities
{

    public string name;
    public bool enabled;
    public float cooldown;
    

    private float yHeight = 0.0f;
    private float timer = 0;
    
    GameObject grenade;
    Rigidbody rigidbody;
    BoxCollider collider;
    bool canThrow = true;

    private float explosionCooldown = 1.0f;
    private float explosionTimer = 0.0f;

    private float abilityCooldown = 3.0f; // Cooldown de la habilidad
    private float abilityTimer = 0.0f;    // Contador del cooldown
    private float time = 0.0f;

    private Audio sound;
    private string barrage = "Assets/Audio/SFX/Weapons/Shotgun/BarrageShot.wav";
    


    public override void Awake()
    {

    }
    public override void Start()
    {
        sound = gameObject.GetComponent<Audio>();
        if (sound == null)
        {
            Engineson.print("PlayerShooting: Audio component not found");
        }
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
            sound.LoadAudio(barrage);
            sound.Play();

            grenade = Engineson.CreateGameObject("Barrage", null);



            if (grenade == null)
            {
                Engineson.print("ERROR: No se pudo crear la granada.");
                return;
            }




            grenade.AddScript("BarrageBullet");
            grenade.GetComponent<BarrageBullet>().Init(gameObject.GetComponent<Transform>().GetPosition(), gameObject.GetComponent<Transform>().forward);


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

