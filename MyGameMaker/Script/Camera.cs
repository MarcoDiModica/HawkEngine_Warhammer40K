using HawkEngine;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace HawkEngine
{
    class Camera : Component
    {

        //[MethodImplAttribute(MethodImplOptions.InternalCall)]
        //public extern static void SetMainCamera();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetCameraFieldOfView(float fieldOfView);

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

        private UIntPtr _nativeCamera;
        private GameObject owner;

        public Camera(UIntPtr nativeCamera, GameObject owner)
        {
            _nativeCamera = nativeCamera;
            this.owner = owner;
        }

        public Camera()
        {
            Engineson.print("Camera created");
        }

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