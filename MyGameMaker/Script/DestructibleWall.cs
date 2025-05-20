using System;
using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public class DestructibleWall : MonoBehaviour
{
    public override void OnCollisionEnter(GameObject other)
    {
        if (other.tag == "Explosion")
        {
           Engineson.Destroy(gameObject);
        }
    }
}