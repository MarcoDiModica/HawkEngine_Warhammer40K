using System.Collections.Generic;
using System.Numerics;
using HawkEngine;
using static BaseWeapon;

public class LaserBeam : BaseAbilities
{
    public string name;
    public bool enabled;
    public float cooldown;
    private float yHeight = 0.0f;
    private float timer = 0;
    GameObject laserBeam;
    Rigidbody rigidbody;
    BoxCollider collider;
    bool canThrow = true;
    private float abilityCooldown = 3.0f; // Cooldown de la habilidad
    private float abilityTimer = 0.0f;    // Contador del cooldown
    private float time = 0.0f;
    private Audio sound;
    private string laserBeamSound = "Assets/Audio/SFX/Weapons/Railgun/BarrageShot.wav";
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
                //Engineson.print("Cooldown terminado. Habilidad lista.");
            }
        }
    }
    public override void TriggerAbility()
    {
        if (canThrow)
        {
            //Engineson.print("Lanzando granada...");
            sound.LoadAudio(laserBeamSound);
            sound.Play();

            laserBeam = Engineson.CreateGameObject("LaserBeam", null);

            if (laserBeam == null)
            {
                //Engineson.print("ERROR: No se pudo crear la granada.");
                return;
            }

            laserBeam.AddScript("LaserBeamObject");
            laserBeam.GetComponent<LaserBeamObject>().Init(gameObject.GetComponent<Transform>().GetPosition(), gameObject.GetComponent<Transform>().forward);

            canThrow = false; // Inicia el cooldown
            abilityTimer = 0.0f;
        }
        else
        {
            //Engineson.print("Habilidad en cooldown. Espera...");
        }

        if (!canThrow)
        {
            abilityTimer += time;
            //Engineson.print("Cooldown: " + abilityTimer + " / " + abilityCooldown);

            if (abilityTimer >= abilityCooldown)
            {
                canThrow = true;
                abilityTimer = 0.0f;
                //Engineson.print("Cooldown terminado. Habilidad lista.");
            }
        }

    }

    public override void ResetCooldowns()
    {
        canThrow = true;
    }


}

