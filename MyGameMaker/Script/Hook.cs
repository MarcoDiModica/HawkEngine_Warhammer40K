using HawkEngine;
using System.Collections;
using System.Numerics;

public class Hook : MonoBehaviour
{
    private Transform transform;
    private Rigidbody rigidbody;
    private GameObject player; // Referencia al jugador

    public override void Awake() { }

    public override void Start() { }

    public override void Update(float deltaTime) { }

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

    public override void OnCollisionEnter(GameObject other)
    {
        if (player != null && other != null)
        {
            Vector3 hookPosition = other.GetComponent<Transform>().GetPosition();
            player.GetComponent<Transform>().SetPosition(hookPosition.X, hookPosition.Y, hookPosition.Z);
            Engineson.print("Jugador teletransportado a la posición del hook.");
        }



    }

}
