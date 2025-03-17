using System;
using System.Runtime.CompilerServices;

namespace HawkEngine
{
    class Engineson
    {
        /* The implementation of theese methods lies in C++ */
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void print(string message);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern GameObject CreateGameObject(string name, GameObject ola);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void Destroy(GameObject object_to_destroy);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void ChangeScene(string scene_name);

        public static int MapComponent(Type type)
        {
            if(type == typeof(Transform))
            {
                return 0;
            }
            else if(type == typeof(MeshRenderer))
            {
                return 1;
            }
            else if(type == typeof(Camera))
            {
                return 2;
            }
            else if(type == typeof(Collider))
            {
                return 3;
            }
            else if(type == typeof(Rigidbody))
            {
                return 4;
            }
            else if(type == typeof(Audio))
            {
                return 5;
            }
            else if (type == typeof(SkeletalAnimation))
            {
                return 6;
            }
            
           

                return -1;
        }

    }
}
