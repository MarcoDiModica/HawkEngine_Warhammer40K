using System.Collections.Generic;
using System.Numerics;
using HawkEngine;
using static BaseWeapon;

public class GrenadeLauncher : BaseAbilities
{

    public string name;
    public bool enabled;
    public float cooldown;

    private float timer = 0;
    private bool exploded = false;
    GameObject grenade;
    Rigidbody rigidbody;

    public override void Start()
    {
       
    }

    public override void Update(float deltaTime)
    {
      
        if (rigidbody != null && exploded == false)
        {
            timer += deltaTime;
            Engineson.print(rigidbody.GetVelocity().Length().ToString());
            if (rigidbody.GetVelocity().Y < 0.1f && timer > 0.1f)
            {
                Explode();
            }
        }
    }

    public override void TriggerAbility()
    {
        grenade = Engineson.CreateGameObject("Grenade", null);
        grenade.AddComponent<MeshRenderer>();
        grenade.AddComponent<Rigidbody>();
        //grenade.GetComponent<Transform>().position = gameObject.GetComponent<Transform>().GetPosition() + gameObject.GetComponent<Transform>().forward * 2.0f+ new Vector3(0,2,0);
        rigidbody = grenade.GetComponent<Rigidbody>();
        grenade.AddComponent<Collider>();
        grenade.GetComponent<Transform>().SetScale(0.25f, 0.25f, 0.25f);
        rigidbody.SetMass(0.05f);
        rigidbody.SetGravity(new Vector3(0.0f,-9.81f,0.0f) * 20);
        rigidbody.AddForce((gameObject.GetComponent<Transform>().forward + new Vector3(0,1,0)) * 500);
        rigidbody.SetFriction(0.5f);
        timer = 0;
        exploded = false;

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
