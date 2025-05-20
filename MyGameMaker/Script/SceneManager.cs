using System;
using System.Runtime.CompilerServices;

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

        //funciones
        public static void LoadScene(string sceneName)
        {
            Tweening.CleanTweens();


           

            if (LoadSceneInternal("Library/Scenes/" + sceneName + ".scene"))
            {
               SetSceneToPlay();
            }
            else
            {
                Engineson.print("Scene not found");
            }
        }

        public static void LoadSceneFromCheckpoint(string sceneName)
        {
            Tweening.CleanTweens();

            if (LoadSceneInternal("Library/Scenes/" + sceneName + ".scene"))
            {
                isLoadedFromCheckpoint = true;
                SetSceneToPlay();
            }
            else
            {
                Engineson.print("Scene not found");
            }
        }

        public static void LoadSceneWithFade(string sceneName, float fadeTime = 1.0f)
        {
           
            GameObject player = GameObject.Find("Player");
            PlayerInput playerInput = player?.GetComponent<PlayerInput>();
            PlayerController playerController = player?.GetComponent<PlayerController>();
            playerInput?.BlockInput();
            Audio.PauseAll();

            if (playerController?.playerData != null)
            {
                playerController.playerData.GodMode = true;
            }

            GameObject fadeObj = GameObject.Find("FadeController"); // debe tener UIImage + FadeController
            FadeController fader = fadeObj?.GetComponent<FadeController>();

            if (fader != null)
            {
                fader.StartFade(fadeTime, true, () =>
                {
                  
                    playerInput?.UnBlockInput();
                    if (playerController?.playerData != null)
                    {
                        playerController.playerData.GodMode = false;
                    }
                    Tweening.CleanTweens();

                    if (LoadSceneInternal("Library/Scenes/" + sceneName + ".scene"))
                    {
                        SetSceneToPlay();
                    }
                    else
                    {
                        Engineson.print("Scene not found");
                    }
                });
            }
        }

        //contructor
        public SceneManager()
        {
            Engineson.print("SceneManager created");
        }
    }
}