using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Numerics;
using HawkEngine;
using System.Diagnostics;
using static System.Net.Mime.MediaTypeNames;
using System.Runtime.InteropServices;
using System.Security.Policy;

public class ProjectileScript : MonoBehaviour
{
    Transform transform;

    public override void Start()
    {
        transform = GetComponent<Transform>();
    }

    public override void Update(float deltaTime)
    {
      
    }

    // Delegar los métodos de TankController sobre los proyectiles
}