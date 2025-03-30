using System;
using System.Runtime.CompilerServices;
using System.Numerics;

namespace HawkEngine
{
    public class Tweening
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void DOValue(float startValue, float endValue, float duration, Modes modes);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void DOColor(Vector4 startColor, Vector4 endColor, float duration, Modes modes);

        public Tweening()
        {
            Engineson.print("Tweening default constructor");
        }

    }

    public enum Modes
    {
        EASE_IN,
        EASE_OUT,
        EASE_IN_OUT,
        LINEAR
    }
}