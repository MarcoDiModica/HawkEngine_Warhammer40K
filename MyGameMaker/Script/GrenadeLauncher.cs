using System;
using System.Runtime.CompilerServices;
using System.Numerics;
using System.Runtime.InteropServices;

namespace HawkEngine
{
    public class GrenadeLauncher : PlayerAbilities
    {

        protected string name;
        protected bool enabled;
        protected float cooldown;

        

        public GrenadeLauncher(UIntPtr instance) : base(instance)
        {
            CplusplusInstance = instance;
        }
        public override void TriggerAbility()
        {
            GameObject projectile = Engineson.CreateGameObject("Grenade", null);
            projectile.AddComponent<MeshRenderer>();
            projectile.AddComponent<Collider>();
            projectile.AddScript("Grenade");
        }
    }    
}