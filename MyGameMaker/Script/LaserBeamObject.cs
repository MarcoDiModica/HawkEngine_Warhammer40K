using HawkEngine;
using System;
using System.Collections.Generic;
using System.Numerics;
using static System.Net.Mime.MediaTypeNames;

public class LaserBeamObject : MonoBehaviour
{
    public float value = 0.0f;
    private float deathtimer = 3.0f; // duración por defecto
    private float deathTimerPrevention = 0.0f;
    float damage = 50;
    public bool needsDestroy = false;
    public List<string> collisionNames = new List<string>();
    private GameObject parent;
    private GameObject laserBeam;

    public override void Awake()
    {

    }
    public void Init(Vector3 pos, Vector3 dir, GameObject parentObject)
    {
        AddComponent<MeshRenderer>();
        AddComponent<BoxCollider>();
        GetComponent<Transform>().position = pos + dir * 3.0f + new Vector3(0, 2, 0);
        GetComponent<Transform>().SetScale(0.5f, 0.5f, 20.0f);
        parent = parentObject;
        needsDestroy = true;
    }
    public override void Update(float deltaTime)
    {
        if (parent != null)
        {
            GetComponent<Transform>().position = parent.GetComponent<Transform>().position + parent.GetComponent<Transform>().forward * 22.0f + new Vector3(0, 3, 0);
            GetComponent<Transform>().rotation = parent.GetComponent<Transform>().rotation;

            if (needsDestroy)
            {
                deathTimerPrevention += deltaTime;
                if (deathTimerPrevention >= deathtimer)
                {
                    Engineson.Destroy(gameObject);
                    needsDestroy = false;
                }
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
