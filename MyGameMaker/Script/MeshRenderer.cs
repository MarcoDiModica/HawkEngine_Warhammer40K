using System;
using System.Numerics;
using System.Runtime.CompilerServices;

namespace HawkEngine
{
    public class MeshRenderer : Component
    {
        private GameObject owner;

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetColor(Vector4 color);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Vector4 GetColor();

        public MeshRenderer(UIntPtr native, GameObject owner)
        {
            CplusplusInstance = native;
            this.owner = owner;
        }

        public MeshRenderer()
        {
            //Engineson.print("MeshRenderer created");
        }

        public override void Awake() { }
        public override void Start() { }
        public override void Update(float deltaTime) { }
        public override void Destroy() { }
    }

}