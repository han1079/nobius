# Nobius: A Capability-Based Context Composition Framework

## Vision

Nobius is a general-purpose 2D application framework built around a novel architectural pattern: **capability-based context composition**. Rather than hardcoding application modes or using traditional state machines, Nobius treats all application behavior as composable, ephemeral contexts that can be dynamically assembled based on user input gestures.

The core insight: **The context stack IS the application state.** No mode flags, no brittle state machines—just lightweight, composable behaviors that declare what they need and automatically resolve conflicts.

## Architectural Philosophy

### The Three Layers

```
┌─────────────────────────────────────────────────┐
│ Configuration Layer (JSON)                      │
│ - Input gesture → context mappings              │
│ - Available contexts & their capabilities       │
│ - Application composition rules                 │
└─────────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────────┐
│ Framework Layer (C++ - Generic)                 │
│ - Orchestrator: Context stack manager           │
│ - InputSystem: Gesture recognition & dispatch   │
│ - Primitives: Entity, Renderer, VertexAllocator │
└─────────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────────┐
│ Application Layer (C++ - Reusable Contexts)     │
│ - Concrete contexts (SelectTool, CurveEdit)     │
│ - Systems (data operations)                     │
│ - Controllers (multi-frame interactions)        │
└─────────────────────────────────────────────────┘
```

### Core Concepts

#### 1. Primitives
The fundamental building blocks that ALL applications need:
- **EntityManager**: Entity creation, deletion, queries
- **VertexAllocator**: GPU memory management
- **Renderer**: Command-based rendering pipeline
- **InputSystem**: Stateful input accumulation + gesture recognition

#### 2. Contexts
Lightweight, composable units of behavior:
- Define **capabilities** (what primitives they read/write)
- Implement **atomic operations** (on_event, on_update, on_render)
- Contain a **"brain"** (systems, controllers, state machines)
- Are **ephemeral** (created when needed, destroyed when done)

#### 3. Capabilities
Explicit declarations of resource access:
```cpp
enum class Capability {
    ReadEntities,    // Query entities
    WriteEntities,   // Modify/create entities
    ReadInput,       // See input state (shareable)
    ConsumeInput,    // Exclusive input (blocks others)
    WriteTimeline,   // Modify animation timeline
    RenderOverlay,   // Draw UI overlays
    // ... more as needed
};
```

Contexts declare:
- `required_capabilities`: What they need to function
- `exclusive_capabilities`: What they can't share with other contexts

The framework **automatically resolves conflicts** based on capability overlap.

#### 4. Gesture-Based Dispatch
The InputSystem recognizes patterns and pushes/pops contexts:

```json
{
  "pattern": "shift + mouse_left_drag",
  "action": "push_context",
  "context": "MultiSelectContext",
  "on_release": "pop_context"
}
```

User input → gesture recognition → context composition. No manual mode tracking.

## Example: Composing Behaviors

Consider editing a curve while visualizing physics:

```
User Action                    Context Stack
────────────────────────────────────────────────────────────
Start in Select mode           [SelectToolContext]
                               
Ctrl+Click on curve            [SelectToolContext, 
                                CurveEditContext]
                               
Press 'D' for debug            [SelectToolContext,
                                CurveEditContext,
                                PhysicsDebugContext]
                               
Hold Shift to multi-select     [SelectToolContext,
                                CurveEditContext,
                                PhysicsDebugContext,
                                MultiSelectContext]
                               
Release Shift                  [SelectToolContext,
                                CurveEditContext,
                                PhysicsDebugContext]
                               
Press Escape (universal)       [SelectToolContext,
                                CurveEditContext]
```

**Four independent features composed dynamically, with automatic conflict resolution.**

## Data-Driven Applications

The same engine binary + different configuration = completely different application:

```bash
# Drawing application
./nobius --config configs/drawing_app.json

# Graph editor
./nobius --config configs/graph_editor_app.json

# Animation timeline
./nobius --config configs/animation_editor_app.json

# Custom hybrid application
./nobius --config configs/my_custom_app.json
```

Configuration files define:
1. Available contexts
2. Input gesture → context mappings
3. Initial context stack
4. Capability definitions

Application behavior emerges from composition rules, not hardcoded logic.

## Testability

Every context is an isolated, testable unit:

