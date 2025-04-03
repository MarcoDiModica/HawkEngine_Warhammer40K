using System.Collections.Generic;
using System.Numerics;
using HawkEngine;
using static BaseWeapon;

public class ToggleMode : BaseAbilities
{
   public Railgun railgun;

    public override void Awake()
    {
        railgun = gameObject.GetComponent<Railgun>();
    }
    public override void Start()
    {

    }

    public override void Update(float deltaTime)
    {


    }

    public override void TriggerAbility()
    {
        if (railgun != null)
        {
            railgun.ChangeMode();
        }
    }

    public override void ResetCooldowns()
    {

    }
    
}
