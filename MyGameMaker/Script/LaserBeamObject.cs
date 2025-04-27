using HawkEngine;
using System;
using System.Collections.Generic;
using System.Numerics;
using static System.Net.Mime.MediaTypeNames;

public class LaserBeamObject : MonoBehaviour
{
    float damage = 50;
    public List<string> collisionNames = new List<string>();


    public override void Awake()
    {

    }
    public void Init(Vector3 pos, Vector3 dir)
    {
        
    }
    public override void Update(float deltaTime)
    {
       
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
