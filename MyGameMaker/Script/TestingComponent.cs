using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HawkEngine;

public class TestingComponent : MonoBehaviour
{
    GameObject actor;
    Transform transfr;

    float timer = 0.0f;
    int count = 0;
   
    public override void Start()
    {
        actor = EngineCalls.CreateGameObject("aaa");
        transfr = actor.GetComponent<Transform>();
        

        if (transfr != null)
        {
            EngineCalls.print("the transform was gotten");
        }
        else
        {
            EngineCalls.print("Waaaawaaaa");
        }

        timer = 0.0f;

    }


    public override void Update(float deltaTime)
    {
        if (Input.GetKeyDown(KeyCode.A))
        {
            EngineCalls.print("pressing a");
            transfr.SetPosition(10, 10, 10);
        }

        timer += deltaTime;

        if (timer > 1.0f)
        {
            EngineCalls.print("1 second has passed");
            EngineCalls.print("count: " + count);
            count++;
            timer = 0.0f;
        }
    }
}

