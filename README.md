# 🦅 HawkEngine 🦅 #

## OUR MEMBERS ##

[Marco Baldán](https://github.com/Marco-v-BaldanII)

![406182133-f955f5f8-faa2-436e-9f16-2e8abad89754](https://github.com/user-attachments/assets/c772cb8b-8550-4f84-9078-067bf60e0a97)

Imported mono , managed initialization. Figured out and layed the groundwork for binding classes from C++ to C#. Created and binded the C# GameObject and the Engineson class.

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

This Shader Sub-system has been worked on by 

[Marc Avante Caballe](https://github.com/MarcoXAvante)

<img src="https://github.com/user-attachments/assets/f1aae89c-c225-4486-837c-d07e25270a84"  width="125" height="125">

Marc improved the water shader fixed all the problems with merging the main and mantaining the clean code

[Salvador Bernades Iglesias](https://github.com/SalvaBernades)

<img src="https://github.com/user-attachments/assets/8a6c54bf-d038-4427-9174-2e66982bf848"  width="125" height="125">

Salva made Save Shaders with an array & change GO shaders and helped Joan with integration in the inspector

[Joan Marques Besses](https://github.com/joanmarquesbesses)

<img src="https://github.com/user-attachments/assets/9b90a359-e898-46ce-9296-c30f7f5cd664"  width="125" height="125">

Joan made the shader template, implementation to the inspector and the shader component

[Justo Jose Tiscornia Sierra](https://github.com/Jusstox)

<img src="https://github.com/user-attachments/assets/7dc35c0a-f959-430d-ae43-226ec5788871" width="125" height="125">

Justo Made the library linking, default shader integration and organizated the readME 


## SCRIPTING CONTROLS ##

- UP ARROW: move tank , move projectile (z positive).

- DOWN ARROW: move tank, move projectile (z negative).

- LEFT ARROW: move tank, move projectile (x positive).

- RIGHT ARROW: move tank, move projectile (x negative).

- D: reset projectile position.

- SPACE (HOLD) + MOVING MOUSE: reorient turret.

- E (DISABLED): instantiate new projectile.

## FEATURES ##

This branch Includes a new test of a water shader with foam 

You can chose the tyoe of shader you want!

## LICENSES ##

This project is licensed under an unmodified MIT license, which is an OSI-certified license that allows static linking 
with closed source software. Check our LICENSE* for further details.


##VIDEO AND GIF##

[Shader Video!](https://youtu.be/j0p4dMhfLWY)

![gifa](https://github.com/user-attachments/assets/43ec0552-ac5d-489c-a151-82ceea3b8657)

![gifb](https://github.com/user-attachments/assets/07375d54-2e56-41d5-9daa-62ee68266a61)

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