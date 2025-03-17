using System;
using System.Runtime.CompilerServices;

namespace HawkEngine
{
    public class SceneManager
    {
        //internal calls
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void LoadSceneInternal(string sceneName);
        
        //funciones
        public static void LoadScene(string sceneName)
        {
            LoadSceneInternal("Library/Scenes/" + sceneName + ".scene");
        }

        //contructor
        public SceneManager()
        {
            Engineson.print("SceneManager created");
        }
    }
}