using System;
using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public class DestroyEnviormentObject : MonoBehaviour
{

    public void DestroyObject()
    {
        gameObject.GetComponent<Transform>().position = new Vector3(0, -100, 0);
        gameObject.GetComponent<Collider>().SetTrigger(true);
    }

    public override void Awake()
    {
        base.Awake();
    }
    public override void Start()
    {
        base.Start();
    }

    public override void Update(float deltaTime)
    {
        base.Update(deltaTime);
    }
}

