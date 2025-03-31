using System;
using System.Collections.Generic;
using System.Numerics;
using HawkEngine;
using static BaseWeapon;


public class Barrage : BaseAbilities
{
    public GameObject shooter; // Ahora almacenamos el objeto completo en vez del transform
    public List<BulletData> bullets = new List<BulletData>();

    private bool isAbilityActive = false;
    public override void Awake() { }
    public override void Start() { }
    public override void Update(float deltaTime) { }
    public override void TriggerAbility()
    {
        if (isAbilityActive) return;
        if (shooter == null) return; // Cambiado de shooterTransform a shooter

        isAbilityActive = true;

        int numProjectiles = 10;
        float spreadAngle = 90.0f;
        float projectileSize = 0.2f;
        float rangeMultiplier = 1.5f;
        float angleStep = spreadAngle / (numProjectiles - 1);
        float startAngle = -spreadAngle / 2;

        Transform shooterTransform = shooter.GetComponent<Transform>(); // Obtenemos el transform
        if (shooterTransform == null) return;

        for (int i = 0; i < numProjectiles; i++)
        {
            GameObject projectile = Engineson.CreateGameObject("Projectile", null);
            if (projectile == null) continue;

            projectile.AddComponent<MeshRenderer>();
            projectile.AddComponent<BoxCollider>();

            Transform projTransform = projectile.GetComponent<Transform>();
            if (projTransform == null) continue;

            Vector3 forward = shooterTransform.forward;
            Vector3 spawnPos = shooterTransform.position + forward * rangeMultiplier;
            projTransform.position = spawnPos;

            float angle = startAngle + angleStep * i;
            double radians = angle * (Math.PI / 180.0);
            Vector3 direction = forward + new Vector3((float)Math.Sin(radians), 0, (float)Math.Cos(radians));

            projTransform.position += direction;
            projTransform.SetScale(projectileSize, projectileSize, projectileSize);

            projectile.AddScript("BulletData");
            BulletData bullet = projectile.GetComponent<BulletData>();

            if (bullet != null)
            {
                bullet.Init(projTransform, direction, shooter); // Ahora pasamos shooter en vez de shooterTransform
                bullets.Add(bullet);
            }

            Engineson.print("Barrage Projectile fired!");
        }

        isAbilityActive = false;
    }
}
