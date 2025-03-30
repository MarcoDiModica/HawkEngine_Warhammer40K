using HawkEngine;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace HawkEngine
{
    internal class Barrage : BaseAbilities
    {
        private Shotgun shotgun;

        public override void Awake()
        {
            shotgun = gameObject.GetComponent<Shotgun>();
            if (shotgun == null)
            {
                Engineson.print("Error: No se encontró Shotgun en Barrage.cs");
            }
        }

        public override void TriggerAbility()
        {
            if (shotgun == null)
            {
                Engineson.print("Barrage: No se encontró Shotgun, no se puede disparar.");
                return;
            }

            Engineson.print("Barrage Activado: Disparando un proyectil especial!");

            int numProjectiles = 7; // Más proyectiles
            float spreadAngle = 60f; // Mayor ángulo de dispersión
            float angleStep = spreadAngle / (numProjectiles - 1);
            float startAngle = -spreadAngle / 2;

            for (int i = 0; i < numProjectiles; i++)
            {
                GameObject projectile = Engineson.CreateGameObject("BarrageProjectile", null);
                projectile.AddComponent<MeshRenderer>();
                projectile.AddComponent<BoxCollider>();

                if (projectile != null)
                {
                    Transform projTransform = projectile.GetComponent<Transform>();
                    if (projTransform != null)
                    {
                        Vector3 forward = shotgun.transform.forward;
                        Vector3 spawnPos = shotgun.transform.position + forward * 1.0f;
                        projTransform.position = spawnPos;

                        float angle = startAngle + angleStep * i;
                        Vector3 direction = Vector3.Transform(forward, Matrix4x4.CreateRotationY(angle * (3.14f / 180f)));
                        projTransform.LookAt(direction);
                        projTransform.SetScale(0.3f, 0.3f, 0.3f); // Proyectiles más grandes

                        projectile.AddScript("BulletData");
                        var bulletData = projectile.GetComponent<BulletData>();

                        if (bulletData != null)
                        {
                            bulletData.Init(projTransform, direction, shotgun.gameObject);
                            Engineson.print($"Barrage: Proyectil {i + 1} disparado correctamente.");
                        }
                        else
                        {
                            Engineson.print("Error: BulletData no encontrado en el proyectil.");
                        }

                        shotgun.bullets.Add(bulletData);
                    }
                }
            }
        }
    }
}