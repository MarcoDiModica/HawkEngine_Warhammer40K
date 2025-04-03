using System;
using System.Runtime.CompilerServices;

namespace HawkEngine
{
    public abstract class BaseAbilities : MonoBehaviour
    {
        private float abilityCooldown;
        public BaseAbilities()
        {
            
        }

        public BaseAbilities(UIntPtr instance)
        {
            CplusplusInstance = instance;
        }

        public abstract void ResetCooldowns();
        

        
        public abstract void TriggerAbility();

    }
}