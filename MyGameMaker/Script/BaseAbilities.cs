using System;
using System.Runtime.CompilerServices;

namespace HawkEngine
{
    public abstract class BaseAbilities : MonoBehaviour
    {
   
        public BaseAbilities()
        {
            
        }

        public BaseAbilities(UIntPtr instance)
        {
            CplusplusInstance = instance;
        }

        public abstract void TriggerAbility();

    }
}