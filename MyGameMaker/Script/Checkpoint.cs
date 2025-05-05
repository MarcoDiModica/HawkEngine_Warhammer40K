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

    [Serializable]
    public class CheckpointData
    {
        public float playerCurrentHealth { get; set; }

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

        //public List<Vector3> enemiesPositions = new List<Vector3>();
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
    }

    
    public override void Start()
    {
    
    }

    public override void Update(float deltatime)
    {
        if (Input.GetKeyDown(KeyCode.F7))
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
        

        string json = JsonSerializer.Serialize<CheckpointData>(checkpointData);
        File.WriteAllText("Serialized/checkpointData.json", json);

        Engineson.print(json);
    }

    public void LoadCheckPointData()
    {
        CheckpointData checkpointData = JsonSerializer.Deserialize<CheckpointData>(File.ReadAllText("Serialized/checkpointData.json"));

        playerData.SetHealth(checkpointData.playerCurrentHealth);
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