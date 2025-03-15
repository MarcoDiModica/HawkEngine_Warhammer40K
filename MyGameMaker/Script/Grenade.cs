﻿using HawkEngine;
using System.Collections;
using System.Numerics;

public class Grenade : MonoBehaviour
{
    public float value = 0.0f;
    private Transform transform;

    float distance = 6;
    float damage = 100;
    float explosionRadius = 2;
    Rigidbody rigidbody;

    public override void Start()
    {
        rigidbody = GetComponent<Rigidbody>();
        rigidbody.AddForce(transform.forward);
    }

    public override void Update(float deltaTime)
    {
        if (rigidbody.GetVelocity().Length() < 0.1f)
        {
            Explode();
        }
    }


    void Explode()
    {
        //Visuals of the grenade explosion
        GameObject explosion = Engineson.CreateGameObject("Explosion", gameObject);

        
    }

}
