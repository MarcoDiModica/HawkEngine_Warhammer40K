using System;
using System.Runtime.CompilerServices;

namespace HawkEngine
{
    public abstract class PlayerAbilities
    {
        public UIntPtr CplusplusInstance;

       

        public PlayerAbilities()
        {
            
        }

        public PlayerAbilities(UIntPtr instance)
        {
            CplusplusInstance = instance;
        }

        public abstract void TriggerAbility();

    }
}