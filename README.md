
# 🦅 HawkEngine - Audio Branch 🦅 #

Second version (v0.5) of the [HawkEngine](https://github.com/CITM-UPC/HawkEngine)

## MEMBERS ##

[Yiwei Ye](https://github.com/Yeeway69)

![yiwei photo](https://github.com/user-attachments/assets/cb78a926-4833-4bdc-945c-2d891b20f55f)

-Main Designer and Programmer for this AudioEngine, implemented Base OpenAL integration, Music and sound effect playing, SoundComponent, AudioListener, and AudioAsset Processer and its adjacent things.


[Biel Liñán](https://github.com/Drauguer)

![Proyecto nuevo](https://github.com/user-attachments/assets/e0bfeb15-2245-45f9-b512-d7f967429530)

- In charge of implementing audio components as part of the components that can be added to the game object and updating the info of said components in the inspector.


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

  
## AUDIO CONTROLS ##

- Click add component on a gameobject to add audio source or audio listener
- Drag your audio file to the path on a Sound Component in the desired gameobject inside the inspector


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

## AUDIO FEATURES ##

### Audio test window ###
- Test Varaious Core AudioEngine Features, like Play Muisc/Sound clip, stop, pause/resume, Loop, print Device info,  volume control,
  Customized Effect Position and Audio Listener Position
  
### SoundComponent ###
- Plays Music/Effect sounds
- Stop and Pause/resume
- Supports Spatial Audio (toggle)
- Individual Volume setting field
- Loop toggle
- AutoPlay toggle
- Can be attached to GameObjects

### Audio Listener ###
- Can be Attacheched to any GameObejct (usually a Camera)
- the AudioListener component represents the "ears" of the 3D audio system. It updates the audio engine with the position of the listener in 3D space.
- Only one AudioListener should be active at a time

## AUDIO KNOWN ISSUES##

### Spatial Audio ###
- No orientation/rotation support yet
- Due to Unsolved reason we can see the Spatial Audio related system working however we can not hear the 3D sounds in effects.

## LICENSES ##

This project is licensed under an unmodified MIT license, which is an OSI-certified license that allows static linking 
with closed-source software. Check our LICENSE* for further details.


## VIDEOS & GIFS
