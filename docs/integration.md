```
[主循环]
   |
   v
[RenderSystem::Update] -> 遍历实体 (Entity)
   |
   +--> [EntityManager] -> 获取组件 (Component)
           |
           +--> [MeshComponent] ----> 持有 std::shared_ptr<JzMesh>
           |
           +--> [MaterialComponent] -> 持有
std::shared_ptr<JzMaterial>
           |
           +--> [TransformComponent] -> (位置、旋转、缩放)
   |
   v
[JzMesh Resource] (CPU端)
   |
   +--> 顶点/索引数据 (CPU内存)
   |
   +--> 持有 std::shared_ptr<JzRHIBuffer> (顶点缓冲)
   |
   +--> 持有 std::shared_ptr<JzRHIBuffer> (索引缓冲)
   |
   v
[JzRHIDevice] (RHI/GPU抽象层)
   |
   +--> BindPipeline(material->GetPipeline())
   |
   +--> BindVertexArray(mesh->GetVertexArray())
   |
   +--> DrawIndexed(...)
   |
   v
[GPU] -> 渲染出图像
```

```cpp
// 1. Get the resource manager
auto& resourceManager = JzResourceManager::GetInstance();

// 2. Load a texture (e.g., an icon for the button)
// This returns a shared_ptr to the texture resource.
auto iconTexture =
resourceManager.GetResource<JzRHITexture>("path/to/your/icon.png");

// 3. Create the image button
if (iconTexture) {
    auto myImageButton = std::make_shared<JzImageButton>(iconTexture,
JzVec2{24.0f, 24.0f});

    // 4. Add the button to your UI and subscribe to its click event
    myImageButton->ClickedEvent.AddListener( () {
        // Handle button click...
    });
}
```
