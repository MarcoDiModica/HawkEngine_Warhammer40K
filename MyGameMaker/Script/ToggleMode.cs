using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static Railgun;

namespace HawkEngine
{
    internal class ToggleMode : BaseAbilities
    {

        public RailgunMode railgunMode;
        public override void Awake()
        {

        }
        public override void Start()
        {

        }

        public override void Update(float deltaTime)
        {


        }

        public override void TriggerAbility()
        {
            if (railgunMode == RailgunMode.SEMIAUTOMATIC)
            {
                railgunMode = RailgunMode.AUTOMATIC;
            }
            else
            {
                railgunMode = RailgunMode.SEMIAUTOMATIC;
            }
        }

        public override void ResetCooldowns()
        {
        }
    }
}
