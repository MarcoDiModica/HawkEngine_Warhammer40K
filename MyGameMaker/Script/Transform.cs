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
        public extern void SetLocalPosition(float x, float y, float z);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Vector3 GetPosition();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Vector3 GetLocalPosition();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetRotation(float pitch, float yaw, float roll);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Vector3 GetEulerAngles();

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

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void DOMove(Vector3 position, float duration, Modes modes);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void DOMoveX(float positionX, float duration, Modes modes);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void DOMoveY(float positionY, float duration, Modes modes);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void DOMoveZ(float positionZ, float duration, Modes modes);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void DOScale(Vector3 scale, float duration, Modes modes);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void DOScaleX(float scaleX, float duration, Modes modes);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void DOScaleY(float scaleY, float duration, Modes modes);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void DOScaleZ(float scaleZ, float duration, Modes modes);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void DORotate(Vector3 rotation, float duration, Modes modes);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void DORotateX(float rotationX, float duration, Modes modes);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void DORotateY(float rotationY, float duration, Modes modes);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void DORotateZ(float rotationZ, float duration, Modes modes);


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

        //-------------Fields ----------------//
        public Vector3 position
        {
            get
            {
                return GetPosition();
            }
            set
            {
                SetPosition(value.X , value.Y, value.Z);
            }
        }

        public Vector3 eulerAngles
        {
            get
            {
                return GetEulerAngles();
            }
            set
            {
                SetRotation(value.X, value.Y, value.Z);
            }
        }

        public Vector3 forward
        {
            get { return GetForward(); }
            // no setter , read-only property
        }


        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern Vector3 GetForward();
    }
}