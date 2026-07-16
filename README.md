# GVUEDK Framework

**Gabevlogd Unreal Engine Development Kit**

<p align="center">
  <img src="https://img.shields.io/badge/GVUEDK-Unreal%20Engine%20Framework-5A5A5A?style=for-the-badge&logo=unrealengine&logoColor=white" alt="GVUEDK Unreal Engine Framework">
  <br>
  <img src="https://img.shields.io/badge/Unreal%20Engine-5.x-blue" alt="Unreal Engine 5.x">
  <img src="https://img.shields.io/badge/C%2B%2B%20%26%20Blueprints-supported-00599C" alt="C++ and Blueprints">
  <img src="https://img.shields.io/badge/status-active%20development-orange" alt="Active development">
  <img src="https://img.shields.io/badge/license-MIT-green" alt="MIT License">
</p>

## 📖 Overview

**GVUEDK** is my personal, modular framework for **Unreal Engine 5**. It collects reusable gameplay systems, components, subsystems, Blueprint assets, data assets, interfaces, and editor tools developed while working on games and technical prototypes.

The framework has a strong focus on **gameplay programming and 3C systems**—Character, Controller, and Camera—but has gradually expanded to cover encounters, interaction, UI, audio, cinematics, multiplayer sessions, platform services, and general gameplay architecture.

GVUEDK is not intended to be a monolithic engine layer or a complete game template. Each system is packaged as an Unreal Engine plugin so projects can adopt only the functionality they need. Plugins are at different stages of maturity and their APIs may change as the framework evolves.

## ✨ Highlights

- **Modular plugin architecture** — systems are separated into focused Unreal Engine plugins.
- **C++ and Blueprint foundations** — systems may be implemented in C++, Blueprints, or a combination of both, depending on their purpose and intended workflow.
- **Reusable gameplay architecture** — components, subsystems, interfaces, data assets, gameplay tags, and event-driven communication are used to reduce project-specific coupling.
- **3C-oriented systems** — movement, camera, aiming, interaction, input, and grappling tools originated from action-game development.
- **Data-driven workflows** — several systems can be configured through assets and editor properties rather than requiring new code for every use case.
- **Runtime, content, and editor tooling** — plugins may include C++ modules, Blueprint assets, data assets, UI content, custom editor modules, and debugging tools.
- **Selective adoption** — most plugins can be integrated individually, provided that their engine or third-party dependencies are also enabled.

## 🧩 Plugin Catalog

### Core and Gameplay Architecture

