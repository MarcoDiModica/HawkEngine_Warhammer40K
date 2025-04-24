using HawkEngine;
using System;
using System.Collections.Generic;
using System.Numerics;
using static System.Net.Mime.MediaTypeNames;

public class LaserBeamObject : MonoBehaviour
{
    public float value = 0.0f;
    private Rigidbody rigidbody;
    private float deathtimer = 3.0f; // duración por defecto
    private float deathTimerPrevention = 0.0f;
    float damage = 50;
    private bool needsDestroy = false;
    public List<string> collisionNames = new List<string>();

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
        needsDestroy = true;

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
