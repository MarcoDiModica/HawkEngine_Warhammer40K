using System;
using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public class ExplosiveBarrel : MonoBehaviour
{
    public GameObject explosion;
    public Explosion explosionScript;

    public override void Start()
    {
        explosionScript = explosion.GetComponent<Explosion>();
    }

    public void Explode()
    {
        explosionScript.Explode();
        Engineson.Destroy(gameObject);
    }



}