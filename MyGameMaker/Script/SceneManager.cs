using System;
using System.Runtime.CompilerServices;
using System.Text.Json;

namespace HawkEngine
{
    public class SceneManager
    {
        //internal calls
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool LoadSceneInternal(string sceneName);

        [MethodImpl(MethodImplOptions.InternalCall)]

        private static extern void SetSceneToPlay();

        public static bool isLoadedFromCheckpoint = false;
        public static bool isLevel2 = false;
        public static bool isBossFight = false;

        public static string currentSceneName = "";
        public static string currentLevel = "";

        public static bool isNewGame = false;
        private static float playerHealthPausedTemp;
        private static float playerHealthPaused;

        public static bool loadAmmo = false;

        public static bool isPaused = false;

        //funciones
        public static void LoadScene(string sceneName)
        {
            Tweening.CleanTweens();
            if (LoadSceneInternal("Library/Scenes/" + sceneName + ".scene"))
            {
                currentSceneName = sceneName;
                SetSceneToPlay();
            }
            else
            {
                Engineson.print("Scene not found");
            }
        }


        public static void LoadSceneFromCheckpoint(string sceneName)
        {
            loadAmmo = false;
            Tweening.CleanTweens();
            if (LoadSceneInternal("Library/Scenes/" + sceneName + ".scene"))
            {
                isLoadedFromCheckpoint = true;
                currentSceneName = sceneName;
                SetSceneToPlay();
            }
            else
            {
                Engineson.print("Scene not found");
            }
        }


        public static void LoadSceneWithFade(string sceneName, float fadeTime = 1.0f)
        {
            loadAmmo = false; 
            bool hasBoltgun = true;
            bool hasBoltgunUpgraded = true;
            bool hasShotgun = true;
            bool hasShotgunUpgraded = true;
            bool hasRailgun = true;
            bool hasRailgunUpgraded = true;


            GameObject player = GameObject.Find("Player");
            PlayerInput playerInput = player?.GetComponent<PlayerInput>();
            PlayerController playerController = player?.GetComponent<PlayerController>();
            playerInput?.BlockInput();
            // Audio.PauseAll(); // Removed - let music managers handle transitions naturally

            if (playerController?.playerData != null)
            {
                playerHealthPausedTemp = playerController.playerData.GetHealthTemp();
                playerHealthPaused = playerController.playerData.GetHealth();
                hasBoltgun = playerController.playerData.hasBoltgun;
                hasBoltgunUpgraded = playerController.playerData.BoltgunUpgraded;
                hasShotgun = playerController.playerData.hasShotgun;
                hasShotgunUpgraded = playerController.playerData.ShotgunUpgraded;
                hasRailgun = playerController.playerData.hasRailgun;
                hasRailgunUpgraded = playerController.playerData.RailgunUpgraded;

                playerController.playerData.GodMode = true;
                playerController.playerData.SetTempHealth(playerHealthPausedTemp);
                playerController.playerData.SetHealth(playerHealthPaused);
            }

            GameObject fadeObj = GameObject.Find("FadeController"); // debe tener UIImage + FadeController
            fadeObj.SetActive(true);
            FadeController fader = fadeObj?.GetComponent<FadeController>();

            if (fader != null)
            {
                fader.StartFade(fadeTime, true, () =>
                {

                    playerInput?.UnBlockInput();
                    if (playerController?.playerData != null)
                    {
                        playerController.playerData.GodMode = false;
                        playerController.playerData.SetTempHealth(playerHealthPausedTemp);
                        playerController.playerData.SetHealth(playerHealthPaused);
                        playerController.playerData.biblePages = playerController.GetComponent<RedThirstManager>().biblePages;
                        if (hasBoltgun)
                        {
                            playerController.playerData.hasBoltgun = hasBoltgun;
                            playerController.playerData.BoltgunUpgraded = hasBoltgunUpgraded;
                            playerController.playerData.boltgunCurrentAmmo = playerController.GetComponent<Boltgun>().currentMagazineAmmo;
                            playerController.playerData.boltgunMaxAmmo = playerController.GetComponent<Boltgun>().currentTotalAmmo; 
                            loadAmmo = true;
                        }
                        if (hasShotgun)
                        {
                            playerController.playerData.hasShotgun = hasShotgun;
                            playerController.playerData.ShotgunUpgraded = hasShotgunUpgraded;
                            playerController.playerData.shotgunCurrentAmmo = playerController.GetComponent<Shotgun>().currentMagazineAmmo;
                            playerController.playerData.shotgunMaxAmmo = playerController.GetComponent<Shotgun>().currentTotalAmmo;
                            loadAmmo = true; 
                        }
                        if (hasRailgun)
                        {
                            playerController.playerData.hasRailgun = hasRailgun;
                            playerController.playerData.RailgunUpgraded = hasRailgunUpgraded;
                        }
                    }
                    Tweening.CleanTweens();

                    if (LoadSceneInternal("Library/Scenes/" + sceneName + ".scene"))
                    {
                        currentSceneName = sceneName;
                        SetSceneToPlay();
                    }
                    else
                    {
                        Engineson.print("Scene not found");
                    }
                });
            }
            else
            {
                Engineson.print("FadeController not found in the scene. Cannot perform fade transition.");
                if (LoadSceneInternal("Library/Scenes/" + sceneName + ".scene"))
                {
                    SetSceneToPlay();
                }
                else
                {
                    Engineson.print("Scene not found");
                }
            }
        }

        public static void SetPause(bool pause)
        {
            isPaused = pause;
            GameObject player = GameObject.Find("Player");
            PlayerInput playerInput = player?.GetComponent<PlayerInput>();
            PlayerController playerController = player?.GetComponent<PlayerController>();
            if (pause)
            {

                playerInput?.BlockInput();
                Audio.PauseAll(); // Removed - let music continue during pause
                if (playerController?.playerData != null)
                {
                    playerHealthPausedTemp = playerController.playerData.GetHealthTemp();
                    playerHealthPaused = playerController.playerData.GetHealth();
                    playerController.playerData.GodMode = true;

                }

            }
            else
            {
                playerInput?.UnBlockInput();
                if (playerController?.playerData != null)
                {
                    playerController.playerData.GodMode = false;
                    playerController.playerData.SetTempHealth(playerHealthPausedTemp);
                    playerController.playerData.SetHealth(playerHealthPaused);
                }
                Audio.ResumeAll();
            }
          
        }


        //contructor
        public SceneManager()
        {
            Engineson.print("SceneManager created");
        }
    }
}