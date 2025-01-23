
# 🦅 HawkEngine - Skeletal Animation Branch🦅 #

Second version (v0.5) of the [HawkEngine](https://github.com/CITM-UPC/HawkEngine)

## OUR MEMBERS ##

[Marc San José](https://github.com/marcsjm19)

![MarcSanJoseFoto1](https://github.com/user-attachments/assets/42dca714-ed9e-470f-a54f-220053572e7e)



...

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

## SKELETAL ANIMATION CONTROLS ##

- Click add component on a gameobject to add skeletal animation source in the inspector
- It should open a window to control the skeleton of the .fbx (not working)


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

## SKELETAL ANIMATION FEATURES ##

- Skeletal animation window should be displayed

## SKELETAL ANIMATION ISSUES ##

- Tried to achieve what it was required but it doesn't import correctly the GameObject or the animation

- It gets laggy when pressing the Skeletal Animation Component when clicking Add Component in the inspector UI (code is commented in order to not affect the rest of the engine)

- Needs to be modified to work as expected (we will keep working on it)

## LINK TO THE REPOSITORY BRANCH ##

[HawkEngine - Skeletal Animation Branch](https://github.com/CITM-UPC-GameEngines-2024-BCN/HawkEngine/tree/Animation)

## LICENSES ##

This project is licensed under an unmodified MIT license, which is an OSI-certified license that allows static linking 
with closed source software. Check our LICENSE* for further details.
