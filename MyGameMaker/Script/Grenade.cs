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

    public override void Start()
    {
      
    }

    public void Init(Vector3 pos, Vector3 dir)
    {
        AddComponent<MeshRenderer>();
        AddComponent<Rigidbody>();
        GetComponent<Transform>().position = pos + dir * 3.0f + new Vector3(0, 2, 0);
        rigidbody = GetComponent<Rigidbody>();
        AddComponent<Collider>();
        GetComponent<Transform>().SetScale(0.25f, 0.25f, 0.25f);
        rigidbody.SetMass(0.05f);
        rigidbody.SetGravity(new Vector3(0.0f, -9.81f, 0.0f) * 20);
        rigidbody.AddForce(dir * 1400);
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
                    //Engineson.Destroy(explosion);
            }
        }
        deathTimerPrevention += deltaTime;
        Engineson.print(deathTimerPrevention.ToString());

        if (deathTimerPrevention > .1f) 
        {
            if (explosion != null)
                //Engineson.Destroy(explosion);
            needsDestroy = true;
        }

    }

    void Explode()
    {
        rigidbody.SetVelocity(new Vector3(0, 0, 0));
        explosion = Engineson.CreateGameObject("Explosion", null);
        explosion.AddComponent<MeshRenderer>();
        explosion.GetComponent<Transform>().SetPosition(GetComponent<Transform>().GetPosition().X, GetComponent<Transform>().GetPosition().Y, GetComponent<Transform>().GetPosition().Z);
        explosion.GetComponent<Transform>().SetScale(4f, 0.25f, 4f);
        isExploded = true;
    }

    public override void OnCollisionEnter(Collider other)
    {
        Explode();
    }

}
