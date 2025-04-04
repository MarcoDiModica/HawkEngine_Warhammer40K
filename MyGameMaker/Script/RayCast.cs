using HawkEngine;
using System;
using System.Numerics;
using System.Runtime.CompilerServices;

namespace HawkEngine
{
    public class RayCast
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool Raycast(Vector3 origin, Vector3 direction, float maxDistance);
    }
}
