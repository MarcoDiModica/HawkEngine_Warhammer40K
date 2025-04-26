using HawkEngine;
using System;
using System.Numerics;

public class LocationTitle : MonoBehaviour
{
    private GameObject title;
    private float timer = 0f;

    public void Spawn()
    {
        title.SetActive(true);
        timer = 0f;
    }

    private void CheckForDespawn()
    {
        if (timer > 2f)
        {
            Despawn();
        }
    }

    public void Despawn()
    {
        title.SetActive(false);
    }

    public override void Awake()
    {

    }

    public override void Start()
    {
        title = GameObject.Find("Title");
        Spawn();
    }


    public override void Update(float deltaTime)
    {
        timer += deltaTime;
        CheckForDespawn();

    }
}
