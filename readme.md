# C++ Real-Time Engine Project
*A learning-oriented real-time engine sandbox for mastering timing, concurrency, memory, and rendering systems.*

---

## Overview
This project is a **low-level real-time engine sandbox** built in C++ with SDL2, OpenGL, and ImGui.  
Its purpose is to expose and explore **how real-time systems behave** under load, contention, and complex interactions.  
Unlike traditional game engines, this project prioritizes **transparency**, **debuggability**, and **mechanical clarity** over performance or abstraction.

The engine acts as a laboratory for studying:

- real-time timing and synchronization  
- event ingestion and state transitions  
- memory fragmentation & custom allocators  
- rendering order & sampling theory  
- architecture & system coupling  
- CPU → GPU update chains  

---

# Core Goals

## 1. Understand Real-Time System Behavior
The project examines:
- update loop timing  
- fixed vs variable timestep  
- aliasing, jitter, stutter  
- concurrency under contention  
- state machine ordering and causality  

The goal is to make engine behavior observable as if through an oscilloscope.

## 2. Build a Custom Real-Time Engine Architecture
The system follows the pipeline:

StateUpdater - WorldUpdater - Renderer


Each stage has clear responsibilities:

| Component       | Responsibility |
|-----------------|----------------|
| **StateUpdater** | Converts raw events (SDL/ImGui) into engine commands (modes, inputs, toggles). |
| **WorldUpdater** | Applies those commands to world state (geometry, transforms, memory updates). |
| **Renderer**     | Performs OpenGL rendering and ImGui overlays. Rendering never mutates state. |

This matches real-engine patterns while remaining simple enough to study deeply.

## 3. Explore Sampling Theory in Engine Design
Concepts borrowed from DSP include:
- **Temporal sampling** (tick rates)  
- **Spatial sampling** (geometry density, aliasing)  
- **Artifact analysis** (jitter, tearing, frame pacing)  

This connects signal theory with simulation/rendering behavior.

## 4. Custom Memory Architecture (Allocator Sandbox)
A major component is a fully custom allocator:

- interval/fragment tracking  
- center-merge coalescing  
- sorted-by-offset validation  
- overlap and OOB detection  
- compactification (defragmentation)  
- UUID → block mapping  
- safe custom iterators  
- double-buffered integrity checking  

This parallels GPU buffer management and ECS-style memory design.

## 5. Clean Integration of SDL2, ImGui, and OpenGL
The project explores:
- ImGui global state and safe access patterns  
- OpenGL loader embedding  
- deterministic render ordering  
- DockSpace-as-root windowing  
- viewport and state propagation between subsystems  

---

# Current State of Ideation

## A. Architecture Stabilizing
- Orchestrator owns configuration and update ordering  
- State and World update stages cleanly separated  
- Event ingestion decoupled from ImGui/SDL internals  

## B. Allocator Work Ongoing
Work includes:
- pointer-safe iterators  
- coalesce safety  
- interval sorting via vector views  
- pointer preservation guarantees  
- rebuild-via-swap compactification  

## C. Rendering Order Established
Render order is:

1. Scene Geometry
2. Imgui Overlay
3. Swap Buffers


## D. ImGui Integration Being Refined
Including:
- loader behavior  
- docking interactions  
- `.ini` persistence  
- decoupled ImGui state usage  

---

# Long-Term Vision

## A. Real-Time Engine “Microscope”
A tool that visualizes:
- tick and frame timing  
- memory maps and fragmentation  
- event timelines  
- buffer lifetimes  
- world state transitions  

## B. Platform for Experiments
Future experiments may explore:
- multithreaded pipelines  
- ECS-style entity memory  
- GPU-resident geometry  
- physics stepping models  
- alternate scheduling algorithms  

## C. Potential Applications
- CAD-style tools  
- simulation visualizers  
- educational engine demos  
- procedural geometry editors  
- DSP visualization tools  

---

# Types of Questions Driving the Project

## Memory & Data Structure Integrity
- pointer stability  
- coalesce safety  
- vector/optional semantics  
- iterator design  
- reference lifetimes  

## System Architecture & Coupling
- ownership boundaries  
- configuration placement  
- Orchestrator-mediated sharing  
- friend semantics & encapsulation  

## Rendering & Event Ingestion
- key repeat and SDL semantics  
- render ordering  
- clean ImGui usage without reliance on globals  

## Language-Level Mechanics
- copy vs move semantics  
- optional/reference_wrapper behavior  
- operator overloading for sort/comparison  
- container safety rules  

## Tooling & Build System
- OpenGL loader behavior  
- CMake and file root semantics  
- submodule resets and diffing  

---

# Technologies
- **C++20**  
- **SDL2**  
- **OpenGL 3.x**  
- **Dear ImGui**  
- **CMake**  

---

# Why This Project Matters
This engine is deliberately built from first principles so that you can **observe every step of the real-time pipeline**:

input - state - world - gpu - frame

Every subsystem is exposed, inspectable, and built to teach.

