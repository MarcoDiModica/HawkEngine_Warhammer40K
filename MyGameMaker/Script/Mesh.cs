using System;
using System.Numerics;
using System.Runtime.CompilerServices;

namespace HawkEngine
{
    public class Mesh : Component
    {
        public Mesh() : base() { }

        public Mesh(UIntPtr instance) : base(instance) { }

        // Properties
        public bool DrawBoundingBox { get; set; }
        public bool DrawTriangleNormals { get; set; }
        public bool DrawVertexNormals { get; set; }
        public bool DrawFaceNormals { get; set; }
        public bool DrawWireframe { get; set; }

        public Vector3 AABBMin { get; set; }
        public Vector3 AABBMax { get; set; }

        public override void Start() { }
        public override void Update(float deltaTime) { }
        public override void Destroy() { }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void Load(UIntPtr instance, Vector3[] vertices, uint[] indices);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void LoadTexCoords(UIntPtr instance, Vector2[] texCoords);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void LoadNormals(UIntPtr instance, Vector3[] normals);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void LoadColors(UIntPtr instance, Vector3[] colors);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void CalculateNormals(UIntPtr instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void Draw(UIntPtr instance);

        // LoadMesh?

        public static Mesh CreateCube()
        {
            UIntPtr instance = CreateCube_Internal();
            return new Mesh(instance);
        }

        public static Mesh CreateSphere()
        {
            UIntPtr instance = CreateSphere_Internal();
            return new Mesh(instance);
        }

        public static Mesh CreatePlane()
        {
            UIntPtr instance = CreatePlane_Internal();
            return new Mesh(instance);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern UIntPtr CreateCube_Internal();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern UIntPtr CreateSphere_Internal();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern UIntPtr CreatePlane_Internal();
    }
}