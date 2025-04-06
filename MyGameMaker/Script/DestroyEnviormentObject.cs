using System;
using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public class DestroyEnviormentObject : MonoBehaviour { 

    public void DestroyObject()
    {
      gameObject.SetActive(false);
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

