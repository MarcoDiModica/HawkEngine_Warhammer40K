using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Numerics;
using System.Text.Json;
using HawkEngine;

public class Checkpoint : MonoBehaviour
{
    private PlayerController playerController;
    private PlayerShooting playerShooting;
    private PlayerData playerData;
    private PlayerPowerUp playerPowerUp;
    private GameObject player;
    private Vector3 spawnPoint;
    private bool isCheckpointActive = false;
    public List<GameObject> enemies = new List<GameObject>();

    [Serializable]
    public class CheckpointData
    {
        public float playerCurrentHealth { get; set; }

        public float playerCurrentTemporalHealth { get; set; }

        public int boltgunCurrentAmmo { get; set; }
        public int shotgunCurrentAmmo { get; set; }

        public bool hasBoltgun{ get; set; }
        public bool hasShotgun{ get; set; }
        public bool hasRailgun { get; set; }

        public bool isBoltgunUpgraded{ get; set; }
        public bool isShotgunUpgraded{ get; set; }
        public bool isRailgunUpgraded { get; set; }

        public bool hasMedicaeStimm { get; set; }
        public bool hasAmmunitionBlessing { get; set; }
        public bool hasMagnet { get; set; }
        public bool hasPiercingBullets { get; set; }

        public float playerSpawnX { get; set; }
        public float playerSpawnY { get; set; }
        public float playerSpawnZ { get; set; }

        public List<bool> areEnemiesDead { get; set; } = new List<bool>();
    }

    

    public override void Awake()
    {
        player = GameObject.Find("Player");
        playerController = player.GetComponent<PlayerController>();
        playerShooting = player.GetComponent<PlayerShooting>();
        playerPowerUp = player.GetComponent<PlayerPowerUp>();
        playerData = PlayerData.Instance;
        spawnPoint = GetComponent<Transform>().position;
        spawnPoint.Y = player.GetComponent<Transform>().position.Y;

        Engineson.print(GameObject.FindGameObjectsWithTag("Melee").Length.ToString());

        for (int i = 0; i < GameObject.FindGameObjectsWithTag("Melee").Length; i++)
        {
            enemies.Add(GameObject.FindGameObjectsWithTag("Melee")[i]);
            Engineson.print(enemies[i].name);
        }

        for (int i = 0; i < GameObject.FindGameObjectsWithTag("Ranged").Length; i++)
        {
            enemies.Add(GameObject.FindGameObjectsWithTag("Ranged")[i]);
            Engineson.print(enemies[i].name);
        }

        for (int i = 0; i < GameObject.FindGameObjectsWithTag("Warrior").Length; i++)
        {
            enemies.Add(GameObject.FindGameObjectsWithTag("Warrior")[i]);
            Engineson.print(enemies[i].name);
        }

        for (int i = 0; i < GameObject.FindGameObjectsWithTag("Stalker").Length; i++)
        {
            enemies.Add(GameObject.FindGameObjectsWithTag("Stalker")[i]);
            Engineson.print(enemies[i].name);
        }

    }

    
    public override void Start()
    {
    
    }

    public override void Update(float deltatime)
    {
        if (playerData.GetHealth() <= 0)
        {
            LoadCheckPointData();
        }
    }

    public CheckpointData checkpointData;

    public void SaveCheckpointData()
    {
        checkpointData = new CheckpointData()
        {
            playerSpawnX = spawnPoint.X,
            playerSpawnY = spawnPoint.Y,
            playerSpawnZ = spawnPoint.Z,
            playerCurrentHealth = playerData.GetHealth(),
            playerCurrentTemporalHealth = playerData.GetHealthTemp(),
            boltgunCurrentAmmo = playerShooting.boltgun.currentTotalAmmo,
            shotgunCurrentAmmo = playerShooting.shotgun.currentTotalAmmo,
            hasBoltgun = playerData.hasBoltgun,
            hasShotgun = playerData.hasShotgun,
            hasRailgun = playerData.hasRailgun,
            isBoltgunUpgraded = playerData.BoltgunUpgraded,
            isShotgunUpgraded = playerData.ShotgunUpgraded,
            isRailgunUpgraded = playerData.RailgunUpgraded,
            hasMedicaeStimm = playerPowerUp.hasMedicaeStimm,
            hasAmmunitionBlessing = playerPowerUp.hasAmmunitionBlessing,
            hasMagnet = playerPowerUp.hasMagnet,
            hasPiercingBullets = playerPowerUp.hasPiercingBullets
           


        };
        
        for(int i = 0; i < enemies.Count; i++)
        {
            if (enemies[i] != null)
            {
                if (enemies[i].tag == "Melee")
                {
                    if (enemies[i].GetComponent<EnemyControllerMelee>().isDead)
                    {
                        checkpointData.areEnemiesDead.Add(true);
                    }
                    else
                    {
                        checkpointData.areEnemiesDead.Add(false);
                    }

                }
                else if (enemies[i].tag == "Ranged")
                {
                    if (enemies[i].GetComponent<EnemyControllerRanged>().isDead)
                    {
                        checkpointData.areEnemiesDead.Add(true);
                    }
                    else
                    {
                        checkpointData.areEnemiesDead.Add(false);
                    }

                }
                else if (enemies[i].tag == "Stalker")
                {
                    if (enemies[i].GetComponent<EnemyControllerStalker>().isDead)
                    {
                        checkpointData.areEnemiesDead.Add(true);
                    }
                    else
                    {
                        checkpointData.areEnemiesDead.Add(false);
                    }

                }
                else if (enemies[i].tag == "Warrior")
                {
                    if (enemies[i].GetComponent<EnemyControllerWarrior>().isDead)
                    {
                        checkpointData.areEnemiesDead.Add(true);
                    }
                    else
                    {
                        checkpointData.areEnemiesDead.Add(false);
                    }

                }
                else
                {
                    checkpointData.areEnemiesDead.Add(false);
                }
            }
            
        }

        string json = JsonSerializer.Serialize<CheckpointData>(checkpointData);
        File.WriteAllText("Serialized/checkpointData.json", json);

        Engineson.print(json);
    }

