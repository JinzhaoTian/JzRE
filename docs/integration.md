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
