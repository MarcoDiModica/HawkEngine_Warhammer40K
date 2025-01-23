
# 🦅 HawkEngine Scripting🦅 #

Preliminar approach to the scripting mechanisms of The [Engineson](https://github.com/CITM-UPC-GameEngines-2024-BCN/HawkEngine/blob/scripting.05) (former HawkEngine). This is the final project for the Game Engines subject, present in Barcelona's Game Development & Design degree, UPC (TTC)

## OUR MEMBERS ##

[Marco Baldán](https://github.com/Marco-v-BaldanII)

[Marco di Modica](https://github.com/MarcoDiModica)

![smaller_github_image](https://github.com/user-attachments/assets/f955f5f8-faa2-436e-9f16-2e8abad89754)

Implemented ScriptComponent, refactorised Scripting entire solution for organisation purposes, some C# binding, general cleanup and error solving and Inspector Script visualisation (wip).

[Jordi Tur](https://github.com/Jordopol) 

![image](https://github.com/user-attachments/assets/cb62d16b-1dcb-485c-a54f-875e61abd02d)

Implemented Transform and MeshRenderer Components C# translations and bindings, developed various script components and thoroughly tested the functions.

[Dídac García](https://github.com/D1dii)

![smaller_github_image_2](https://github.com/user-attachments/assets/8b41f1d5-24c7-457c-9e1d-c0de226f629f)

Implemented Input and Camera class with all their functions

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

## SCRIPTING CONTROLS ##

- UP ARROW: move tank , move projectile (z positive).

- DOWN ARROW: move tank, move projectile (z negative).

- LEFT ARROW: move tank, move projectile (x positive).

- RIGHT ARROW: move tank, move projectile (x negative).

- D: reset projectile position.

- SPACE (HOLD) + MOVING MOUSE: reorient turret.

- E (DISABLED): instantiate new projectile.

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

## SCRIPTING FEATURES ##

- Added MonoBehaviour environment.

- Added ScriptComponent (editor interactability work in progress).

- Added Engine Binds to translate between Cpp and C# functions.

- Added C# translations for GameObject, TransformComponent, MeshRendererComponent and CameraComponent.

- Added a simple example to portray ScriptComponent functionalities.

## PROBLEMS TO SOLVE ##

- Some core functionalities need to either be implemented or changed.

- Creating new C# Components is a tedius procedure.

- Interactability via the editor is not finished.

- Many improvements for user experience should be performed.

- Scripts work always, including when not in play mode.

## GIFS ##

![2025-01-23 21-28-22](https://github.com/user-attachments/assets/54af0e23-5aab-4898-b67d-9209790702b7)

![2025-01-23 21-27-28](https://github.com/user-attachments/assets/854108ad-ce55-4fe8-a15a-e2accf9d0589)

## LICENSES ##

This project is licensed under an unmodified MIT license, which is an OSI-certified license that allows static linking 
with closed source software. Check our LICENSE* for further details.
