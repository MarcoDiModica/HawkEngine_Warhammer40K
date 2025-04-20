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
    GameObject barrage;
    Rigidbody rigidbody;
    BoxCollider collider;
    bool canThrow = true;
    private float abilityCooldown = 3.0f; // Cooldown de la habilidad
    private float abilityTimer = 0.0f;    // Contador del cooldown
    private float time = 0.0f;
    private Audio sound;
    private string barrageSound = "Assets/Audio/SFX/Weapons/Shotgun/BarrageShot.wav";
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
    }
    public override void TriggerAbility()
    {
        if (canThrow)
        {
            Engineson.print("Lanzando granada...");
            sound.LoadAudio(barrageSound);
            sound.Play();

            barrage = Engineson.CreateGameObject("Barrage", null);

            if (barrage == null)
            {
                Engineson.print("ERROR: No se pudo crear la granada.");
                return;
            }

            barrage.AddScript("BarrageBullet");
            barrage.GetComponent<BarrageBullet>().Init(gameObject);

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

