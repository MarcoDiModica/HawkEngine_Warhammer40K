using System;
using System.Runtime.CompilerServices;
using System.Numerics;

namespace HawkEngine
{
    public static class Physics
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern GameObject[] OverlapSphere(Vector3 position, float radius, string tag);
    }
}