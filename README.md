
# 🦅 HawkEngine - Physics Branch 🦅 # 

Branch created from the Second version (v0.5) of the [HawkEngine](https://github.com/CITM-UPC-GameEngines-2024-BCN/HawkEngine/tree/PhysicsModule)

## OUR MEMBERS ##

[Samuel Martínez](https://github.com/Samu163)

[Àlex Becerra](https://github.com/Bonnie041008)

[Sergi Gantzer](https://github.com/sgantzer12) 


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