| Plugin | Description |
| --- | --- |
| **GVUEDK** | Shared and experimental framework utilities, including player-action management, custom input triggers, dodge support, environment tracing, ground checks, and debug helpers. |
| **GlobalEventsSystem** | Gameplay-tag-based global event bus supporting parameterless events and events carrying a `UObject` payload. |
| **[StateMachineSystem](https://github.com/gabevlogd/StateMachineSystem)** | Component-based state-machine framework with reusable states, gameplay-tag transitions, input handling, transition rules, pause/reset support, multiple concurrent state machines, and optional replicated state data. |
| **CombatEventSystem** | Lightweight world-level combat event channel. The current implementation broadcasts structured death events containing the killer, victim, and damage type. |

### Player, Movement, and Interaction

| Plugin | Description |
| --- | --- |
| **AimAssistSystem** | Configurable target detection and aim correction using distance, angle, collision-channel filtering, target events, and optional adjustment curves. |
| **CompassSystem** | Compass UI framework with cardinal directions, world-space targets, target components, interfaces, and configurable marker widgets. |
| **CustomMovementSystem** | Extension of Unreal's Character Movement Component with custom movement modes, currently including wall running and rail grinding. |
| **DynamicCameraSystem** | Camera framework built around a custom Player Camera Manager, reusable camera modifiers, configurable camera POV data, and a custom spring-arm component. |
| **GrapplingHookSystem** | Behaviour-based grappling framework with configurable gameplay-tag modes, interchangeable target-search strategies, movement behaviours, grab-point interfaces, and runtime events. |
| **InputBindingSystem** | Enhanced Input integration layer for registering mapping contexts and working with player-mappable key profiles and user settings. |
| **InteractionSystem** | Interface-driven interaction framework with pluggable detection behaviours, highlighting, interaction validation, and reusable interactable actors/components. |
| **ResourceAttributeSystem** | Modular resource and stat framework with tagged attributes, min/max values, regeneration, update controls, lifecycle events, and built-in health, shield, and stamina types. |

### World, Encounters, and Progression

| Plugin | Description |
| --- | --- |
| **EncounterSystem** | Data-driven encounter framework combining wave management, enemy tracking, asynchronous spawning, reusable spawn methods, configurable completion rules, and multiple wave execution modes. It includes sequential, random, looping, ping-pong, conditional, custom, and endless flows, together with editor tools for spawn-point placement and wave visualization. |
| **WorldTriggerSystem** | Modular world-trigger framework based on reusable conditions and actions. Supports independent enter/exit logic, AND/OR condition evaluation, delays, retriggering, manual activation, Blueprint-defined extensions, and editor asset factories. |
| **EnvironmentalScannerSystem** | Blueprint-based environmental scanning system for detecting and reacting to nearby objects, surfaces, and other contextual elements. Its functional implementation is provided primarily through assets contained in the plugin's `Content` folder rather than through its C++ module. |

### UI, Audio, and Sequences

| Plugin | Description |
| --- | --- |
| **PopUpSystem** | Data-driven UI pop-up manager using gameplay tags, configurable widget registrations, styling data, runtime parameters, and show/hide/add/remove operations. |
| **AudioEnvironmentSystem** | Middleware-agnostic spatial audio-environment framework with volumes, presets, named parameters, priority/layer blending, configurable falloff, multiple listeners, backend interfaces, and runtime debugging. |
| **FMODEventsManagerSystem** | FMOD integration for managed 2D/3D event playback and soundtrack control, including gameplay-tag registration, parameter updates, queued requests, and dual-channel fades or crossfades. |
| **SequencesManagerSystem** | Level Sequence playback manager with centralized configuration and optional hold-to-skip input handling, gamepad/keyboard bindings, and skip-indicator widgets. |

### Online and Platform Services

| Plugin | Description |
| --- | --- |
| **MultiplayerCore** | Online Subsystem session layer supporting session creation, discovery, friend-session lookup, joining, invites, destruction, ending, and runtime session-setting updates through Blueprint-facing events and utilities. |
| **SteamAchievementsSystem** | Steam achievement subsystem with queued writes, progress updates, offline local persistence and later synchronization, plus optional meta-achievement handling. |

## 🔗 Dependencies and Integration Notes

Most plugins depend only on standard Unreal Engine modules, but some require additional engine plugins or third-party integrations:

| Plugin | Required or notable dependency |
| --- | --- |
| **GVUEDK**, **InputBindingSystem**, **StateMachineSystem** | Unreal Engine **Enhanced Input** plugin. |
| **FMODEventsManagerSystem** | The **FMOD Studio Unreal Engine integration** must be installed and configured separately. |
| **MultiplayerCore** | Unreal's **OnlineSubsystem** and **OnlineSubsystemUtils**, plus a configured online backend when not using LAN sessions. |
| **SteamAchievementsSystem** | **OnlineSubsystemSteam**, Steam App ID/configuration, and the usual Steamworks project setup. |
| **EncounterSystem**, **WorldTriggerSystem** | Include separate editor modules in addition to their runtime modules. |

Some plugins can communicate naturally through gameplay tags or shared Unreal patterns, but the repository does not require every plugin to be enabled at once. Certain systems also depend on Blueprint assets, data assets, widgets, or other resources stored in their `Content` folders, so the complete plugin directory should always be copied rather than only its `Source` folder.

## 🚀 Getting Started

1. Clone or download the repository.
2. Choose the plugin folders required by your project.
3. Copy each complete plugin folder—including its `Source`, `Content`, `Config`, and `Resources` directories when present—into your project, for example:

   ```text
   YourProject
   └── Plugins
       └── GVUEDK
           ├── StateMachineSystem
           ├── InteractionSystem
           └── ...
   ```

4. Enable any required engine or third-party dependencies listed above.
5. Regenerate the project files if required by your development environment.
6. Build the project and enable the selected plugins through the **Unreal Engine Plugin Manager**.

Plugins containing C++ modules must be compiled for the Unreal Engine version and target platform used by the host project. Content-based systems must retain their bundled assets and may require project-specific Blueprint or data-asset configuration before use.

## 📂 Repository Structure

```text
GVUEDK_Framework
└── Plugins
    └── GVUEDK
        ├── AimAssistSystem
        ├── AudioEnvironmentSystem
        ├── CombatEventSystem
        ├── CompassSystem
        ├── CustomMovementSystem
        ├── DynamicCameraSystem
        ├── EncounterSystem
        ├── EnvironmentalScannerSystem
        ├── FMODEventsManagerSystem
        ├── GlobalEventsSystem
        ├── GrapplingHookSystem
        ├── GVUEDK
        ├── InputBindingSystem
        ├── InteractionSystem
        ├── MultiplayerCore
        ├── PopUpSystem
        ├── ResourceAttributeSystem
        ├── SequencesManagerSystem
        ├── StateMachineSystem
        ├── SteamAchievementsSystem
        └── WorldTriggerSystem
```

Individual plugin directories may contain different combinations of `Source`, `Content`, `Config`, `Resources`, and editor-specific modules. A plugin should be treated as a complete unit when copied into another project.

## 🕹 Origin

The first version of GVUEDK was created during the development of **[Negative Edge](https://store.steampowered.com/app/4825940/Negative_Edge_Demo/)**, a third-person action arena shooter where I worked primarily as a **Gameplay Programmer** and **3C Programmer**.

Several systems began as solutions to concrete production needs from that project and were later separated, generalized, and expanded into reusable plugins. Other plugins were developed through later prototypes, framework research, and integration work.

The repository therefore reflects an evolving personal toolkit rather than a finished, all-purpose replacement for Unreal Engine's existing gameplay frameworks.

## 🛠 Unreal Engine Compatibility

- Designed for **Unreal Engine 5.x**.
- The plugins have been developed across projects using different UE5 releases, primarily within the **5.3–5.6** range.
- Not every plugin is guaranteed to have been validated against every engine version in that range.
- Third-party integrations such as FMOD and Steam must use versions compatible with the selected Unreal Engine release.

## ⚠️ Development Status

GVUEDK is under active development. Most plugin descriptors are currently marked as beta, documentation coverage varies between systems, and public APIs may be refactored.

Before using a plugin in production, review its source and bundled content, dependencies, networking assumptions, and project-specific configuration. Bug reports, technical feedback, and implementation discussions are welcome.

## 📌 Roadmap

- [ ] Add dedicated setup and usage documentation for each plugin.
- [ ] Provide example maps and small demonstration projects.
- [ ] Add clearer per-plugin compatibility and dependency information.
- [ ] Introduce automated build checks across supported Unreal Engine versions.
- [ ] Expand tests, validation, and debugging tools for the most reusable systems.
- [ ] Stabilize public APIs and publish versioned releases for mature plugins.

## 📜 License

This project is licensed under the **MIT License**. You may use, modify, and distribute it according to the terms of the repository's license file.

## 🤝 Contributing

GVUEDK is primarily a personal framework, but feedback, suggestions, bug reports, and technical discussions are welcome.
