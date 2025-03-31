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

    private float yHeight = 0.0f;
    private float timer = 0;
    private bool exploded = false;
    GameObject grenade;
    Rigidbody rigidbody;
    BoxCollider collider;
    bool canThrow = true;

    private float explosionCooldown = 1.0f;
    private float explosionTimer = 0.0f;

    private float abilityCooldown = 3.0f; // Cooldown de la habilidad
    private float abilityTimer = 0.0f;    // Contador del cooldown

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
                exploded = false;
                explosionTimer = 0f;
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
        grenade = Engineson.CreateGameObject("Grenade", null);

        if (grenade == null)
        {
            Engineson.print("ERROR: No se pudo crear la granada.");
            return;
        }

        grenade.AddScript("Grenade");
        grenade.GetComponent<Grenade>().Init(gameObject.GetComponent<Transform>().GetPosition(), gameObject.GetComponent<Transform>().forward);

        canThrow = false; // Inicia el cooldown
        abilityTimer = 0.0f;
    }

    void Explode()
    {
        if (grenade == null) return;

        // Crear explosión (sin destruir nada)
        explosion = Engineson.CreateGameObject("Explosion", null);
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
}
