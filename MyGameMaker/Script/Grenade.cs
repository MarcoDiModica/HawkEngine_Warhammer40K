using HawkEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;

public class Grenade : MonoBehaviour
{
    public float value = 0.0f;
    private Transform transform;

    float distance = 6;
    float damage = 100;
    float explosionRadius = 2;
    Rigidbody rigidbody;
    bool isExploded = false;
    GameObject explosion;
    float deathtimer = 0.0f;
    public bool needsDestroy = false;
    float deathTimerPrevention = 2.0f;
    private Audio sound;
    Collider collider;
    public List<string> collisionNames = new List<string>();
    private string granadeExplosion = "Assets/Audio/SFX/Weapons/Boltgun/BoltgunAbility1GrenadeExplosion.wav";

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

    public void Init(Vector3 pos, Vector3 dir)
    {
        AddComponent<MeshRenderer>();      
        GetComponent<Transform>().position = pos + dir * 3.0f + new Vector3(0, 2, 0);
        GetComponent<Transform>().SetScale(0.25f, 0.25f, 0.25f);
        AddComponent<CapsuleCollider>();
        AddComponent<Rigidbody>();
        rigidbody = GetComponent<Rigidbody>();
        rigidbody.SetMass(0.05f);
        rigidbody.SetGravity(new Vector3(0.0f, -9.81f, 0.0f) * 20);
        rigidbody.AddForce(dir * 140);
        rigidbody.SetFriction(0.5f);
    }

    public override void Update(float deltaTime)
    {
        if (isExploded)
        {
            deathtimer += deltaTime;
            if (deathtimer >= deathTimerPrevention)
            {
                
                if (explosion != null && needsDestroy)
                {
                    Engineson.print("Grenade: Destroying grenade");
                    Engineson.Destroy(explosion);
                    needsDestroy = false;
                }
                deathtimer = 0;
            }
        }

    }
    void Explode()
    {
        rigidbody.SetVelocity(new Vector3(0, 0, 0));
        explosion = Engineson.CreateGameObject("Explosion", null);
        sound.LoadAudio(granadeExplosion);
        sound.Play();
        explosion.AddComponent<MeshRenderer>();
        explosion.GetComponent<Transform>().SetPosition(GetComponent<Transform>().GetPosition().X, GetComponent<Transform>().GetPosition().Y, GetComponent<Transform>().GetPosition().Z);
        explosion.GetComponent<Transform>().SetScale(4f, 0.25f, 4f);
        isExploded = true;
        explosion.AddComponent<BoxCollider>();
        needsDestroy = true;

        for (int i = 0; i < collisionNames.Count; i++)
        {
            var enemy = GameObject.Find(collisionNames[i]);
            if (enemy.tag == "Melee")
            {
                enemy.GetComponent<EnemyControllerMelee>().TakeDamage(damage); //placeholder damage
            }
            if (enemy.tag == "Ranged")
            {
                enemy.GetComponent<EnemyControllerRanged>().TakeDamage(damage); //placeholder damage
            }
            if (enemy.tag == "Stalker")
            {
                //enemy.GetComponent<EnemyControllerStalker>().TakeDamage(damage); //placeholder damage
            }
            if (enemy.tag == "Boss")
            {
                enemy.GetComponent<EnemyControllerBoss>().TakeDamage(damage); //placeholder damage
            }
            if (enemy.tag == "Destroyable")
            {
                enemy.GetComponent<DestroyEnviormentObject>().DestroyObject();
            }
        }

    }
    public override void OnCollisionEnter(GameObject other)
    {
        if (isExploded == false)
        {
            Explode();
            Engineson.print("Grenade: Exploded");
        }
                
    }

}
