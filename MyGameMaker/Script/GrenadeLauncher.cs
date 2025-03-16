using System.Collections.Generic;
using System.Numerics;
using HawkEngine;
using static BaseWeapon;

public class GrenadeLauncher : BaseAbilities
{

    public string name;
    public bool enabled;
    public float cooldown;


    private float yHeight = 0.0f;
    private float timer = 0;
    private bool exploded = false;
    GameObject grenade;
    Rigidbody rigidbody;
    Collider collider;
    public override void Start()
    {
       
    }

    public override void Update(float deltaTime)
    {
      
        if (rigidbody != null && exploded == false && collider != null)
        {
            timer += deltaTime;
            Engineson.print(rigidbody.GetVelocity().Y.ToString());
            
            if (  rigidbody.GetVelocity().Y <= 0.1f && timer > 0.1f && yHeight > grenade.GetComponent<Transform>().GetPosition().Y)
            {
                Explode();
            }
        }
    }

    public override void TriggerAbility()
    {
        grenade = Engineson.CreateGameObject("Grenade", null);
        grenade.AddScript("Grenade");
        grenade.GetComponent<Grenade>().Init(gameObject.GetComponent<Transform>().GetPosition(), gameObject.GetComponent<Transform>().forward);
        //collider = grenade.AddComponent<Collider>();
        //grenade.AddComponent<MeshRenderer>();
        //grenade.AddComponent<Rigidbody>();
        //grenade.GetComponent<Transform>().position = gameObject.GetComponent<Transform>().GetPosition() + gameObject.GetComponent<Transform>().forward * 2.0f+ new Vector3(0,2,0);
        //yHeight = grenade.GetComponent<Transform>().GetPosition().Y;
        //rigidbody = grenade.GetComponent<Rigidbody>();
        //grenade.AddComponent<Collider>();
        //grenade.GetComponent<Transform>().SetScale(0.25f, 0.25f, 0.25f);
        //rigidbody.SetMass(0.05f);
        //rigidbody.SetGravity(new Vector3(0.0f,-9.81f,0.0f) * 20);
        //rigidbody.AddForce(gameObject.GetComponent<Transform>().forward * 1400);
        //rigidbody.SetFriction(0.5f);
        //timer = 0;
        //exploded = false;

    }

    void Explode()
    {
        //Visuals of the grenade explosion
        GameObject explosion = Engineson.CreateGameObject("Explosion", null);
        explosion.AddComponent<MeshRenderer>();
        explosion.GetComponent<Transform>().SetPosition(grenade.GetComponent<Transform>().GetPosition().X, grenade.GetComponent<Transform>().GetPosition().Y, grenade.GetComponent<Transform>().GetPosition().Z);
        explosion.GetComponent<Transform>().SetScale(4f, 0.25f, 4f);
        exploded = true;
    }
}    
