using HawkEngine;
using System;
using System.Numerics;

public class BarrageBullet : MonoBehaviour
{
    public float value = 0.0f;
    private Rigidbody rigidbody;
    private float deathtimer = 0.2f;
    private bool needsDestroy = false;
    private float deathTimerPrevention = 0;
    public override void Awake()
    { 
    }
    public void Init(Vector3 pos, Vector3 dir)
    {
        AddComponent<MeshRenderer>();
        GetComponent<Transform>().position = pos + dir * 3.0f + new Vector3(0, 2, 0);
        Vector3 perpendicularDir = new Vector3(dir.Z, 0, 0);
        GetComponent<Transform>().SetRotationQuat(Quaternion.CreateFromAxisAngle(perpendicularDir, (float)Math.PI / 2));
        GetComponent<Transform>().SetScale(5.0f, 1.0f, 0.5f);
        AddComponent<BoxCollider>();
        AddComponent<Rigidbody>();
        rigidbody = GetComponent<Rigidbody>();
        rigidbody.SetMass(0.05f);
        rigidbody.SetGravity(new Vector3(0.0f, 0.0f, 0.0f) * 20);
        rigidbody.AddForce(dir * 140);
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
