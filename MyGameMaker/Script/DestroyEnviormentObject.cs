using System;
using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public class DestroyEnviormentObject : MonoBehaviour
{

    public DropManager dropManager;
    public bool onlyMedicaeStimm = false;

    public void DestroyObject()
    {
        //gameObject.GetComponent<Transform>().position = new Vector3(0, -100, 0);
        //gameObject.GetComponent<Collider>().SetTrigger(true);
        if(dropManager != null)
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

        GameObject normalMesh = gameObject.GetChild("wooden box");
        GameObject destroyedMesh = gameObject.GetChild("DestroyedBox");
        //GameObject vfx = gameObject.GetChild("VFX");

        if (normalMesh != null)
        {
            Engineson.Destroy(normalMesh);
        }

        //Engineson.Destroy(gameObject);
        
        if (destroyedMesh == null)
        {
            Engineson.print("DestroyedMesh not found in " + gameObject.name);
            return;
        }
        //vfx.GetComponent<ParticleFX>().EmitBurst(20);
        gameObject.GetComponent<Collider>().SetTrigger(true);
        Vector4 color = destroyedMesh.GetComponent<MeshRenderer>().GetColor();
        destroyedMesh.GetComponent<MeshRenderer>().SetColor(new Vector4(color.X, color.Y, color.Z, 255));

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

