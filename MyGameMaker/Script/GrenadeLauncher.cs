using System.Collections.Generic;
using System.Numerics;
using HawkEngine;
using static BaseWeapon;

public class GrenadeLauncher : BaseAbilities
{
    public string name;
    public bool enabled;
    public float cooldown;
    GameObject explosion;
    public GameObject hudGO;
    public HUD hud;
    private float yHeight = 0.0f;
    private float timer = 0;
    private bool exploded = false;
    GameObject grenade;
    Rigidbody rigidbody;
    BoxCollider collider;
    public bool canThrow = true;
    public bool needsDestroy = false;

    private float explosionCooldown = 1.0f;
    private float explosionTimer = 0.0f;

    private float abilityCooldown = 3.0f; // Cooldown de la habilidad
    private float abilityTimer = 0.0f;    // Contador del cooldown

   // private AudioSource sound;
    private string granadeLaunch = "Assets/Audio/SFX/Weapons/Boltgun/BoltgunAbility1GrenadeShot.wav";
    private string granadeExplosion = "Assets/Audio/SFX/Weapons/Boltgun/BoltgunAbility1GrenadeExplosion.wav";
  //  private AudioClip launchFX;
   // private AudioClip explosionFX;

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

        //launchFX = new AudioClip(granadeLaunch, "GrenadeLaunchFX", false, false);
        //explosionFX = new AudioClip(granadeExplosion, "GrenadeExplosionFX", false, false);
        //sound.LoadAudioClip(launchFX);
        //sound.LoadAudioClip(explosionFX);
        hudGO = GameObject.Find("Canvas_HUD");
        hud = hudGO.GetComponent<HUD>();
    }

    public override void Update(float deltaTime)
    {
        // Manejo del cooldown de la habilidad
        if (!canThrow)
        {
            abilityTimer += deltaTime;
          //  Engineson.print("Cooldown: " + abilityTimer + " / " + abilityCooldown);

            if (abilityTimer >= abilityCooldown)
            {
                canThrow = true;
                abilityTimer = 0.0f;
               // Engineson.print("Cooldown terminado. Habilidad lista.");
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
                    Explode();
                }
            }
        }

        // Manejo de la explosión (pero sin destruir la explosión)
        if (exploded)
        {
            explosionTimer += deltaTime;

            if (explosionTimer >= explosionCooldown)
            {
                GetComponent<Collider>().SetPosition(new Vector3(0, -100, 0));
                needsDestroy = false;
                exploded = false;
                explosionTimer = 0f;
            }
        }
    }

    public override void TriggerAbility()
    {
        if (!canThrow)
        {
           // Engineson.print("Habilidad en cooldown. Espera...");
            return;
        }

        Engineson.print("Lanzando granada...");
       // sound?.Play(launchFX);
        grenade = Engineson.CreateGameObject("Grenade", null);

        if (grenade == null)
        {
           // Engineson.print("ERROR: No se pudo crear la granada.");
            return;
        }

        grenade.AddScript("Grenade");
        grenade.GetComponent<Grenade>().Init(gameObject.GetComponent<Transform>().GetPosition(), gameObject.GetComponent<Transform>().forward);
        grenade.GetComponent<Grenade>().Start();

        canThrow = false; // Inicia el cooldown
        abilityTimer = 0.0f;
        hud.TriggerCooldown(abilityCooldown);
    }

    void Explode()
    {
        if (grenade == null) return;

        // Crear explosión (sin destruir nada)
        explosion = Engineson.CreateGameObject("Explosion", null);
        //sound?.Play(explosionFX);
        if (explosion == null) return;

        explosion.AddComponent<MeshRenderer>();
        explosion.GetComponent<Transform>().SetPosition(
            grenade.GetComponent<Transform>().GetPosition().X,
            grenade.GetComponent<Transform>().GetPosition().Y,
            grenade.GetComponent<Transform>().GetPosition().Z
        );
        explosion.GetComponent<Transform>().SetScale(4f, 0.25f, 4f);

        exploded = true;
    }

    public override void ResetCooldowns()
    {
        canThrow = true;
    }
}
