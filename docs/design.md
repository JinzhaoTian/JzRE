# JzRE Design

## Class Diagram

```mermaid
classDiagram
    direction TB

    %% Interfaces
    class JzISerializable {
        <<Interface>>
        +Serialize(const String &filePath)
        +Deserialize(const String &filePath)
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
    class JzRenderEngine {
    }
    
    class JzContext {
    }
    
    class JzDevice {
    }
    
    class JzWindow {
    }
    
    class JzInputManager {
    }

    JzRenderEngine *-- JzContext
    JzContext *-- JzDevice
    JzContext *-- JzWindow
    JzContext *-- JzInputManager

    %% Editor Related Classes
    class JzEditor {
    }
    
    class JzCanvas {
    }
    
    class JzPanelsManager {
    }

    JzRenderEngine *-- JzEditor
    JzEditor *-- JzCanvas
    JzEditor *-- JzPanelsManager
    JzEditor ..> JzContext

    %% UI Related Classes
    class JzUIManager {
    }
    
    class JzWidgetContainer {
    }
    
    class JzPanel {
    }

    class JzWidget {
    }

    JzContext *-- JzUIManager
    JzUIManager ..> JzCanvas
    JzWidgetContainer *.. JzWidget

    %% UI PANEL Related Classes
    class JzPanelMenuBar {
    }

    class JzMenuBar {
    }

    class JzPanelTransformable { }

    class JzPanelWindow { }

    class JzAssetBrowser { }

    class JzView { }

    class JzViewControllable { }

    class JzAssetView { }

    class JzSceneView { }

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
    participant Editor as JzEditor
    participant Engine as JzRenderEngine
    participant Context as JzContext
    participant Device as JzDevice
    participant Window as JzWindow
    participant SceneMgr as JzSceneManager
    participant Scene as JzScene
    participant Panel as JzPanel

    %% Initialization
    Editor->>Engine: Initialize
    Engine->>Context: Create Context

    %% Device Initialization
    Context->>Device: Initialize Device
    Device-->>Context: Device Handle
    Context->>Window: Create Context
    Window-->>Context: Window Handle

    %% Scene Initialization
    Editor->>SceneMgr: Create JzSceneManager
    SceneMgr->>Scene: Load Default Scene
    activate Scene
    deactivate Scene
    SceneMgr-->>Editor: Scene Reference

    %% UI Initialization
    Editor->>Panel: Create Panel

    %% 主循环
    loop Render Loop
        Editor->>Window: Handle Input
        Editor->>Scene: Get Drawable Object
        Editor->>Panel: Draw()
        Editor->>Engine: Render Frame
        Engine->>Context: Commit GPU Command
    end
```
