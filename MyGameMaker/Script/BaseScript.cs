using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


public class Test
{

    public static int testValue = 0;
        public void Start()
        {
            testValue = 23;
            HawkEngine.EngineCalls.print("Hello from C#");
        }
    


}