    public void LoadCheckPointData()
    {
        CheckpointData checkpointData = JsonSerializer.Deserialize<CheckpointData>(File.ReadAllText("Serialized/checkpointData.json"));

        playerData.SetHealth(checkpointData.playerCurrentHealth);
        playerData.SetTempHealth(checkpointData.playerCurrentTemporalHealth);
        playerShooting.boltgun.currentTotalAmmo = checkpointData.boltgunCurrentAmmo;
        playerShooting.shotgun.currentTotalAmmo = checkpointData.shotgunCurrentAmmo;
        playerData.hasBoltgun = checkpointData.hasBoltgun;
        playerData.hasShotgun = checkpointData.hasShotgun;
        playerData.hasRailgun = checkpointData.hasRailgun;
        playerData.BoltgunUpgraded = checkpointData.isBoltgunUpgraded;
        playerData.ShotgunUpgraded = checkpointData.isShotgunUpgraded;
        playerData.RailgunUpgraded = checkpointData.isRailgunUpgraded;
        playerPowerUp.hasMedicaeStimm = checkpointData.hasMedicaeStimm;
        playerPowerUp.hasAmmunitionBlessing = checkpointData.hasAmmunitionBlessing;
        playerPowerUp.hasMagnet = checkpointData.hasMagnet;
        playerPowerUp.hasPiercingBullets = checkpointData.hasPiercingBullets;
        Vector3 spawnPos = new Vector3(checkpointData.playerSpawnX, checkpointData.playerSpawnY, checkpointData.playerSpawnZ);
        player.GetComponent<Collider>().SetPosition(spawnPos);

        for (int i = 0; i < enemies.Count; i++)
        {
            if (enemies[i] != null)
            {
                if (checkpointData.areEnemiesDead[i] == true)
                {
                    if (enemies[i].tag == "Melee")
                    {
                        enemies[i].GetComponent<EnemyControllerMelee>().isDead = true;
                    }
                    else if (enemies[i].tag == "Ranged")
                    {
                        enemies[i].GetComponent<EnemyControllerRanged>().isDead = true;

                    }
                    else if (enemies[i].tag == "Stalker")
                    {
                        enemies[i].GetComponent<EnemyControllerStalker>().isDead = true;

                    }
                    else if (enemies[i].tag == "Warrior")
                    {
                        enemies[i].GetComponent<EnemyControllerWarrior>().isDead = true;

                    }
                }
                else
                {
                    if (enemies[i].tag == "Melee")
                    {
                        enemies[i].GetComponent<EnemyControllerMelee>().isDead = false;
                        enemies[i].GetComponent<EnemyControllerMelee>().ResetEnemyCheckPoint();
                    }
                    else if (enemies[i].tag == "Ranged")
                    {
                        enemies[i].GetComponent<EnemyControllerRanged>().isDead = false;
                        enemies[i].GetComponent<EnemyControllerRanged>().ResetEnemyCheckPoint();

                    }
                    else if (enemies[i].tag == "Stalker")
                    {
                        enemies[i].GetComponent<EnemyControllerStalker>().isDead = false;
                        enemies[i].GetComponent<EnemyControllerStalker>().ResetEnemyCheckPoint();

                    }
                    else if (enemies[i].tag == "Warrior")
                    {
                        enemies[i].GetComponent<EnemyControllerWarrior>().isDead = false;
                        enemies[i].GetComponent<EnemyControllerWarrior>().ResetEnemyCheckPoint();

                    }
                }

                
            }
        }

        Engineson.print(spawnPos.ToString());
    }

    public override void OnTriggerEnter(GameObject other)
    {
        if (other.tag == "Player")
        {
            SaveCheckpointData();
            Engineson.print("PlayerDetected");
        }

    }
}