using HawkEngine;
using System;
using System.Numerics;
using System.Runtime.CompilerServices;

namespace HawkEngine
{
    public class RayCast
    {
        public RayCast()
        {

        }

        public GameObject hitGameObject;

        public GameObject PerformRaycast(Vector3 origin, Vector3 direction, float maxDistance)
        {
            hitGameObject = Raycast(origin, direction, maxDistance);
            return hitGameObject;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern static GameObject Raycast(Vector3 origin, Vector3 direction, float maxDistance);
    }
}
