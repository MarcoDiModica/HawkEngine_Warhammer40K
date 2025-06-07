using System;
using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public class DestructibleWall : MonoBehaviour
{
    public void DestroyWall()
    {
        Engineson.print("Destroy Wall");
        Engineson.Destroy(gameObject);
    }
}