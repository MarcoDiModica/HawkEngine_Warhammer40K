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
    private int loadSceneContinueTimer = 0;

    [Serializable]
    public class CheckpointData
    {
        public float playerCurrentHealth { get; set; }
        public float playerCurrentTemporalHealth { get; set; }
        public int boltgunCurrentAmmo { get; set; }
        public int shotgunCurrentAmmo { get; set; }
        public bool hasBoltgun { get; set; }
        public bool hasShotgun { get; set; }
        public bool hasRailgun { get; set; }
        public bool isBoltgunUpgraded { get; set; }
        public bool isShotgunUpgraded { get; set; }
        public bool isRailgunUpgraded { get; set; }
        public bool hasMedicaeStimm { get; set; }
        public bool hasAmmunitionBlessing { get; set; }
        public bool hasMagnet { get; set; }
        public bool hasPiercingBullets { get; set; }
        public List<bool> areEnemiesDead { get; set; } = new List<bool>();
        public string savedSceneName { get; set; }
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

        AddEnemiesByTag("Melee");
        AddEnemiesByTag("Ranged");
        AddEnemiesByTag("Warrior");
        AddEnemiesByTag("Stalker");

        if (SceneManager.isNewGame)
        {
            // Reset logical data
            playerData.hasBoltgun = false;
            playerData.hasShotgun = false;
            playerData.hasRailgun = false;
            playerData.BoltgunUpgraded = false;
            playerData.ShotgunUpgraded = false;
            playerData.RailgunUpgraded = false;

            // Opcional: también resetea upgrades o powerups si persisten en memoria
            playerPowerUp.hasMedicaeStimm = false;
            playerPowerUp.hasAmmunitionBlessing = false;
            playerPowerUp.hasMagnet = false;
            playerPowerUp.hasPiercingBullets = false;

            // Reset player health
            playerData.SetHealth(100f);
            playerData.SetTempHealth(100f);

            ResetCheckPointData();

            SceneManager.isLoadedFromCheckpoint = true;
            SceneManager.isNewGame = false;
        }
    }

    private void AddEnemiesByTag(string tag)
    {
        GameObject[] found = GameObject.FindGameObjectsWithTag(tag);
        foreach (var enemy in found)
        {
            enemies.Add(enemy);
            Engineson.print(enemy.name);
        }
    }

    public override void Update(float deltatime)
    {
        if (SceneManager.isLoadedFromCheckpoint)
        {
            if (loadSceneContinueTimer >= 0)
            {
                LoadCheckPointData();
                SceneManager.isLoadedFromCheckpoint = false;
                loadSceneContinueTimer = 0;
            }
        }

        if (playerData.GetHealth() <= 0)
        {
            SaveCheckpointData();
        }
    }

    public CheckpointData checkpointData;

    public void SaveCheckpointData()
    {
        // Force-refresh weapon and data state before saving
        playerData = PlayerData.Instance;
        playerShooting = player.GetComponent<PlayerShooting>();

        checkpointData = new CheckpointData()
        {
            playerCurrentHealth = 100f,
            playerCurrentTemporalHealth = 100f,
            boltgunCurrentAmmo = playerShooting?.boltgun?.currentTotalAmmo ?? 0,
            shotgunCurrentAmmo = playerShooting?.shotgun?.currentTotalAmmo ?? 0,
            hasBoltgun = playerData?.hasBoltgun ?? false,
            hasShotgun = playerData?.hasShotgun ?? false,
            hasRailgun = playerData?.hasRailgun ?? false,
            isBoltgunUpgraded = playerData?.BoltgunUpgraded ?? false,
            isShotgunUpgraded = playerData?.ShotgunUpgraded ?? false,
            isRailgunUpgraded = playerData?.RailgunUpgraded ?? false,
            hasMedicaeStimm = playerPowerUp?.hasMedicaeStimm ?? false,
            hasAmmunitionBlessing = playerPowerUp?.hasAmmunitionBlessing ?? false,
            hasMagnet = playerPowerUp?.hasMagnet ?? false,
            hasPiercingBullets = playerPowerUp?.hasPiercingBullets ?? false,
            savedSceneName = SceneManager.currentSceneName
        };

        foreach (var enemy in enemies)
        {
            if (enemy == null)
            {
                checkpointData.areEnemiesDead.Add(false);
                continue;
            }
            bool dead = false;
            if (enemy.tag == "Melee")
                dead = enemy.GetComponent<EnemyControllerMelee>()?.isDead ?? false;
            else if (enemy.tag == "Ranged")
                dead = enemy.GetComponent<EnemyControllerRanged>()?.isDead ?? false;
            else if (enemy.tag == "Stalker")
                dead = enemy.GetComponent<EnemyControllerStalker>()?.isDead ?? false;
            else if (enemy.tag == "Warrior")
                dead = enemy.GetComponent<EnemyControllerWarrior>()?.isDead ?? false;
            checkpointData.areEnemiesDead.Add(dead);
        }

        string json = JsonSerializer.Serialize(checkpointData);
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
    }

    public override void OnTriggerEnter(GameObject other)
    {
        if (other.tag == "Player")
        {
            SaveCheckpointData();
            Engineson.print("PlayerDetected");
        }
    }

    public void ResetCheckPointData()
    {
        checkpointData = new CheckpointData()
        {
            playerCurrentHealth = 100f,
            playerCurrentTemporalHealth = 100f,
            boltgunCurrentAmmo = 180,
            shotgunCurrentAmmo = 24,
            hasBoltgun = false,
            hasShotgun = false,
            hasRailgun = false,
            isBoltgunUpgraded = false,
            isShotgunUpgraded = false,
            isRailgunUpgraded = false,
            hasMedicaeStimm = false,
            hasAmmunitionBlessing = false,
            hasMagnet = false,
            hasPiercingBullets = false
        };

        string json = JsonSerializer.Serialize<CheckpointData>(checkpointData);
        File.WriteAllText("Serialized/checkpointData.json", json);
    }
}