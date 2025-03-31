using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public abstract class PickUp : MonoBehaviour
{

    public abstract void OnPickUp(PlayerController playerController);

}