```cpp
// Unit test: Does CurveEditContext handle input correctly?
TEST(CurveEditContext, DragsControlPoint) {
    MockEntityManager entities;
    MockTimelineSystem timeline;
    CurveEditContext ctx(entities, timeline);
    
    InputSystem input;
    input.mouse.position = {100, 100};
    input.mouse.left_button = true;
    
    ctx.on_event(input);
    
    EXPECT_TRUE(timeline.was_modified());
}

// Stress test: Can these contexts coexist?
TEST(ContextComposition, MultiSelectWithCurveEdit) {
    Orchestrator orch;
    
    auto curve_ctx = std::make_unique<CurveEditContext>();
    auto select_ctx = std::make_unique<MultiSelectContext>();
    
    EXPECT_TRUE(orch.try_push_context(std::move(curve_ctx)));
    EXPECT_TRUE(orch.try_push_context(std::move(select_ctx)));
    
    // Verify both contexts receive input
    // Verify no capability conflicts
}

// Conflict test: Ensure exclusive capabilities are enforced
TEST(CapabilitySystem, ExclusiveConflict) {
    Orchestrator orch;
    
    auto modal_dialog = std::make_unique<ModalDialogContext>();
    auto select_tool = std::make_unique<SelectToolContext>();
    
    orch.try_push_context(std::move(modal_dialog));
    
    // Should fail - modal has exclusive input
    EXPECT_FALSE(orch.try_push_context(std::move(select_tool)));
}
```

Contexts are easy to test because:
- They're small (single responsibility)
- They're isolated (explicit dependencies via capabilities)
- They're composable (automatic conflict detection)

## Generalizability

The framework makes minimal assumptions:
- **Primitives** are chosen to support any 2D application
- **Contexts** define their own behavior
- **Capabilities** provide a universal conflict resolution mechanism
- **Gestures** are data-driven and configurable

This isn't a game engine, a drawing app, or an animation tool—it's a **substrate for building any of these** by composing contexts.

### Current Primitives

| Primitive | Purpose | Generality |
|-----------|---------|------------|
| EntityManager | CRUD for scene objects | Any application with objects |
| VertexAllocator | GPU memory management | Any graphical application |
| Renderer | Command-based rendering | Any visual output |
| InputSystem | Gesture recognition | Any interactive application |

With these four primitives, you can build:
- Drawing applications
- Graph editors
- Animation timelines
- Physics simulations
- Node-based editors
- Modal dialog systems
- Debug visualizations
- ... and arbitrary compositions

## Why This Matters

Traditional approaches to application architecture:
- **State machines**: Brittle, hard to compose, exponential state space
- **Mode flags**: Scattered checks, difficult to reason about interactions
- **Layer stacks** (Hazel): Better, but no conflict resolution, hard to query state
- **Entity-Component-System**: Great for entities, not for application-level behavior

Nobius proposes:
- **Context composition**: Behaviors as first-class composable units
- **Capability-based access**: Explicit, verifiable resource usage
- **Gesture-driven dispatch**: Declarative mapping from input to behavior
- **Data-driven configuration**: Application logic as data, not code
## Rapid Application Development

The true power of Nobius emerges when building specialized tools:

**Traditional approach:**
- Start from scratch
- Build UI, input handling, visualization, I/O
- Implement domain logic
- Test everything
- Result: Weeks/months per application

**Nobius approach:**
- Pick pre-built contexts from library (95%)
- Write domain logic as 1-2 custom contexts (5%)
- Compose with config file
- Result: Hours/days per application

### Example: Control System Testbed

```json
{
  "contexts": {
    "from_library": [
      "PlaybackControls", "CameraControls", "PhysicsSimulation",
      "TelemetryLogger", "GraphVisualizer", "StateLoader"
    ],
    "custom": [
      { "name": "PIDController", "brain": "plugins/pid.so" }
    ]
  }
}
```

## Why This Changes Everything

**Before:** Every specialized app is a from-scratch project  
**After:** Every specialized app is a composition exercise

**Before:** Testing means testing the whole app  
**After:** Library contexts have 10,000 hours of testing. Your 50-line domain logic is the only new surface area.

**Before:** Can't reuse components across domains  
**After:** CameraControls works the same in control systems, robotics, fluids, circuits

**Before:** Shipping an app means shipping a monolith  
**After:** Shipping an app means shipping a config file + small plugin

This is **Lego for applications**. You've designed a genuinely reusable component system at the *application behavior level*, not just the entity level.

The control system example is perfect because it shows: **Even "serious" real-time applications can benefit from compositional architecture when the critical path is isolated in a tiny custom context.**



## Acknowledgments

Inspired by:
- Hazel Engine's layer architecture
- Unix philosophy of composition
- Capability-based security models
- Functional reactive programming
- The realization that application state doesn't need explicit tracking if structure implies state
