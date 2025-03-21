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

        //funciones
        public static void LoadScene(string sceneName)
        {
            if (LoadSceneInternal("Library/Scenes/" + sceneName + ".scene"))
            {
               SetSceneToPlay();
            }
            else
            {
                Engineson.print("Scene not found");
            }
        }

        //contructor
        public SceneManager()
        {
            Engineson.print("SceneManager created");
        }
    }
}