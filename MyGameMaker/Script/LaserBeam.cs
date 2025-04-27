using System.Collections.Generic;
using System.Numerics;
using HawkEngine;
using static System.Net.Mime.MediaTypeNames;
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
    private string laserBeamSound = "Assets/Audio/SFX/Weapons/Railgun/BarrageShot.wav";
    private bool laserActive = false;
    private float deathtimer = 3.0f;
    private float deathTimerPrevention = 0.0f;
    public List<string> collisionNames = new List<string>();

    //stats
    private float damage = 20.0f;
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
            //Engineson.print("Cooldown: " + abilityTimer + " / " + abilityCooldown);

            if (abilityTimer >= abilityCooldown)
            {
                canThrow = true;
                abilityTimer = 0.0f;
                //Engineson.print("Cooldown terminado. Habilidad lista.");
            }
        }

        if (laserActive)
        {
            deathTimerPrevention += deltaTime;
            if (deathTimerPrevention >= deathtimer)
            {
                if(laserBeam != null)
                {
                    laserActive = false;
                    Engineson.Destroy(laserBeam);
                    Audio.Stop(laserBeamSound);
                    deathTimerPrevention = 0.0f;
                }
            }
        }

    }
    public override void TriggerAbility()
    {
        if (canThrow)
        {
            
            laserBeam = Engineson.CreateGameObject("LaserBeam", null);
            laserBeam.tag = "LaserBeam";


            if (laserBeam == null)
            {
                //Engineson.print("ERROR: No se pudo crear la granada.");
                return;
            }

            laserBeam.AddScript("LaserBeamObject");
            gameObject.AddChild(laserBeam);
            laserBeam.AddComponent<MeshRenderer>();
            laserBeam.AddComponent<BoxCollider>();
            laserBeam.GetComponent<Transform>().position = gameObject.transform.GetPosition() + gameObject.transform.forward * 22.0f + new Vector3(0, 3, 0);
            laserBeam.GetComponent<Transform>().SetScale(0.5f, 0.5f, 20.0f);
            laserActive = true;
            Audio.Play(laserBeamSound, true);
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

