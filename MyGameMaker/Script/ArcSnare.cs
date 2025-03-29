using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HawkEngine
{
    internal class ArcSnare : BaseAbilities
    {
        public string name;
        public bool enabled;
        public float cooldown;
        GameObject explosion;


        private float timer = 0;
        private bool exploded = false;
        GameObject arcsnare;
        BoxCollider collider;
     
        bool canThrow = true;
        private float explosionCooldown = 1.0f;
        private float explosionTimer = 0.0f;

        public override void Awake()
        {

        }
        public override void Start()
        {

        }

        public override void Update(float deltaTime)
        {
            

            // Manejo de la explosión
            if (exploded)
            {
                explosionTimer += deltaTime;
                   

                if (explosionTimer >= explosionCooldown)
                {
                    if (explosion != null)
                    {
                        //Engineson.print("Destruyendo explosión...");
                        Engineson.Destroy(explosion);
                        explosion = null;
                        //Engineson.print("Explosión destruida.");
                    }
                    else
                    {
                        //Engineson.print("ERROR: explosion ya era NULL antes de destruir.");
                    }

                    exploded = false;
                    explosionTimer = 0f;
                }
            }

            // Manejo de la granada (después de la explosión)
            if (!exploded && arcsnare != null && arcsnare.GetComponent<Grenade>().needsDestroy)
            {
                //Engineson.print("Destruyendo granada...");
                Engineson.Destroy(arcsnare.GetComponent<Grenade>().gameObject);
                arcsnare = null;
                canThrow = true;
            }
        }

        public override void TriggerAbility()
        {
            if (canThrow)
            {
                arcsnare = Engineson.CreateGameObject("Grenade", null);
                arcsnare.AddScript("Grenade");
                arcsnare.GetComponent<Grenade>().Init(gameObject.GetComponent<Transform>().GetPosition(), gameObject.GetComponent<Transform>().forward);
                canThrow = false;
            }
        }

        void Explode()
        {
            

            if (arcsnare == null)
            {
                
                return;
            }

            // Crear explosión
            explosion = Engineson.CreateGameObject("Explosion", null);

            if (explosion == null)
            {
                
                return;
            }

            explosion.AddComponent<MeshRenderer>();
            explosion.GetComponent<Transform>().SetPosition(
                arcsnare.GetComponent<Transform>().GetPosition().X,
                arcsnare.GetComponent<Transform>().GetPosition().Y,
                arcsnare.GetComponent<Transform>().GetPosition().Z
            );
            explosion.GetComponent<Transform>().SetScale(4f, 0.25f, 4f);

            exploded = true;


               
        }
    }
}

