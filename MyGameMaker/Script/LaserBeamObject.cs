using HawkEngine;
using System;
using System.Numerics;

public class LaserBeamObject : MonoBehaviour
{
    public float value = 0.0f;
    private Rigidbody rigidbody;
    private float deathtimer = 3.0f; // duración por defecto
    private float deathTimerPrevention = 0.0f;
    private bool needsDestroy = false;

    public override void Awake()
    {

    }
    public void Init(Vector3 pos, Vector3 dir)
    {
        AddComponent<MeshRenderer>();
        GetComponent<Transform>().position = pos + dir * 3.0f + new Vector3(0, 2, 0);
        GetComponent<Transform>().SetScale(0.5f, 0.5f, 5.0f);
        AddComponent<BoxCollider>();
        AddComponent<Rigidbody>();
        rigidbody = GetComponent<Rigidbody>();
        //rigidbody.SetMass(0.05f);
        //rigidbody.SetGravity(new Vector3(0.0f, -9.81f, 0.0f) * 20);
        //rigidbody.AddForce(dir * 140);
        //rigidbody.SetFriction(0.5f);

    }
    public override void Update(float deltaTime)
    {
        if (needsDestroy)
        {
            deathTimerPrevention += deltaTime;
            if (deathTimerPrevention >= deathtimer)
            {
                // En vez de destruir, se mueve 100 unidades hacia abajo
                Engineson.Destroy(gameObject); 
                needsDestroy = false; // Para que no siga moviéndose constantemente
            }
        }
    }
}
