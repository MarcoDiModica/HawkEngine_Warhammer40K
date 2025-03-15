using System.Collections.Generic;
using System.Numerics;
using HawkEngine;
using static BaseWeapon;

public class GrenadeLauncher : BaseAbilities
{

    public string name;
    public bool enabled;
    public float cooldown;


    GameObject grenade;
    Rigidbody rigidbody;

    public override void Start()
    {
       
    }

    public override void Update(float deltaTime)
    {
      
        if (rigidbody != null)
        {
            Engineson.print(rigidbody.GetVelocity().Length().ToString());
            if (rigidbody.GetVelocity().Length() <= 0f)
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
        rigidbody = grenade.GetComponent<Rigidbody>();
        grenade.AddComponent<Collider>();
        grenade.GetComponent<Transform>().SetScale(0.25f, 0.25f, 0.25f);
        rigidbody.SetMass(0.1f);
        rigidbody.SetGravity(new Vector3(0.0f,-9.81f,0.0f) * 40);
        rigidbody.AddForce(new Vector3(2,5,0)*200);
        rigidbody.SetFriction(0.5f);
    }

    void Explode()
    {
        //Visuals of the grenade explosion
        grenade.GetComponent<Transform>().SetScale(4, 0.25f, 4);
        rigidbody.SetMass(1);
    }
}    
