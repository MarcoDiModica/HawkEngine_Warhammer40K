using System;

namespace HawkScriptCore
{
    public class CSharpTester
    {
        public float MyPublicFloatVar = 5.0f;

        CSharpTester()
        {
            Console.WriteLine("CSharpTester Constructor");
        }

        ~CSharpTester()
        {
            Console.WriteLine("CSharpTester Destructor");
        }

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