using HawkEngine;
using System;
using System.Numerics;

public class Interaction : MonoBehaviour
{
    private GameObject Box;
    private GameObject Text;
    private float timer = 0f;

    public void Spawn()
    {
        Box.SetActive(true);
        Text.SetActive(true);
        timer = 0f;
    }

    private void CheckForDespawn()
    {
        if (timer > 0.5f)
        {
            Despawn();
        }
    }

    public void Despawn()
    {
        Box.SetActive(false);
        Text.SetActive(false);
    }

    public override void Awake()
    {

    }

    public override void Start()
    {
        Box = GameObject.Find("Box");
        Text = GameObject.Find("Text");
    }


    public override void Update(float deltaTime)
    {
        timer += deltaTime;
        if (Input.GetKeyDown(KeyCode.RETURN) || Input.GetControllerButtonDown(ControllerButton.A))
        {
            CheckForDespawn();
        }
        if (Input.GetKeyDown(KeyCode.X))
        {
            Spawn();
        }

    }
}
