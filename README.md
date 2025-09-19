# GVUEDK Framework  
**Gabevlogd Unreal Engine Development Kit**

<p align="center">
  <img src="https://img.shields.io/badge/GVUEDK-Unreal%20Engine%20Framework-5A5A5A?style=for-the-badge&logo=unrealengine&logoColor=white">
  <br>
  <img src="https://img.shields.io/badge/Unreal%20Engine-5.x-blue">
  <img src="https://img.shields.io/badge/status-in%20development-orange">
  <img src="https://img.shields.io/badge/license-MIT-green">
</p>

## 📖 Overview
GVUEDK (Gabevlogd Unreal Engine Development Kit) is my personal framework for **Unreal Engine development**.  
It provides a collection of modular systems and utilities that I’ve built to streamline game development, prototyping, and 3C programming.

## ✨ Key Features
- 🧩 **Plugin-based architecture** → every system is developed as an **Unreal Engine plugin** for maximum modularity.  
- 🎮 **Gameplay & 3C systems** → initially designed for action-oriented gameplay.  
- 🔄 **Constantly evolving** → updated and expanded as I progress with new projects.  
- ⚡ **Independent usage** → each plugin can be used individually, without the need to import the entire framework.  


## 🕹 Origin
The first version of GVUEDK was created during the development of **_[The Negative Edge](https://eventhorizonschool.itch.io/the-negative-edge)_**, a **third-person action arena shooter**, where I worked as a:  
- 🎯 **Gameplay Programmer**  
- 🎥 **3C (Camera, Controller, Character) Programmer**  

Since it was developed within a team, this framework does **not yet cover all the core systems** required for a full game production.  


## 📌 Roadmap
- [ ] Expand base gameplay systems.  
- [ ] Add more generalized features.  
- [ ] Create sample projects/demos for each plugin.  
- [ ] Documentation and usage examples.  


## 🚀 Getting Started
1. Clone or download the repository.  
2. Copy the desired plugins into your Unreal Engine project’s `Plugins` folder.  
3. Enable them via the **Unreal Engine Plugin Manager**.  


## 📂 Repository Structure

```text
GVUEDK_Framework
└── Plugins
    └── GVUEDK
        ├── AimAssistSystem
        ├── CompassSystem
        ├── CustomMovementSystem
        ├── DynamicCameraSystem
        ├── EnvironmentalScannerSystem
        ├── GlobalEventsSystem
        ├── GrapplingHookSystem
        ├── GVUEDK
        ├── InputBindingSystem
        ├── InteractionSystem
        ├── PopUpSystem
        ├── ResourceAttributeSystem
        └── StateMachineSystem
```

- **AimAssistSystem** → Context-aware targeting assistance for improved aiming accuracy with gamepad.  
- **CompassSystem** → Navigation and directional UI system.  
- **CustomMovementSystem** → Unreal Character Movement Component extension.  
- **DynamicCameraSystem** → Adaptive camera behavior with smooth transitions based on player actions.  
- **EnvironmentalScannerSystem** → Detects and reacts to surrounding objects, surfaces, and interactable elements.  
- **GlobalEventsSystem** → Centralized event broadcasting and listening for efficient game-wide communication.
- **GrapplingHookSystem** → Grappling hook mechanics.  
- **GVUEDK** → Generic utilities and not yet system structured code.  
- **InputBindingSystem** → Flexible input mapping system for easy reconfiguration. 
- **InteractionSystem** → Context-based interaction with world objects and entities.
- **PopUpSystem** → Modular UI pop-ups and notifications. 
- **ResourceAttributeSystem** → Modular stat and resource management (e.g., health, stamina).  
- **[StateMachineSystem](https://github.com/gabevlogd/StateMachineSystem)** → Structured actor state handling for consistent behavior.  


## 🛠 Unreal Engine Compatibility
- Currently tested with **Unreal Engine 5.5**  
- Future updates will maintain compatibility with UE5.x  


## 📜 License
This project is licensed under the **MIT License** – feel free to use it in your own projects.


## 🤝 Contributing
This is a personal framework, but feedback, suggestions, and discussions are always welcome!  

