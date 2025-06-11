# 🦅 HawkEngine 🦅

HawkEngine is a game engine built in C++20, featuring C# scripting via Mono and a robust ImGui-based editor. It supports advanced rendering, animation, audio, physics, and UI systems, making it ideal for both 2D and 3D game development.

---

## ✨ Main Features

### 🎬 Animation / Skeletal Animation
- **Skeletal Animation**: Import and play skeletal animations with support for multiple animation clips, blending, and real-time control. The engine supports bone hierarchies and GPU skinning for efficient character animation.
- **Animator Component**: Attach to GameObjects to manage animation states, transitions, and parameters, enabling complex character behaviors.

### 🔊 Audio System (FMOD)
- **FMOD Integration**: Leverage FMOD for high-quality audio playbook, including 2D/3D sound, spatialization, and real-time effects.
- **Audio Components**: Add sound sources and listeners to GameObjects, with support for music, ambient sounds, and SFX.
- **Audio Manager**: Centralized control for all audio events, volume, and playback states.

### 🖥️ Editor Interface (ImGui)
- **ImGui-based Editor**: Fast, responsive, and fully customizable editor interface using ImGui.
- **Dockable Panels**: Includes Hierarchy, Inspector, Console, Scene View, Game View, Asset Browser, and more.
- **Theme Support**: Easily switch between color schemes and layouts.

### 🕵️ Inspector
- **Component Editing**: Inspect and modify all GameObject components in real time.
- **Property Reflection**: Automatic display and editing of properties for both C++ and C# components, supporting custom editors.

### 📚 Asset Library
- **Asset Browser**: Browse, import, and manage assets such as models, textures, audio files, and scripts.
- **Drag & Drop**: Assign assets to GameObjects and components via drag-and-drop for rapid iteration.

### 🌳 Hierarchy Editor
- **Scene Graph Visualization**: View and manage the parent-child relationships of all GameObjects in the scene.
- **Multi-Selection & Grouping**: Select, group, and manipulate multiple objects simultaneously.

### 🖼️ Viewport / Game Viewport
- **Scene Viewport**: Real-time rendering of the scene with camera controls, gizmos, and object manipulation.
- **Game Viewport**: Preview the game as it will run, including UI overlays and post-processing effects.

### 💬 Console
- **Log Output**: View engine logs, warnings, and errors in real time.
- **Command Input**: Execute engine commands and scripts directly from the console.

### 🎮 Input System (SDL)
- **Unified Input Handling**: Keyboard, mouse, and gamepad support using SDL.
- **Custom Bindings**: Map actions to keys or buttons for flexible and user-friendly controls.

### 🌎 Scene Management & Serialization
- **Scene System**: Create, load, and save multiple scenes with full support for undo/redo.
- **Serialization**: Save and load scenes in both YAML and binary formats for flexibility and performance.
- **Prefab System**: Create reusable GameObject templates for rapid prototyping.

### 🏗️ GameObject - Components
- **Built-in Components**:
  - Camera, Light, MeshRenderer, Transform, ParticleFX, ScriptComponent, Physics (RigidBody, Colliders), UI (Canvas, Image, Button, Text, etc.)

### 📦 Resource Manager
- **Asset Caching**: Efficient loading and reuse of meshes, textures, materials, and audio.
- **Hot Reloading**: Update assets in the editor without restarting the engine.

### 💻 Scripting System (C# / Mono CLI)
- **Mono Integration**: Write gameplay logic in C# with full access to engine APIs.
- **Live Reloading**: Hot-reload scripts during development for rapid iteration.
- **Component Model**: Attach C# scripts as components to GameObjects, supporting inheritance and custom logic.

### 🔄 Tweening
- **Animation Tweens**: Smoothly interpolate values for UI, transforms, and more.
- **Custom Easing**: Support for various easing functions to create natural motion.

### ✨ Particle System
- **Preset Library**: Built-in particle presets (smoke, fire, explosions, etc.) for quick effects.
- **Custom Emitters**: Create and configure new particle effects with full control over emission, shape, and animation.
- **Instanced Rendering**: Efficient GPU-based particle rendering for high performance.

### 🧲 Physics (Bullet)
- **Bullet Physics Integration**: Realistic rigid body dynamics, collision detection, and constraints.
- **Collider Components**: Box, Sphere, Capsule, and Mesh colliders for versatile physical interactions.
- **Physics Materials**: Control friction, restitution, and mass for each object.

### 🖌️ UI System
- **UI Canvas**: Hierarchical UI system for HUDs, menus, and overlays.
- **UI Components**: Image, Button, Text, and custom widgets, all editable in the editor.
- **Editor Integration**: Design and preview UI layouts directly in the editor.

---

## 👥 Contributors

- Marco Di Modica [@MarcoDiModica](https://github.com/MarcoDiModica)
- Pau Mora [@Peluchomp](https://github.com/Peluchomp)
- David Lastra [@DavidLastra8](https://github.com/DavidLastra8)
- Ivo Montes [@Ivomm9](https://github.com/Ivomm9)
- Didac Garcia [@D1dii](https://github.com/D1dii)
- Alex Becerra [@Bonifacioo04](https://github.com/Bonifacioo04)
- Marc Avante [@MarcoXAvante](https://github.com/MarcoXAvante)
- Samuel Martinez [@Samu163](https://github.com/Samu163)
- Jordi Tur [@Jordopol](https://github.com/Jordopol)
- Marco Baldan [@Marco-v-Baldanll](https://github.com/Marco-v-Baldanll)
- Joan Marques [@joanmarquesmesses](https://github.com/joanmarquesmesses)
- Noel Colon [@Noelswag](https://github.com/Noelswag)
- Salva Bernades [@SavaBernades](https://github.com/SavaBernades)
- Yiwey ye [@Yeeway69](https://github.com/Yeeway69)
- Biel Linan [@Drauguer](https://github.com/Drauguer)
- Miguel Tamano [@migon25](https://github.com/migon25)
- Pablo Longaron [@PabloL2003](https://github.com/PabloL2003)
- Daniel Berrio [@Onnda](https://github.com/Onnda)
- Pol Celaya [@pcelaya](https://github.com/pcelaya)
- Justo Tiscornia [@Jusstox](https://github.com/Jusstox)
- Andrea Dona [@poderoculto5](https://github.com/poderoculto5)
- Sergi Gantzer [@sgantzer12](https://github.com/sgantzer12)
- Oliver [@Esnaiper](https://github.com/Esnaiper)

---

For more information, visit the [HawkEngine GitHub Repository](https://github.com/CITM-UPC/HawkEngine).
