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

    public override void Awake()
    {

    }
    public override void Start()
    {
        
    }

    public override void Update(float deltaTime)
    {
        if (rigidbody != null && !exploded && collider != null)
        {
            timer += deltaTime;
            //Engineson.print("Velocidad Y: " + rigidbody.GetVelocity().Y);
            //Engineson.print("Timer: " + timer);

            if (rigidbody.GetVelocity() != null && grenade != null && grenade.GetComponent<Transform>() != null)
            {
                float grenadeY = grenade.GetComponent<Transform>().GetPosition().Y;

                //Engineson.print("Chequeando condiciones para Explode()...");
                //Engineson.print("Velocidad Y: " + rigidbody.GetVelocity().Y);
                //Engineson.print("yHeight: " + yHeight);
                //Engineson.print("grenadeY: " + grenadeY);

                if (rigidbody.GetVelocity().Y <= 0.1f && timer > 0.1f && yHeight > grenadeY)
                {
                    //Engineson.print("Llamando a Explode()...");
                    Explode();
                }
            }
        }

        // Manejo de la explosión
        if (exploded)
        {
            explosionTimer += deltaTime;
            //Engineson.print("Explosion Timer: " + explosionTimer + " / " + explosionCooldown);

            if (explosionTimer >= explosionCooldown)
            {
                if (explosion != null)
                {
                    //Engineson.print("Destruyendo explosión...");
                    Engineson.Destroy(explosion);
                    explosion = null;
                    //Engineson.print("Explosión destruida.");
                }
                else
                {
                    //Engineson.print("ERROR: explosion ya era NULL antes de destruir.");
                }

                exploded = false;
                explosionTimer = 0f;
            }
        }

        // Manejo de la granada (después de la explosión)
        if (!exploded && grenade != null && grenade.GetComponent<Grenade>().needsDestroy)
        {
            //Engineson.print("Destruyendo granada...");
            Engineson.Destroy(grenade.GetComponent<Grenade>().gameObject);
            grenade = null;
            canThrow = true;
        }
    }



    public override void TriggerAbility()
    {
        if (canThrow)
        {
            grenade = Engineson.CreateGameObject("Grenade", null);
            grenade.AddScript("Grenade");
            grenade.GetComponent<Grenade>().Init(gameObject.GetComponent<Transform>().GetPosition(), gameObject.GetComponent<Transform>().forward);
            canThrow = false;
        }
    }

    void Explode()
    {
       // Engineson.print("Ejecutando Explode()...");

        if (grenade == null)
        {
            //Engineson.print("ERROR: Grenade es NULL en Explode().");
            return;
        }

        // Crear explosión
        explosion = Engineson.CreateGameObject("Explosion", null);

        if (explosion == null)
        {
            //Engineson.print("ERROR: No se pudo crear la explosión.");
            return;
        }

        explosion.AddComponent<MeshRenderer>();
        explosion.GetComponent<Transform>().SetPosition(
            grenade.GetComponent<Transform>().GetPosition().X,
            grenade.GetComponent<Transform>().GetPosition().Y,
            grenade.GetComponent<Transform>().GetPosition().Z
        );
        explosion.GetComponent<Transform>().SetScale(4f, 0.25f, 4f);

        exploded = true;
        

        //Engineson.print("Explosión creada. exploded = " + exploded);
    }

}
