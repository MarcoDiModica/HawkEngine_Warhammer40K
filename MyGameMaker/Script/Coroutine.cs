// CoroutineSystem.cs
using System;
using System.Collections;
using System.Collections.Generic;

namespace HawkEngine
{
    public class Coroutine
    {
        public IEnumerator Routine;
        public float WaitTimer;
        public object CurrentYield;

        public Coroutine(IEnumerator routine)
        {
            Routine = routine;
            CurrentYield = null;
            WaitTimer = 0f;
        }
    }
}