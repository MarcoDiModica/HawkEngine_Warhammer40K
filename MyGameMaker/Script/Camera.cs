using System;
using System.Numerics;
using System.Runtime.CompilerServices;

namespace HawkEngine
{
    public class Camera : Component
    {

        //[MethodImplAttribute(MethodImplOptions.InternalCall)]
        //public extern static void SetMainCamera();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetCameraFieldOfView(double fieldOfView);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetCameraNearClipPlane(float nearClipPlane);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetCameraFarClipPlane(float farClipPlane);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetCameraAspect(float aspect);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetCameraOrthographicSize(float orthographicSize);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetCameraProjectionType(int projectionType);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetFollowTarget(GameObject target, Vector3 offset, float distance, bool followX, bool followY, bool followZ, float smoothness);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetDistance(float distance);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetOffset(Vector3 offset);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern float GetCameraFieldOfView();

        public bool ortographic
        {
            get
            {
                return ortographic;
            }
            set
            {
                ortographic = value;
            }
        }

        public float orthographicSize
        {
            get
            {
                return orthographicSize;
            }
        }

        public GameObject owner;

        public Camera(UIntPtr nativeCamera, GameObject owner)
        {
            CplusplusInstance = nativeCamera;
            this.owner = owner;
        }

        public Camera()
        {
            //Engineson.print("Camera created");
        }

        public override void Awake() { }
        public override void Start() { }
        public override void Update(float deltaTime)
        {
            if (ortographic)
            {
                SetCameraProjectionType(1);
            }
            else
            {
                SetCameraProjectionType(0);
            }

        }
        public override void Destroy() { }
    }
}