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

        float distance = 6;
        float damage = 100;
        float explosionRadius = 2;

        public GrenadeLauncher(UIntPtr instance) : base(instance)
        {
            CplusplusInstance = instance;
        }
        public override void TriggerAbility()
        {
            GameObject projectile = Engineson.CreateGameObject("Grenade", null);
            Rigidbody rigidbody = projectile.AddComponent<Rigidbody>();
            //rigidbody.AddForce(Engineson.GetForwardVector() * distance, ForceMode.Impulse);
        }
    }    
}