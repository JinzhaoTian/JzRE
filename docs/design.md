# JzRE Design

## Class Diagram

```mermaid
classDiagram
    direction TB

    %% Interfaces
    class JzISerializable {
        <<Interface>>
        +Serialize()
        +Deserialize()
    }

    class JzIDrawable {
        <<Interface>>
        +Draw()
    }

    class JzIPluginable {
        <<Interface>>
        +AddPlugin()
        +GetPlugin()
        +ExecutePlugins()
    }

    %% Core Engine Classes
    class JzRenderEngine { }

    class JzContext { }

    class JzDevice { }

    class JzWindow { }

    class JzInputManager { }

    JzRenderEngine *-- JzContext
    JzContext *-- JzDevice
    JzContext *-- JzWindow
    JzContext *-- JzInputManager

    %% Editor Related Classes
    class JzEditor { }

    class JzCanvas { }

    class JzPanelsManager { }

    JzRenderEngine *-- JzEditor
    JzEditor *-- JzCanvas
    JzEditor *-- JzPanelsManager
    JzEditor ..> JzContext

    %% UI Related Classes
    class JzUIManager { }

    class JzWidgetContainer { }

    class JzPanel { }

    class JzWidget { }

    JzContext *-- JzUIManager
    JzUIManager ..> JzCanvas
    JzWidgetContainer *.. JzWidget

    %% UI PANEL Related Classes
    class JzPanelMenuBar { }

    class JzMenuBar { }

    class JzPanelTransformable { }

    class JzPanelWindow { }

    class JzAssetBrowser { }

    class JzView { }

    class JzViewControllable { }

    class JzAssetView { }

    class JzSceneView { }

    class JzGameView { }

    class JzConsole { }

    class JzMaterialEditor { }

    JzPanelsManager *-- JzPanel
    JzWidgetContainer <|-- JzPanel
    JzIDrawable <|.. JzPanel
    JzIPluginable <|.. JzPanel

    JzPanel <|-- JzPanelMenuBar
    JzPanelMenuBar <|-- JzMenuBar
    JzPanel <|-- JzPanelTransformable
    JzPanelTransformable <|-- JzPanelWindow
    JzPanelWindow <|-- JzView
    JzView <|-- JzViewControllable
    JzViewControllable <|-- JzAssetView
    JzViewControllable <|-- JzSceneView
    JzPanelWindow <|-- JzAssetBrowser
    JzView <|-- JzGameView
    JzPanelWindow <|-- JzConsole
    JzPanelWindow <|-- JzMaterialEditor

    %% SCENE SYSTEM
    class JzSceneManager { }
    class JzScene { }

    JzContext *-- JzSceneManager
    JzSceneManager *-- JzScene
    JzISerializable <|.. JzScene

    %% SETTINGS
    class JzWindowSettings { }
    class JzDeviceSettings { }

    JzContext *-- JzWindowSettings
    JzDevice ..> JzDeviceSettings
    JzWindow ..> JzWindowSettings
    JzWindow ..> JzDevice
    JzInputManager ..> JzWindow
    JzPanelsManager ..> JzCanvas
```

## Sequence Diagram

```mermaid
sequenceDiagram
    participant Engine as JzRenderEngine
    participant Editor as JzEditor
    participant Context as JzContext
    participant Device as JzDevice
    participant Window as JzWindow
    participant InputMgr as JzInputManager
    participant UIMgr as JzUIManager
    participant SceneMgr as JzSceneManager
    participant Scene as JzScene
    participant PanelMgr as JzPanelsManager
    participant Panel as JzPanel

    %% Initialization
    Engine->>Context: Create Context

    %% Device Initialization
    Context->>Device: Initialize Device
    Device-->>Context: Device Handle
    Context->>Window: Create Context
    Window-->>Context: Window Handle

    Context->>InputMgr: Create Input Manager
    InputMgr-->>Context: Input Manager Handle

    %% UI
    Context->>UIMgr: Create JzUIManager
    UIMgr-->>Context: UI Handle

    %% Scene Initialization
    Context->>SceneMgr: Create JzSceneManager
    SceneMgr-->>Context: Scene Manager Handle

    Context-->>Engine: Success

    %% UI Initialization
    Engine->>Editor: Create Editor

    Editor->>SceneMgr: Load Scene
    SceneMgr->>Scene: Load Default Scene
    activate Scene
    deactivate Scene
    Scene-->>SceneMgr: Scene Reference
    SceneMgr-->>Editor: Success

    Editor->>PanelMgr: Create Panel Manager
    PanelMgr-->>Editor: Panel Manager Handle

    Editor->>PanelMgr: Set UI
    PanelMgr->>Panel: Load Panels
    activate Panel
    Panel->>Panel: Add Menu Bar
    Panel->>Panel: Add Views
    Panel->>Panel: Add Panel Windows
    deactivate Panel
    Panel-->>PanelMgr: Success
    PanelMgr-->>Editor: Success

    Editor-->>Engine: Success

    %% Render Loop
    loop Render Loop
        Engine->>Editor: PreUpdate
        Editor->>Device: Poll Events
        Device-->>Editor: Events
        Editor-->>Engine: Success

        Engine->>Editor: Update
        Editor->>InputMgr: Process Input
        InputMgr-->>Editor: Success
        Editor->>PanelMgr: Update Panel
        PanelMgr-->>Editor: Success
        Editor->>UIMgr: Update UI
        UIMgr-->>Editor: Success
        Editor-->>Engine: Success

        Engine->>Editor: PostUpdate
        Editor->>Context: Process Context
        Context->>Window: Swap Buffers
        Window-->>Context: Success
        Context->InputMgr: Clear Events
        InputMgr-->>Context: Success
        Context-->>Editor: Success
        Editor-->>Engine: Success
    end
```
