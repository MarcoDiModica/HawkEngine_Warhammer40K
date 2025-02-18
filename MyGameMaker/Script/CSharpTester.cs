using System;

namespace HawkEngine
{
    public class CSharpTester
    {
        public float MyPublicFloatVar = 5.0f;

        public void PrintFloatVar()
        {
            Console.WriteLine("MyPublicFloatVar: " + MyPublicFloatVar);
        }

        private void IncrementFloatVar(float value)
        {
            MyPublicFloatVar += value;
        }
    }

}