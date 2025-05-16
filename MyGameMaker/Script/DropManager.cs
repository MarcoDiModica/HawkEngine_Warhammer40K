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
    private PlayerController playerController;
    public override void Awake()
    {

    }

    public override void Start()
    {
        playerController = GameObject.Find("Player").GetComponent<PlayerController>();
    }

    public override void Update(float deltaTime)
    {
        
    }

    public void SpawnPrefab(EnemyController enemy)
    {
        //Engineson.print(enemy.gameObject.tag);
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
            if (randomValueBullets < 75) // 75% de probabilidad de soltar munición
            {
                if (playerController.playerData.hasShotgun)
                {
                    int ammoTypeChance = Bullets.Next(0, 100);
                    if (ammoTypeChance < 50) // 50% escopeta
                    {
                        var shotgunShells = Instantiate(ShotgunShellsPrefab);
                        shotgunShells.transform.position = new Vector3(enemy.gameObject.transform.position.X + 3, enemy.gameObject.transform.position.Y, enemy.gameObject.transform.position.Z + 3);
                    }
                    else // 50% boltgun
                    {
                        var boltgunBullets = Instantiate(BoltgunBulletsPrefab);
                        boltgunBullets.transform.position = new Vector3(enemy.gameObject.transform.position.X - 3, enemy.gameObject.transform.position.Y, enemy.gameObject.transform.position.Z - 3);
                    }
                }
                else
                {
                    // Solo boltgun si no tiene escopeta
                    var boltgunBullets = Instantiate(BoltgunBulletsPrefab);
                    boltgunBullets.transform.position = new Vector3(enemy.gameObject.transform.position.X - 3, enemy.gameObject.transform.position.Y, enemy.gameObject.transform.position.Z - 3);
                }
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
            if (randomValueBullets < 75) // 50 chance
            {
                for (int i = 0; i < 2; i++)
                {
                    if (playerController.playerData.hasShotgun)
                    {
                        var shotgunShells = Instantiate(ShotgunShellsPrefab);
                        shotgunShells.transform.position = new Vector3(enemy.gameObject.transform.position.X, enemy.gameObject.transform.position.Y, enemy.gameObject.transform.position.Z - i + 3);
                    }
                    else
                    {
                        var boltgunBullets = Instantiate(BoltgunBulletsPrefab);
                        boltgunBullets.transform.position = new Vector3(enemy.gameObject.transform.position.X, enemy.gameObject.transform.position.Y, enemy.gameObject.transform.position.Z - i + 3);
                    }
                }
            }
            else if (randomValueBullets < 75) // 25 chance (aunque esta condición no se cumple nunca: ya está cubierta por la anterior)
            {
                for (int i = 0; i < 2; i++)
                {
                    var boltgunBullets = Instantiate(BoltgunBulletsPrefab);
                    boltgunBullets.transform.position = new Vector3(enemy.gameObject.transform.position.X - 3, enemy.gameObject.transform.position.Y, enemy.gameObject.transform.position.Z - 3);
                }
            }
            else // 50 chance
            {
                if (playerController.playerData.hasShotgun)
                {
                    var shotgunShells = Instantiate(ShotgunShellsPrefab);
                    shotgunShells.transform.position = new Vector3(enemy.gameObject.transform.position.X, enemy.gameObject.transform.position.Y, enemy.gameObject.transform.position.Z + 3);
                }
                else
                {
                    var boltgunBulletsExtra = Instantiate(BoltgunBulletsPrefab);
                    boltgunBulletsExtra.transform.position = new Vector3(enemy.gameObject.transform.position.X, enemy.gameObject.transform.position.Y, enemy.gameObject.transform.position.Z + 3);
                }

                var boltgunBullets = Instantiate(BoltgunBulletsPrefab);
                boltgunBullets.transform.position = new Vector3(enemy.gameObject.transform.position.X - 3, enemy.gameObject.transform.position.Y, enemy.gameObject.transform.position.Z - 3);
            }
        }
    }

    public void SpawnPrefabFromDestroyableObject(Vector3 position)
    {
        // 12.5% Shotgun Shells
        // 12.5% Boltgun Bullets
        // 75% Nothing

        Random randomGeneral = new Random();
        float randomValueGeneral = randomGeneral.Next(0, 100);

        if (randomValueGeneral < 12.5f) 
        {
            if (randomValueGeneral < 6.25f) 
            {
                // 12.5% chance
                SpawnShotgunShells(position);
            }
            else 
            {
                // 12.5% chance
                SpawnBoltgunBullets(position);
            }
        }
        else
        {
            // 75% chance
        }
    }


    public void SpawnMagnet(Vector3 position)
    {
        var magnet = Instantiate(MagnetPrefab);
        magnet.transform.position = position;
    }

    public void SpawnBlackHeart(Vector3 position)
    {
        var blackHeart = Instantiate(BlackHeartPrefab);
        blackHeart.transform.position = position;
    }

    public void SpawnAmmunitionBlessing(Vector3 position)
    {
        var ammunitionBlessing = Instantiate(AmmunitionBlessingPrefab);
        ammunitionBlessing.transform.position = position;
    }

    public void SpawnChapterStandard(Vector3 position)
    {
        var chapterStandard = Instantiate(ChapterStandardPrefab);
        chapterStandard.transform.position = position;
    }

    public void SpawnShotgunShells(Vector3 position)
    {
        var shotgunShells = Instantiate(ShotgunShellsPrefab);
        shotgunShells.transform.position = position;
    }

    public void SpawnBoltgunBullets(Vector3 position)
    {
        var boltgunBullets = Instantiate(BoltgunBulletsPrefab);
        boltgunBullets.transform.position = position;
    }

    public void SpawnMedicaeStimm(Vector3 position)
    {
        var medicaeStimm = Instantiate(MedicaeStimmPrefab);
        medicaeStimm.transform.position = position;
    }

    public void SpawnPiercingBullets(Vector3 position)
    {
        var piercingBullets = Instantiate(PiercingBulletsPrefab);
        piercingBullets.transform.position = position;
    }

}
          