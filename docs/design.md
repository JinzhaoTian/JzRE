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

    %% Initialization Context
    Engine->>Context: Create Context
    activate Context

    Context->>Device: Initialize Device
    activate Device
    Device-->>Context: Device Handle
    deactivate Device

    Context->>Window: Create Context
    activate Window
    Window-->>Context: Window Handle
    deactivate Window

    Context->>InputMgr: Create Input Manager
    activate InputMgr
    InputMgr-->>Context: Input Manager Handle
    deactivate InputMgr

    Context->>UIMgr: Create JzUIManager
    activate UIMgr
    UIMgr-->>Context: UI Handle
    deactivate UIMgr

    Context->>SceneMgr: Create JzSceneManager
    activate SceneMgr
    SceneMgr-->>Context: Scene Manager Handle
    deactivate SceneMgr

    Context-->>Engine: Success
    deactivate Context

    %% UI Initialization
    Engine->>Editor: Create Editor
    activate Editor

    Editor->>SceneMgr: Use JzSceneManager
    activate SceneMgr
    SceneMgr->>Scene: Load Default Scene
    activate Scene
    Scene-->>SceneMgr: Scene Reference
    deactivate Scene
    SceneMgr-->>Editor: Success
    deactivate  SceneMgr

    Editor->>PanelMgr: Create JzPanelsManager
    activate PanelMgr
    PanelMgr-->>Editor: JzPanelsManager Handle
    Editor->>PanelMgr: Use JzUIManager
    PanelMgr->>Panel: Load Panels
    activate Panel
    Panel->>Panel: Add Menu Bar
    Panel->>Panel: Add Views
    Panel->>Panel: Add Panel Windows
    deactivate Panel
    Panel-->>PanelMgr: Success
    PanelMgr-->>Editor: Success
    deactivate PanelMgr

    Editor-->>Engine: Success
    deactivate Editor

    %% Render Loop
    loop Render Loop
        Engine->>Editor: PreUpdate
        activate Editor
        Editor->>Context: Use JzContext
        activate Context
        Context->>Device: Poll Events
        activate Device
        Device-->>Context: Events
        deactivate Device
        Context-->>Editor: Success
        deactivate Context
        Editor-->>Engine: Success

        Engine->>Editor: Update
        Editor->>Context: Use JzContext
        activate Context
        Context->>InputMgr: Process Input
        activate InputMgr
        InputMgr-->>Context: Success
        deactivate InputMgr
        Context-->>Editor: Success
        deactivate Context

        Editor->>PanelMgr: Use JzPanelsManager
        activate PanelMgr
        PanelMgr-->>Editor: Success
        deactivate PanelMgr
        Editor->>Context: Use JzContext
        activate Context
        Context->>UIMgr: Update UI
        activate UIMgr
        UIMgr-->>Context: Success
        deactivate UIMgr
        Context-->>Editor: Success
        deactivate Context
        Editor-->>Engine: Success

        Engine->>Editor: PostUpdate
        Editor->>Context: Use JzContext
        activate Context
        Context->>Window: Swap Buffers
        activate Window
        Window-->>Context: Success
        deactivate Window
        Context->InputMgr: Clear Events
        activate InputMgr
        InputMgr-->>Context: Success
        deactivate InputMgr
        Context-->>Editor: Success
        deactivate Context
        Editor-->>Engine: Success

        deactivate Editor
    end
```
