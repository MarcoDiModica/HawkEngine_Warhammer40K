using System.Collections.Generic;
using System.Numerics;
using HawkEngine;
using static BaseWeapon;

public class ArcSnare : BaseAbilities
{

    public string name;
    public bool enabled;
    public float cooldown;
    GameObject explosion;

    private float yHeight = 0.0f;
    private float timer = 0;
    private bool exploded = false;
    GameObject arcSnare;
    Rigidbody rigidbody;
    BoxCollider collider;
    bool canThrow = true;

    private float explosionCooldown = 1.0f;
    private float explosionTimer = 0.0f;

    private float abilityCooldown = 3.0f; // Cooldown de la habilidad
    private float abilityTimer = 0.0f;    // Contador del cooldown
    private float time = 0.0f;

    private Audio sound;
    private string arcLaunch = "Assets/Audio/SFX/Weapons/Boltgun/ArcSnareThrow.wav";
    


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
            sound?.LoadAudio(arcLaunch);
            sound?.Play();
            arcSnare = Engineson.CreateGameObject("Arc", null);

            if (arcSnare == null)
            {
                return;
            }

            arcSnare.AddScript("Arc");
            arcSnare.GetComponent<Arc>().Init(gameObject.GetComponent<Transform>().GetPosition(), gameObject.GetComponent<Transform>().forward);
            arcSnare.AddComponent<Audio>();
            arcSnare.GetComponent<Arc>().Start();


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

    void Explode()
    {
        if (arcSnare == null) return;

        // Crear explosión
        explosion = Engineson.CreateGameObject("Explosion", null);
        Engineson.Destroy(arcSnare);

        if (explosion == null) return;

        explosion.AddComponent<MeshRenderer>();
        explosion.GetComponent<Transform>().SetPosition(
            arcSnare.GetComponent<Transform>().GetPosition().X,
            arcSnare.GetComponent<Transform>().GetPosition().Y,
            arcSnare.GetComponent<Transform>().GetPosition().Z
        );
        explosion.GetComponent<Transform>().SetScale(4f, 0.25f, 4f);

        

        exploded = true;
    }

    public override void ResetCooldowns()
    {
        canThrow = true;
    }
}

