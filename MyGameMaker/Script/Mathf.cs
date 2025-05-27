using System;
using System.Numerics;

namespace HawkEngine
{
    public static class Mathf
    {

        public const float PI = (float)Math.PI;

        public const float Deg2Rad = PI / 180f;

        public const float Rad2Deg = 180f / PI;


        public static float Clamp(float value, float min, float max)
        {
            if (value < min) return min;
            if (value > max) return max;
            return value;
        }

        public static float Clamp01(float value) => Clamp(value, 0f, 1f);

        public static float Lerp(float a, float b, float t)
        {
            return a + (b - a) * Clamp01(t);
        }

        public static Vector3 LerpVector3(Vector3 start, Vector3 end, float t)
        {
            t = Clamp01(t);
            return new Vector3(
                Lerp(start.X, end.X, t),
                Lerp(start.Y, end.Y, t),
                Lerp(start.Z, end.Z, t)
            );
        }

        public static Quaternion Slerp(Quaternion a, Quaternion b, float t)
        {
            t = Clamp01(t);
            return Quaternion.Slerp(a, b, t);
        }

        public static float MoveTowards(float current, float target, float maxDelta)
        {
            if (Math.Abs(target - current) <= maxDelta)
                return target;
            return current + Math.Sign(target - current) * maxDelta;
        }

        public static float Abs(float value) => Math.Abs(value);
        public static float Sign(float value) => Math.Sign(value);
        public static float SmoothStep(float from, float to, float t)
        {
            t = Clamp01(t);
            t = t * t * (3f - 2f * t);
            return from + (to - from) * t;
        }

        public static float Repeat(float t, float length)
        {
            return t - (float)(Math.Floor(t / length)) * length;
        }

        public static float PingPong(float t, float length)
        {
            t = Repeat(t, length * 2f);
            return length - Abs(t - length);
        }

        public static float Acos(float f)
        {
            if (f < -1f) return (float)Math.PI;
            if (f > 1f) return 0f;
            return (float)Math.Acos(f);
        }
    }
}
