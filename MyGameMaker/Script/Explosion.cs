using System;
using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public class Explosion : MonoBehaviour
{
    public GameObject player;
    public PlayerController playerController;
    public float explosionRadius = 5.0f;
    public CapsuleCollider explosionArea;
    public bool justExploded = false;
    public bool flamesActive = false;

    private float explosionTimer = 0.0f;
    private float flamesTimer = 0.0f;
    private float flamesDuration = 3.0f;
    private bool flameDamageActive = false;

    public override void Start()
    {
        explosionArea = GetComponent<CapsuleCollider>();
        explosionArea.SetActive(false);
        player = GameObject.Find("Player");
        playerController = player.GetComponent<PlayerController>();
    }

    public override void Update(float deltaTime)
    {
        if (justExploded)
        {
            explosionTimer += deltaTime;
            if (explosionTimer >= 0.5f)
            {
                justExploded = false;
                flamesActive = true;
                explosionTimer = 0.0f;
            }
        }

        if (flamesActive)
        {
            flamesTimer += deltaTime;
            if (Math.Floor(flamesTimer) > Math.Floor(flamesTimer - deltaTime))
            {
                flameDamageActive = true;
            }
            else
            {
                flameDamageActive = false;
            }


            if (flamesTimer >= flamesDuration)
            {
                flamesActive = false;
                flamesTimer = 0.0f;
                explosionArea.SetActive(false);
                Engineson.Destroy(gameObject);
            }
        }
    }

    public void Explode()
    {
        explosionArea.SetActive(true);
        justExploded = true;
    }

    public override void OnTriggerEnter(GameObject other)
    {
        if (justExploded)
        {
           
            if (other.tag == "Player")
            {
                playerController.playerData.TakeDamage(60);
            }
            if (other.tag == "Melee")
            {
                other.GetComponent<EnemyControllerMelee>().TakeDamage(60);
            }
            if (other.tag == "Ranged")
            {
                other.GetComponent<EnemyControllerRanged>().TakeDamage(60);
            }
            if (other.tag == "Stalker")
            {
                other.GetComponent<EnemyControllerStalker>().TakeDamage(60);
            }
            if (other.tag == "Warrior")
            {
                other.GetComponent<EnemyControllerBoss>().TakeDamage(60);
            }
        }

    }

    public override void OnTriggerStay(GameObject other)
    {
        if (flameDamageActive)
        {
            if (other.tag == "Player")
            {
                playerController.playerData.TakeDamage(5);
            }
            if (other.tag == "Melee")
            {
                other.GetComponent<EnemyControllerMelee>().TakeDamage(5);
            }
            if (other.tag == "Ranged")
            {
                other.GetComponent<EnemyControllerRanged>().TakeDamage(5);
            }
            if (other.tag == "Stalker")
            {
                other.GetComponent<EnemyControllerStalker>().TakeDamage(5);
            }
            if (other.tag == "Warrior")
            {
                other.GetComponent<EnemyControllerBoss>().TakeDamage(5);
            }
        }
    }
}