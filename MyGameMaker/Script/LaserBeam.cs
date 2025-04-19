using HawkEngine;
using System;
using System.Numerics;

internal class LaserBeam : BaseAbilities
{
    private GameObject laserBlock;
    private float activeTime = 0.0f;
    private float maxDuration = 3.0f; // Duración en segundos
    private bool isActive = false;
    private PlayerController playerController;

    public override void TriggerAbility()
    {
        if (isActive) return;

        // Crear objeto láser y asignar script
        laserBlock = Engineson.CreateGameObject("LaserBeamObject", null);
        laserBlock.AddScript("LaserBeamObject");

        LaserBeamObject laserScript = laserBlock.GetComponent<LaserBeamObject>();
        laserScript.Init(gameObject); // gameObject es el jugador
        //laserScript.SetLifeTime(maxDuration); // duración del láser

        // Poner como hijo del jugador
        Transform blockTransform = laserBlock.GetComponent<Transform>();
        Transform playerTransform = gameObject.GetComponent<Transform>(); // Usa gameObject, no playerController

        //blockTransform.SetParent(playerTransform); // Esto asume que SetParent existe, si no, confirma el método correcto
        blockTransform.localPosition = new Vector3(0, 1.5f, 2.0f);
        //blockTransform.localRotation = Quaternion.Identity;
        blockTransform.SetScale(1.0f, 1.0f, 1.0f);

        // Activar temporizador
        activeTime = 0.0f;
        isActive = true;
    }

    public override void Update(float deltaTime)
    {
        if (!isActive) return;

        activeTime += deltaTime;

        if (activeTime >= maxDuration)
        {
            // Destruir o esconder el bloque
            if (laserBlock != null)
            {
                laserBlock.GetComponent<Collider>().SetPosition(new Vector3(0, -100, 0)); // Lo manda lejos
                laserBlock = null;
            }

            isActive = false;
            activeTime = 0.0f;
        }
    }

    public override void ResetCooldowns()
    {
        // También puede usarse para forzar la desaparición
        if (laserBlock != null)
        {
            laserBlock.GetComponent<Collider>().SetPosition(new Vector3(0, -100, 0));
            laserBlock = null;
        }

        isActive = false;
        activeTime = 0.0f;
    }
}
