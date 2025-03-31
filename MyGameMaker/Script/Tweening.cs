using System;
using System.Runtime.CompilerServices;
using System.Numerics;

namespace HawkEngine
{
    public class Tweening
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void DOValue(ref float value, float startValue, float endValue, float duration, Modes mode);

        //esto es lo mismo que hacer un DoColor, ya que un color es un vector4
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void DOVector4(ref Vector4 color, Vector4 startColor, Vector4 endColor, float duration, Modes mode);

        //haz tambien un DOVector3 porfa

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