using HawkEngine;
using System.Collections;
using System.Collections.Generic;
using System.Numerics;
using static System.Net.Mime.MediaTypeNames;

public class Ball : MonoBehaviour
{
    public float value = 0.0f;
    private Rigidbody rigidbody;
    private float deathtimer = 0.2f;
    private bool needsDestroy = false;
    private float deathTimerPrevention = 0;
    public List<string> collisionNames = new List<string>();
    private float damage = 100.0f; // Placeholder damage value
    public override void Awake(){ }

    public override void Start() 
    {
        
    }
    public void Init(Vector3 pos, Vector3 dir)
    {
        //AddComponent<MeshRenderer>();
        GetComponent<Transform>().position = pos + dir * 3.0f + new Vector3(0, 2, 0);
        GetComponent<Transform>().SetScale(2.0f, 2.0f, 2.0f);
        AddComponent<BoxCollider>();
        AddComponent<Rigidbody>();
        rigidbody = GetComponent<Rigidbody>();
        rigidbody.SetMass(0.05f);
        rigidbody.SetGravity(new Vector3(0.0f, 0.0f, 0.0f) * 20);
        rigidbody.AddForce(dir * 20);
        rigidbody.SetFriction(0.5f);
        
    }
    public override void Update(float deltaTime)
    {
        if (needsDestroy)
        {
            deathTimerPrevention += deltaTime;
            if (deathTimerPrevention >= deathtimer)
            {
                GetComponent<Transform>().position -= new Vector3(0, 100, 0);
                needsDestroy = false;
            }
        }
        

    }

    public override void OnCollisionEnter(GameObject other)
    {
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

}
