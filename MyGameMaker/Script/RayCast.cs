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

        public struct RaycastHit
        {
            public GameObject gameObject;
            public Vector3 point;
            public Vector3 normal;
            public float distance;
        }

        public RaycastHit hit;

        public void PerformRaycast(Vector3 origin, Vector3 direction, float maxDistance)
        {
            hit.gameObject = Raycast(origin, direction, maxDistance, out hit.point, out hit.normal, out hit.distance);
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern static GameObject Raycast(Vector3 origin, Vector3 direction, float maxDistance, out Vector3 hitPoint, out Vector3 normal, out float distance);
    }
}
