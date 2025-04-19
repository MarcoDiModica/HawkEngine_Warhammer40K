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
    public void Init(GameObject player)
    {
        AddComponent<MeshRenderer>();
        AddComponent<BoxCollider>();
        AddComponent<Rigidbody>();


        GetComponent<Transform>().SetScale(0.25f, 0.25f, 0.25f);

        // Física del proyectil
        rigidbody = GetComponent<Rigidbody>();
        rigidbody.SetMass(0.05f);
        rigidbody.SetGravity(Vector3.Zero); // Sin gravedad
        rigidbody.SetFriction(0.5f);
        
    }
    public override void Update(float deltaTime)
    {
        if (needsDestroy)
        {
            deathTimerPrevention += deltaTime;
            if (deathTimerPrevention >= deathtimer)
            {
                // En vez de destruir, se mueve 100 unidades hacia abajo
                GetComponent<Collider>().SetPosition(new Vector3(0, -100, 0));
                needsDestroy = false; // Para que no siga moviéndose constantemente
            }
        }
    }
}
