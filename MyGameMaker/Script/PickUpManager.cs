using HawkEngine;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HawkEngine
{
    internal class PickUpManager
    {

        public void DestroyPickUp(GameObject pickUp)
        {
            Engineson.Destroy(pickUp);
        }

    }
}
