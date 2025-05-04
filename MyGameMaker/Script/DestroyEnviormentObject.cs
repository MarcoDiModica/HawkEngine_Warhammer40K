using System;
using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public class DestroyEnviormentObject : MonoBehaviour
{

    public DropManager dropManager;

    public void DestroyObject()
    {
        //gameObject.GetComponent<Transform>().position = new Vector3(0, -100, 0);
        //gameObject.GetComponent<Collider>().SetTrigger(true);
        if(dropManager != null)
        {
            dropManager.SpawnShotgunShells(gameObject.transform.position);
        }
        else
        {
            Engineson.print("DropManager not found");
        }
        Engineson.Destroy(gameObject);
    }

    public override void Awake()
    {
        //base.Awake();
    }
    public override void Start()
    {
        //base.Start();
        dropManager = GameObject.Find("DropManager").GetComponent<DropManager>();
        if (dropManager == null)
        {
           Engineson.print("DropManager not found");
        }
    }

    public override void Update(float deltaTime)
    {
        //base.Update(deltaTime);
    }
}

