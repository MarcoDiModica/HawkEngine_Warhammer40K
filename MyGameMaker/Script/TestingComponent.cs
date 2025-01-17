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

    }


    public override void Update(float deltaTime)
    {



    }
}

