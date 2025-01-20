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
        actor = Engineson.CreateGameObject("aaa");
        transfr = actor.GetComponent<Transform>();
        

        if (transfr != null)
        {
            Engineson.print("the transform was gotten");
        }
        else
        {
            Engineson.print("Waaaawaaaa");
        }

        timer = 0.0f;

        actor.AddComponent<Camera>();
    }


    public override void Update(float deltaTime)
    {
        if (Input.GetKeyDown(KeyCode.A))
        {
            Engineson.print("pressing a");
            transfr.SetPosition(10, 10, 10);
        }

        timer += deltaTime;

        if (timer > 1.0f)
        {
            Engineson.print("1 second has passed");
            Engineson.print("count: " + count);
            count++;
            timer = 0.0f;

            actor.name = count.ToString();

        }
    }
}

