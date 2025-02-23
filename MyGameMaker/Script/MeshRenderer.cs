using System;
using System.Numerics;
using System.Runtime.CompilerServices;

namespace HawkEngine
{
    public class MeshRenderer : Component
    {
        public UIntPtr MeshInstance;
        public UIntPtr MaterialInstance;
        private Vector3 color = new Vector3(1.0f, 1.0f, 1.0f);
        private GameObject owner;

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetMesh(UIntPtr rendererInstance, UIntPtr meshInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetCubeMesh(UIntPtr rendererInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern UIntPtr GetMesh(UIntPtr rendererInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetMaterial(UIntPtr rendererInstance, UIntPtr materialInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern UIntPtr GetMaterial(UIntPtr rendererInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetColor(UIntPtr rendererInstance, Vector3 color);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Vector3 GetColor(UIntPtr rendererInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void Render(UIntPtr rendererInstance);

        public MeshRenderer(UIntPtr native, UIntPtr nativeMesh, UIntPtr nativeMaterial, GameObject owner)
        {
            CplusplusInstance = native;
            MeshInstance = nativeMesh;
            MaterialInstance = nativeMaterial;
            this.owner = owner;
        }

        public MeshRenderer()
        {
            Engineson.print("MeshRenderer created");
        }

        public override void Start() { }
        public override void Update(float deltaTime) { }
        public override void Destroy() { }
    }

}