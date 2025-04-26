using HawkEngine;
using System.Collections;
using System.Collections.Generic;
using System.Numerics;

public class Arc : MonoBehaviour
{
    public float value = 0.0f;
    private Transform transform;
    public List<string> collisionNames = new List<string>();
    float damage = 20;
    Rigidbody rigidbody;
    bool isExploded = false;
    GameObject explosion;
    float deathtimer = 0.2f;
    public bool needsDestroy = false;
    float deathTimerPrevention = 0;

    //private AudioSource sound;
    //private string arcExplosion = "Assets/Audio/SFX/Weapons/Boltgun/ArcSnareExplosion.wav";
    //AudioClip arcFX;

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

        //arcFX = new AudioClip(arcExplosion, "ArcExplosionFX", false, false);

        //sound.LoadAudioClip(arcFX);
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
        rigidbody.SetGravity(new Vector3(0.0f, 0.0f, 0.0f) * 20);
        rigidbody.AddForce(dir * 140);
        rigidbody.SetFriction(0.5f);

    }

    public override void Update(float deltaTime)
    {

        // ESTO PETA EN RELEASE. SE HA DE ARREGLAR
        if (explosion == null)
        {
            return;
        }

        if (isExploded)
        {
            deathtimer -= deltaTime;
            if (deathtimer <= 0)
            {
                if (explosion != null)
                {
                    Engineson.Destroy(explosion);
                    explosion = null; // Asegurarse de que no se acceda a un objeto destruido
                }
                return;
            }
        }

        deathTimerPrevention += deltaTime;

        if (deathTimerPrevention > .1f)
        {
            if (explosion != null)
            {
                Engineson.Destroy(explosion);
                explosion = null; // Asegurarse de que no se acceda a un objeto destruido
                needsDestroy = true;
                return;
            }
        }
    }





    void Explode()
    {
        rigidbody.SetVelocity(new Vector3(0, 0, 0));
        explosion = Engineson.CreateGameObject("Explosion", null);
        explosion.GetComponent<Transform>().SetPosition(GetComponent<Transform>().GetPosition().X, GetComponent<Transform>().GetPosition().Y, GetComponent<Transform>().GetPosition().Z);
        explosion.GetComponent<Transform>().SetScale(4f, 0.25f, 4f);
        var explosionFX = Engineson.CreateGameObject("ExplosionFX", null);
        gameObject.AddChild(explosionFX);
        explosionFX.AddComponent<ParticleFX>().ApplyPreset(17);
        explosionFX.GetComponent<ParticleFX>().EmitBurst(1);
        explosionFX.GetComponent<Transform>().SetPosition(
            GetComponent<Transform>().GetPosition().X,
            GetComponent<Transform>().GetPosition().Y,
            GetComponent<Transform>().GetPosition().Z
        );
        Engineson.print("Explosion");
        
        //sound.Play(arcFX);
        isExploded = true;

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
        Explode();
    }

}
