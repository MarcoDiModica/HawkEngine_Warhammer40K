using System;
using System.Collections;
using System.Collections.Generic;

namespace HawkEngine
{
    public static class CoroutineManager
    {
        private static readonly List<Coroutine> coroutines = new List<Coroutine>();

        public static void Start(IEnumerator routine)
        {
            coroutines.Add(new Coroutine(routine));
        }

        public static void Update(float deltaTime)
        {
            for (int i = coroutines.Count - 1; i >= 0; i--)
            {
                Coroutine c = coroutines[i];

                if (c.CurrentYield is WaitForSeconds wait)
                {
                    c.WaitTimer += deltaTime;
                    if (c.WaitTimer < wait.Duration)
                        continue;
                    c.WaitTimer = 0f;
                }

                if (!c.Routine.MoveNext())
                {
                    coroutines.RemoveAt(i);
                    continue;
                }

                c.CurrentYield = c.Routine.Current;
            }
        }
    }
}
