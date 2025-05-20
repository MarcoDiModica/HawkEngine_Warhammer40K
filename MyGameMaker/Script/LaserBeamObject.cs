using HawkEngine;
using System;
using System.Collections.Generic;
using System.Numerics;
using static System.Net.Mime.MediaTypeNames;

public class LaserBeamObject : MonoBehaviour
{
    float damage = 50;


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
            if (other.tag == "Melee")
            {
                other.GetComponent<EnemyControllerMelee>().TakeDamage(damage); 
            }
            if (other.tag == "Ranged")
            {
                other.GetComponent<EnemyControllerRanged>().TakeDamage(damage);
            }
            if (other.tag == "Stalker")
            {
                other.GetComponent<EnemyControllerStalker>().TakeDamage(damage);
            }
            if (other.tag == "Warrior")
            {
                other.GetComponent<EnemyControllerWarrior>().TakeDamage(damage);
            }
            if (other.tag == "Boss")
            {
                other.GetComponent<EnemyControllerBoss>().TakeDamage(damage);
            }
            if (other.tag == "Destroyable")
            {
                other.GetComponent<DestroyEnviormentObject>().DestroyObject();
            }
    }



}
