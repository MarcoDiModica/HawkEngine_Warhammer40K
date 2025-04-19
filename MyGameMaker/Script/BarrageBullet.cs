using HawkEngine;
using System;
using System.Collections.Generic;
using System.Numerics;
using static System.Net.Mime.MediaTypeNames;

public class BarrageBullet : MonoBehaviour
{
    public float value = 0.0f;
    private Rigidbody rigidbody;
    private float deathtimer = 0.2f;
    private bool needsDestroy = false;
    private float deathTimerPrevention = 0;
    public List<string> collisionNames = new List<string>();
    private float damage = 100.0f; // Placeholder damage value
    public override void Awake()
    {

    }
    public void Init(GameObject player)
    {
        AddComponent<MeshRenderer>();
        AddComponent<BoxCollider>();
        AddComponent<Rigidbody>();

        Transform playerTransform = player.GetComponent<Transform>();
        Transform transform = GetComponent<Transform>();

        // Posicionar el proyectil justo frente y un poco arriba del jugador
        Vector3 forward = playerTransform.forward;
        Vector3 playerPosition = playerTransform.position;

        transform.position = playerTransform.position + playerTransform.up * 2.0f;

        Vector3 barragePosition = playerPosition + forward * 2.0f;

        transform.position = barragePosition;
        transform.SetScale(5.0f, 1.0f, 0.5f);

        // Asignar rotación del proyectil según la dirección del jugador
        float angle = (float)Math.Atan2(forward.X, forward.Z);
        Quaternion rotation = Quaternion.CreateFromAxisAngle(Vector3.UnitY, angle);
        transform.SetRotationQuat(rotation);

        // Física del proyectil
        rigidbody = GetComponent<Rigidbody>();
        rigidbody.SetMass(0.05f);
        rigidbody.SetGravity(Vector3.Zero); // Sin gravedad
        rigidbody.SetFriction(0.5f);
        rigidbody.AddForce(forward * 140.0f);
    }
    public override void Update(float deltaTime)
    {
        if (needsDestroy)
        {
            deathTimerPrevention += deltaTime;
            if (deathTimerPrevention >= deathtimer)
            {
                // En vez de destruir, se mueve 100 unidades hacia abajo
                GetComponent<Collider>().SetPosition(new Vector3(0, -100, 0));
                needsDestroy = false; // Para que no siga moviéndose constantemente
            }
        }
    }
    public override void OnCollisionEnter(GameObject other)
    {
        for (int i = 0; i < collisionNames.Count; i++)
        {
            var enemy = GameObject.Find(collisionNames[i]);
            if (enemy.tag == "Melee")
            {
                enemy.GetComponent<EnemyControllerMelee>().TakeDamage(damage); //placeholder damage
            }
            if (enemy.tag == "Ranged")
            {
                enemy.GetComponent<EnemyControllerRanged>().TakeDamage(damage); //placeholder damage
            }
            if (enemy.tag == "Stalker")
            {
                //enemy.GetComponent<EnemyControllerStalker>().TakeDamage(damage); //placeholder damage
            }
            if (enemy.tag == "Boss")
            {
                enemy.GetComponent<EnemyControllerBoss>().TakeDamage(damage); //placeholder damage
            }
            if (enemy.tag == "Destroyable")
            {
                enemy.GetComponent<DestroyEnviormentObject>().DestroyObject();
            }
        }
    }

}
