using System;
using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public class DestroyEnviormentObject : MonoBehaviour
{

    public DropManager dropManager;
    public bool onlyMedicaeStimm = false;

    public GameObject boxAlive;
    public GameObject boxDestroyed;
    public bool isDestroyed = false;

    public void DestroyObject()
    {
        Engineson.print("Destroying object: " + gameObject.name);
        if (!isDestroyed)
        {
            if (dropManager != null)
            {
                if (onlyMedicaeStimm)
                {
                    dropManager.SpawnMedicaeStimm(gameObject.transform.position);
                }
                else
                {
                    dropManager.SpawnPrefabFromDestroyableObject(gameObject.transform.position);
                }

            }
            else
            {
                Engineson.print("DropManager not found");
            }
            if (boxAlive != null && boxDestroyed != null)
            {
                Engineson.print("Destroying object, switching box states.");
                boxAlive.SetActive(false);
                boxDestroyed.SetActive(true);
            }
            isDestroyed = true;
        }
        //gameObject.GetComponent<Transform>().position = new Vector3(0, -100, 0);
        //gameObject.GetComponent<Collider>().SetTrigger(true);
        
        
    }

    public override void Awake()
    {
        //base.Awake();
    }
    public override void Start()
    {
        //base.Start();
        dropManager = GameObject.Find("DropManager").GetComponent<DropManager>();
        if (boxAlive != null && boxDestroyed != null)
        {
            Engineson.print("Box Alive and Box Destroyed found, setting initial state.");
            boxAlive.SetActive(true);
            boxDestroyed.SetActive(false);
        }
      
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

