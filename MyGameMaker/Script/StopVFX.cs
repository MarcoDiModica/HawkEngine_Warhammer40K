using HawkEngine;
using System;
using System.Collections.Generic;
using System.Numerics;

public class StopVFX : MonoBehaviour
{
    public override void Start()
    {
        GetComponent<ParticleFX>().Stop();
    }

    public override void Update(float deltaTime)
    {
        
    }

}