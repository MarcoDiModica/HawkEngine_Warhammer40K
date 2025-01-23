# 🦅 HawkEngine - Physics Branch 🦅 # 

Branch created from the Second version (v0.5) of the [HawkEngine](https://github.com/CITM-UPC-GameEngines-2024-BCN/HawkEngine/tree/PhysicsModule)

Release link: https://github.com/CITM-UPC-GameEngines-2024-BCN/HawkEngine/releases/tag/v1.0.0_Physics


## OUR MEMBERS ##

[Samuel Martínez](https://github.com/Samu163) (aka Kepler)

<img src="https://github.com/user-attachments/assets/3781794f-f1f6-4543-bbbb-917e0c22ee95" alt="djsaiodioasho" width="125" height="125" />

- For this delivery Samuel (Kepler) was in charge of setting up Bullet 3D and creating the collider component. 

[Àlex Becerra](https://github.com/Bonnie041008) (aka Newton)

<img src="https://github.com/user-attachments/assets/e7ce5ee6-deb3-4932-aa55-3c54f1c87145" alt="IMG-20240108-WA0011" width="125" height="125" />

- For this delivery Àlex (Newton) was in charge of creating the shooting balls and the initial setup for the car

[Sergi Gantzer](https://github.com/sgantzer12) (aka Planck)

<img src="https://github.com/user-attachments/assets/e6db36f9-3594-4178-aab0-a1aa1f733e00" alt="Imagen de WhatsApp" width="125" height="125" />

- For this delivery Sergi (Planck) helped implement the constraints in the WorldEnvironment and created the car.


# CONTROLS #

- Press Left Click on the inspector to select an object

- Use Mouse Wheel to zoom in and out

- Hold Right Click and use WASD to move freely across the scene + Q/E to make the camera move upwards/downwards

- Hold Right Click and Shift to make the camera double the speed

- Press F if an Object is selected to make the camera look at an object

- You can press the Middle Mouse Button to move the camera across the screen

## OTHER CONTROLS ##

- You can move/rotate the objects with the mouse on the inspector window

- You can delete GameObjects in the hierarchy

- You can drag and drop textures and meshes directly on the window

- Right click hierarchy to create objects; drag to parent/unparent; right click object to destroy or create empty child

- To add components, click Add Component in the Inspector. Each component has its own settings in the Inspector

- You can modify the Editor Camera in Settings Panel

- To open and close tabs, Click view in the GameBar and toggle the desired panels.

- To save a scene, File->Save Scene; To create a scene, File->New Scene; To load a scene, right click the scene in the library and load

## PHYSICS CONTROLS ##

- Press F1 to activate Physics Mode!!
- Scroll MouseWheel UP to spawn balls (projectiles)
- Scroll MouseWheel DOWN to spawn the car

## FEATURES ##

- New revamped dockable UI look

- Scene and Camera serialization

- Asset/Library folder to visualise custom file formats

- Model Importer (problems in texture coordinates)

- Parenting and object creation within hierarchy and menu bar

- GameObject components: Camera, Light, MeshRenderer, Transform

- BoundinBoxes, MousePicking, Frustum Culling

- Shader Module (work in progress - almost finished)

- Octree (work in progress - implemented but not functional)

## PHYSICS FEATURES ##

- The initial scene is loaded with colliders that interact with other rigid bodies
- The ball spawning acts as a "shooting" system where the ball has initial force, restitution, gravity, etc.

## PHYSICS ISSUES ##

- In the PhysicsTesting branch, the gizmos was visible for all objects. Once we fixed the issues and moved it to the PhysicsModule, the gizmos is no longer visible.
- When spawning the car, despite (in theory) having the chassis parented  to the wheels and having variables for customization of height, suspension, width, etc, the car collides and clips with the ground plane and/or the wheels, making it fly away really fast.

## LICENSES ##

This project is licensed under an unmodified MIT license, which is an OSI-certified license that allows static linking 
with closed source software. Check our LICENSE* for further details.

## VIDEO & GIFS ##


https://github.com/user-attachments/assets/b3d9c1d9-17d8-4cdb-ae7d-bf89fe555000


![boing](https://github.com/user-attachments/assets/0b3d2ee4-1a03-4454-a086-c57ab0c95047)


![boingboing](https://github.com/user-attachments/assets/0d0b2f73-0ec6-4fa4-8d7c-b8f2c6666878)

## BULLET 3D LICENSE ##

The files in this repository are licensed under the zlib license, except for the files under 'Extras' and examples/ThirdPartyLibs.

Bullet Continuous Collision Detection and Physics Library
http://bulletphysics.org

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it freely,
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
