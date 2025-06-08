using System.Collections.Generic;
using System.Numerics;
using HawkEngine;
using static BaseWeapon;

public class Barrage : BaseAbilities
{
    public string name;
    public bool enabled;
    public float cooldown;

    private HUD hud;
    private float yHeight = 0.0f;
    private float timer = 0;

    public GameObject grenade;
    Rigidbody rigidbody;
    BoxCollider collider;
    public bool canThrow = true;

    private float explosionCooldown = 1.0f;
    private float explosionTimer = 0.0f;

    private float abilityCooldown = 3.0f;
    private float abilityTimer = 0.0f;
    private float time = 0.0f;

    private string barrageShotAudio = "Assets/Audio/SFX/Weapons/Shotgun/BarrageShot.wav";

    public override void Awake()
    {
    }

    public override void Start()
    {
        hud = GameObject.Find("Canvas_HUD").GetComponent<HUD>();
        if (hud == null)
        {
            Engineson.print("ERROR: HUD no encontrado.");
        }
    }

    public override void Update(float deltaTime)
    {
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
                    // Evento de colisión o detonación aquí si fuera necesario
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

        Engineson.print("Lanzando granada...");
        Audio.PlayOneShot(barrageShotAudio);

        grenade = Engineson.CreateGameObject("Barrage", null);

        if (grenade == null)
        {
            Engineson.print("ERROR: No se pudo crear la granada.");
            return;
        }

        grenade.AddScript("BarrageBullet");
        grenade.GetComponent<BarrageBullet>().Init(gameObject);

        canThrow = false;
        abilityTimer = 0.0f;

        if (hud != null)
        {
            hud.TriggerCooldown("Barrage",abilityCooldown);
            Engineson.print("HUD cooldown disparado por " + abilityCooldown + "s.");
        }
        else
        {
            Engineson.print("HUD es null, no se puede mostrar cooldown.");
        }
    }

    public override void ResetCooldowns()
    {
        canThrow = true;
        abilityTimer = 0.0f;
    }
}

