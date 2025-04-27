using HawkEngine;
using System.Collections;
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
    float deathtimer = 0.2f;
    public bool needsDestroy = false;
    float deathTimerPrevention = 0;
    private const string granadeExplosion = "Assets/Audio/SFX/Weapons/Boltgun/BoltgunAbility1GrenadeExplosion.wav";

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

        //grenadeFX = new AudioClip(granadeExplosion, "GrenadeFX", false, false);
        //sound.LoadAudioClip(grenadeFX);

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
            deathtimer -= deltaTime;
            if (deathtimer <= 0)
            {
                if (explosion != null) ;
                GetComponent<Collider>().SetPosition(new Vector3(0, -100, 0));
                needsDestroy = false;
            }
        }
        deathTimerPrevention += deltaTime;
        //Engineson.print(deathTimerPrevention.ToString());

        if (deathTimerPrevention > .1f)
        {
            if (explosion != null) { 
                GetComponent<Collider>().SetPosition(new Vector3(0, -100, 0));
                needsDestroy = false;
            }
        }

    }

    void Explode()
    {
        rigidbody.SetVelocity(new Vector3(0, 0, 0));
        explosion = Engineson.CreateGameObject("Explosion", null);
        Audio.PlayOneShot(granadeExplosion);
        explosion.GetComponent<Transform>().SetPosition(GetComponent<Transform>().GetPosition().X, GetComponent<Transform>().GetPosition().Y, GetComponent<Transform>().GetPosition().Z);
        explosion.GetComponent<Transform>().SetScale(4f, 0.25f, 4f);
        var explosionFX = Engineson.CreateGameObject("ExplosionGranadeFX", null);
        gameObject.AddChild(explosionFX);
        explosionFX.AddComponent<ParticleFX>().ApplyPreset(4);
        explosionFX.GetComponent<ParticleFX>().EmitBurst(40);
        explosionFX.GetComponent<Transform>().SetPosition(
            GetComponent<Transform>().GetPosition().X,
            GetComponent<Transform>().GetPosition().Y,
            GetComponent<Transform>().GetPosition().Z
        );
        isExploded = true;
    }

    public override void OnCollisionEnter(GameObject other)
    {
        Explode();
    }

}
