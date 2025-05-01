using System;
using System.Collections;
using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public class DropManager : MonoBehaviour
{
    public Prefab MagnetPrefab;
    public Prefab BlackHeartPrefab;
    public Prefab AmmunitionBlessingPrefab;
    public Prefab ChapterStandardPrefab;
    public Prefab ShotgunShellsPrefab;
    public Prefab BoltgunBulletsPrefab;
    public Prefab MedicaeStimmPrefab;
    public Prefab PiercingBulletsPrefab;
    public override void Awake()
    {

    }

    public override void Start()
    {

    }

    public override void Update(float deltaTime)
    {
        
    }

    public void SpawnPrefab(EnemyController enemy)
    {
        Engineson.print(enemy.gameObject.tag);
        if (enemy.gameObject.tag == "Stalker")
        {
            Random randomGeneral = new Random();
            int randomValueGeneral = randomGeneral.Next(0, 100);
            if(randomValueGeneral <= 70)
            {
                Random random = new Random();
                int randomValue = random.Next(0, 100);
                if (randomValue < 25) // 25 chance
                {
                    var ammunitionBlessing = Instantiate(AmmunitionBlessingPrefab);
                    ammunitionBlessing.transform.position = new Vector3 (enemy.gameObject.transform.position.X + 3, enemy.gameObject.transform.position.Y, enemy.gameObject.transform.position.Z);
                }
                else if (randomValue < 75) // 50 chance
                {
                    var blackHeart = Instantiate(BlackHeartPrefab);
                    blackHeart.transform.position = new Vector3(enemy.gameObject.transform.position.X - 3, enemy.gameObject.transform.position.Y, enemy.gameObject.transform.position.Z);
                }
                else // 25 chance
                {
                    var chapterStandard = Instantiate(ChapterStandardPrefab);
                    chapterStandard.transform.position = new Vector3(enemy.gameObject.transform.position.X , enemy.gameObject.transform.position.Y, enemy.gameObject.transform.position.Z + 3);
                }
            }

            for (int i = 0; i < 3; i++)
            {
                var shotgunShells = Instantiate(ShotgunShellsPrefab);
                shotgunShells.transform.position = new Vector3(enemy.gameObject.transform.position.X, enemy.gameObject.transform.position.Y, enemy.gameObject.transform.position.Z - i + 3);
                var boltgunBullets = Instantiate(BoltgunBulletsPrefab);
                boltgunBullets.transform.position = new Vector3(enemy.gameObject.transform.position.X, enemy.gameObject.transform.position.Y, enemy.gameObject.transform.position.Z - i - 3);
            }
        }

        if(enemy.gameObject.tag == "Melee")
        {
            Random randomGeneral = new Random();
            int randomValueGeneral = randomGeneral.Next(0, 100);
            if (randomValueGeneral <= 10)
            {
                Random random = new Random();
                int randomValue = random.Next(0, 100);
                if (randomValue < 25) // 25 chance
                {
                    var ammunitionBlessing = Instantiate(AmmunitionBlessingPrefab);
                    ammunitionBlessing.transform.position = new Vector3(enemy.gameObject.transform.position.X + 3, enemy.gameObject.transform.position.Y, enemy.gameObject.transform.position.Z);
                }
                else if (randomValue < 50) // 25 chance
                {
                    var medicaeStimm = Instantiate(MedicaeStimmPrefab);
                    medicaeStimm.transform.position = new Vector3(enemy.gameObject.transform.position.X - 3, enemy.gameObject.transform.position.Y, enemy.gameObject.transform.position.Z);
                }
                else if (randomValue < 75) // 25 chance
                {
                    var magnet = Instantiate(MagnetPrefab);
                    magnet.transform.position = new Vector3(enemy.gameObject.transform.position.X, enemy.gameObject.transform.position.Y, enemy.gameObject.transform.position.Z + 3);
                }
                else // 25 chance
                {
                    var piercingBullets = Instantiate(PiercingBulletsPrefab);
                    piercingBullets.transform.position = new Vector3(enemy.gameObject.transform.position.X, enemy.gameObject.transform.position.Y, enemy.gameObject.transform.position.Z - 3);
                }
            }

            Random Bullets = new Random();
            int randomValueBullets = Bullets.Next(0, 100);
            if (randomValueBullets < 50) // 50 chance
            {
                    var shotgunShells = Instantiate(ShotgunShellsPrefab);
                    shotgunShells.transform.position = new Vector3(enemy.gameObject.transform.position.X + 3, enemy.gameObject.transform.position.Y, enemy.gameObject.transform.position.Z  + 3);
            }
            else // 50 chance
            {
                    var boltgunBullets = Instantiate(BoltgunBulletsPrefab);
                    boltgunBullets.transform.position = new Vector3(enemy.gameObject.transform.position.X - 3, enemy.gameObject.transform.position.Y, enemy.gameObject.transform.position.Z - 3);
            }
        }

        if (enemy.gameObject.tag == "Warrior")
        {
            Random randomGeneral = new Random();
            int randomValueGeneral = randomGeneral.Next(0, 100);
            if (randomValueGeneral <= 20)
            {
                Random random = new Random();
                int randomValue = random.Next(0, 100);
                if (randomValue < 30) // 30 chance
                {
                    var blackHeart = Instantiate(BlackHeartPrefab);
                    blackHeart.transform.position = new Vector3(enemy.gameObject.transform.position.X + 3, enemy.gameObject.transform.position.Y, enemy.gameObject.transform.position.Z);
                }
                else if (randomValue < 60) // 30 chance
                {
                    var chapterStandard = Instantiate(ChapterStandardPrefab);
                    chapterStandard.transform.position = new Vector3(enemy.gameObject.transform.position.X - 3, enemy.gameObject.transform.position.Y, enemy.gameObject.transform.position.Z);
                }
                else if (randomValue < 80) // 20 chance
                {
                    var ammunitionBlessing = Instantiate(AmmunitionBlessingPrefab);
                    ammunitionBlessing.transform.position = new Vector3(enemy.gameObject.transform.position.X, enemy.gameObject.transform.position.Y, enemy.gameObject.transform.position.Z + 3);
                }
                else // 20 chance
                {
                    var piercingBullets = Instantiate(PiercingBulletsPrefab);
                    piercingBullets.transform.position = new Vector3(enemy.gameObject.transform.position.X, enemy.gameObject.transform.position.Y, enemy.gameObject.transform.position.Z - 3);
                }
            }
            Random Bullets = new Random();
            int randomValueBullets = Bullets.Next(0, 100);
            if (randomValueBullets < 25) // 50 chance
            {
                for (int i = 0; i < 2; i++)
                {
                    var shotgunShells = Instantiate(ShotgunShellsPrefab);
                    shotgunShells.transform.position = new Vector3(enemy.gameObject.transform.position.X, enemy.gameObject.transform.position.Y, enemy.gameObject.transform.position.Z - i + 3);
                }
            }
            else if(randomValueBullets < 75)// 25 chance
            {
                for (int i = 0; i < 2; i++)
                {
                    var boltgunBullets = Instantiate(BoltgunBulletsPrefab);
                    boltgunBullets.transform.position = new Vector3(enemy.gameObject.transform.position.X - 3, enemy.gameObject.transform.position.Y, enemy.gameObject.transform.position.Z - 3);
                }
            }
            else // 50 chance
            {
                var shotgunShells = Instantiate(ShotgunShellsPrefab);
                shotgunShells.transform.position = new Vector3(enemy.gameObject.transform.position.X, enemy.gameObject.transform.position.Y, enemy.gameObject.transform.position.Z + 3);
                var boltgunBullets = Instantiate(BoltgunBulletsPrefab);
                boltgunBullets.transform.position = new Vector3(enemy.gameObject.transform.position.X - 3, enemy.gameObject.transform.position.Y, enemy.gameObject.transform.position.Z - 3);
            }
        }
    }
}
          