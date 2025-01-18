using System;
using System.Runtime.CompilerServices;
using System.Numerics;

namespace HawkEngine
{
    public class Transform : Component
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetPosition(float x, float y, float z);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetRotation(float x, float y, float z);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetRotationQuat(Quaternion rotation);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void Rotate(float radians, Vector3 axis);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void RotateLocal(float radians, Vector3 axis);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void LookAt(Vector3 target);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetScale(float x, float y, float z);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void Scale(float x, float y, float z);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void TranslateLocal(float x, float y, float z);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void AlignToGlobalUp(Vector3 worldUp);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetForward(Vector3 forward);


        //private UIntPtr _nativeTransform;
        private GameObject owner;

        public Transform(UIntPtr nativeTransform, GameObject owner)
        {
            CplusplusInstance = nativeTransform;
            this.owner = owner;
        }

        public Transform()
        {
            Engineson.print("transform created");
        }

        public override void Start() { }
        public override void Update(float deltaTime) { }
        public override void Destroy() { }


        // Faltan algunos m�todos 
        // Probablemente tenga que a�adir getters y setters para las propiedades de la transformaci�n
    }
}