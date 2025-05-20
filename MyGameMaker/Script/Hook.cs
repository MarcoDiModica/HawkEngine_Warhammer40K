using HawkEngine;
using System.Collections;
using System.Collections.Generic;
using System.Numerics;
using static System.Net.Mime.MediaTypeNames;

public class Hook : MonoBehaviour
{
    private Transform transform;
    private Rigidbody rigidbody;
    private GameObject player;
    public List<string> collisionNames = new List<string>();
    private bool needsDestroy = false;
    private string hookTp = "Assets/Audio/SFX/Weapons/Shotgun/HookTp.wav";


    public override void Awake() { }

    public override void Start() 
    { 
        player = GameObject.Find("Player");
        if(player == null)
        {
            Engineson.print("No se ha encontrado el jugador.");
        }       
    }

    public override void Update(float deltaTime) 
    { 
        if (needsDestroy)
        {
            Engineson.Destroy(gameObject);
        }
    }

    public void Init(Vector3 pos, Vector3 dir)
    {
        //AddComponent<MeshRenderer>();
        GetComponent<Transform>().position = pos + dir * 3.0f + new Vector3(0, 2, 0);
        GetComponent<Transform>().SetScale(0.25f, 0.25f, 0.25f);
        AddComponent<BoxCollider>();
        AddComponent<Rigidbody>();

        rigidbody = GetComponent<Rigidbody>();
        rigidbody.SetMass(0.05f);
        rigidbody.SetGravity(new Vector3(0.0f, 0.0f, 0.0f) * 20);
        rigidbody.AddForce(dir * 140);
        rigidbody.SetFriction(0.5f);
    }

    public override void OnCollisionEnter(GameObject other)
    {
        if (other.tag == "Melee" || other.tag == "Ranged" || other.tag == "Stalker" || other.tag == "Boss" || other.tag == "Warrior")
        {
            if (player != null)
            {
                Vector3 hookPosition = other.GetComponent<Transform>().GetPosition() - (gameObject.transform.forward * 5);
                player.GetComponent<Collider>().SetPosition(hookPosition);
                Engineson.print("Jugador teletransportado a la posición del hook.");
                Audio.PlayOneShot(hookTp);
            }
            Engineson.print("Colisión con " + other.name);
        }
        GetComponent<Collider>().SetPosition(new Vector3(0, -100, 0));
        needsDestroy = true;
    }

}
