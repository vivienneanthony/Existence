// Urho3D editor scene handling

#include "Scripts/Editor/EditorHierarchyWindow.as"
#include "Scripts/Editor/EditorInspectorWindow.as"

const int PICK_GEOMETRIES = 0;
const int PICK_LIGHTS = 1;
const int PICK_ZONES = 2;
const int PICK_RIGIDBODIES = 3;
const int PICK_UI_ELEMENTS = 4;
const int MAX_PICK_MODES = 5;
const int MAX_UNDOSTACK_SIZE = 256;

Scene@ editorScene;

String instantiateFileName;
CreateMode instantiateMode = REPLICATED;
bool sceneModified = false;
bool runUpdate = false;

Array<Node@> selectedNodes;
Array<Component@> selectedComponents;
Node@ editNode;
Array<Node@> editNodes;
Array<Component@> editComponents;
uint numEditableComponentsPerNode = 1;

Array<XMLFile@> sceneCopyBuffer;

bool suppressSceneChanges = false;
bool inSelectionModify = false;
bool skipMruScene = false;

Array<EditActionGroup> undoStack;
uint undoStackPos = 0;

bool revertOnPause = false;
XMLFile@ revertData;

void ClearSceneSelection()
{
    selectedNodes.Clear();
    selectedComponents.Clear();
    editNode = null;
    editNodes.Clear();
    editComponents.Clear();
    numEditableComponentsPerNode = 1;

    HideGizmo();
}

void CreateScene()
{
    // Create a scene only once here
    editorScene = Scene();

    // Allow access to the scene from the console
    script.defaultScene = editorScene;

    // Always pause the scene, and do updates manually
    editorScene.updateEnabled = false;
}

bool ResetScene()
{
    ui.cursor.shape = CS_BUSY;

    if (messageBoxCallback is null && sceneModified)
    {
        MessageBox@ messageBox = MessageBox("Scene has been modified.\nContinue to reset?", "Warning");
        if (messageBox.window !is null)
        {
            Button@ cancelButton = messageBox.window.GetChild("CancelButton", true);
            cancelButton.visible = true;
            cancelButton.focus = true;
            SubscribeToEvent(messageBox, "MessageACK", "HandleMessageAcknowledgement");
            messageBoxCallback = @ResetScene;
            return false;
        }
    }
    else
        messageBoxCallback = null;

    suppressSceneChanges = true;

    // Create a scene with default values, these will be overridden when loading scenes
    editorScene.Clear();
    editorScene.CreateComponent("Octree");
    editorScene.CreateComponent("DebugRenderer");

    // Release resources that became unused after the scene clear
    cache.ReleaseAllResources(false);

    sceneModified = false;
    revertData = null;
    StopSceneUpdate();

    UpdateWindowTitle();
    UpdateHierarchyItem(editorScene, true);
    ClearEditActions();

    suppressSceneChanges = false;

    ResetCamera();
    CreateGizmo();
    CreateGrid();
    SetActiveViewport(viewports[0]);

    return true;
}

void SetResourcePath(String newPath, bool usePreferredDir = true, bool additive = false)
{
    if (newPath.empty)
        return;

    if (usePreferredDir)
        newPath = AddTrailingSlash(cache.GetPreferredResourceDir(newPath));
    else
        newPath = AddTrailingSlash(newPath);

    if (newPath == sceneResourcePath)
        return;

    // Remove the old scene resource path if any. However make sure that the default data paths do not get removed
    if (!additive)
    {
        cache.ReleaseAllResources(false);
        renderer.ReloadShaders();

        String check = AddTrailingSlash(sceneResourcePath);
        bool isDefaultResourcePath = check.Compare(fileSystem.programDir + "Data/", false) == 0 ||
            check.Compare(fileSystem.programDir + "CoreData/", false) == 0;

        if (!sceneResourcePath.empty && !isDefaultResourcePath)
            cache.RemoveResourceDir(sceneResourcePath);
    }

    cache.AddResourceDir(newPath);

    if (!additive)
    {
        sceneResourcePath = newPath;
        uiScenePath = GetResourceSubPath(newPath, "Scenes");
        uiElementPath = GetResourceSubPath(newPath, "UI");
        uiNodePath = GetResourceSubPath(newPath, "Objects");
        uiScriptPath = GetResourceSubPath(newPath, "Scripts");
        uiParticlePath = GetResourceSubPath(newPath, "Particle");
    }
}

String GetResourceSubPath(String basePath, const String&in subPath)
{
    basePath = AddTrailingSlash(basePath);
    if (fileSystem.DirExists(basePath + subPath))
        return AddTrailingSlash(basePath + subPath);
    else
        return basePath;
}

bool LoadScene(const String&in fileName)
{
    if (fileName.empty)
        return false;

    ui.cursor.shape = CS_BUSY;

    // Always load the scene from the filesystem, not from resource paths
    if (!fileSystem.FileExists(fileName))
    {
        MessageBox("No such scene.\n" + fileName);
        return false;
    }

    File file(fileName, FILE_READ);
    if (!file.open)
    {
        MessageBox("Could not open file.\n" + fileName);
        return false;
    }

    // Add the scene's resource path in case it's necessary
    String newScenePath = GetPath(fileName);
    if (!rememberResourcePath || !sceneResourcePath.StartsWith(newScenePath, false))
        SetResourcePath(newScenePath);

    suppressSceneChanges = true;
    sceneModified = false;
    revertData = null;
    StopSceneUpdate();

    String extension = GetExtension(fileName);
    bool loaded;
    if (extension != ".xml")
        loaded = editorScene.Load(file);
    else
        loaded = editorScene.LoadXML(file);

    // Release resources which are not used by the new scene
    cache.ReleaseAllResources(false);

    // Always pause the scene, and do updates manually
    editorScene.updateEnabled = false;

    UpdateWindowTitle();
    UpdateHierarchyItem(editorScene, true);
    ClearEditActions();

    suppressSceneChanges = false;

    // global variable to mostly bypass adding mru upon importing tempscene
    if (!skipMruScene)
        UpdateSceneMru(fileName);

    skipMruScene = false;

    ResetCamera();
    CreateGizmo();
    CreateGrid();
    SetActiveViewport(viewports[0]);

    return loaded;
}

bool SaveScene(const String&in fileName)
{
    if (fileName.empty)
        return false;

    ui.cursor.shape = CS_BUSY;

    // Unpause when saving so that the scene will work properly when loaded outside the editor
    editorScene.updateEnabled = true;

    File file(fileName, FILE_WRITE);
    String extension = GetExtension(fileName);
    bool success = (extension != ".xml" ? editorScene.Save(file) : editorScene.SaveXML(file));

    editorScene.updateEnabled = false;

    if (success)
    {
        UpdateSceneMru(fileName);
        sceneModified = false;
        UpdateWindowTitle();
    }
    else
        MessageBox("Could not save scene successfully!\nSee Urho3D.log for more detail.");

    return success;
}

bool SaveSceneWithExistingName()
{
    if (editorScene.fileName.empty || editorScene.fileName == TEMP_SCENE_NAME)
        return PickFile();
    else
        return SaveScene(editorScene.fileName);
}

void CreateNode(CreateMode mode)
{
    Node@ newNode = editorScene.CreateChild("", mode);
    // Set the new node a certain distance from the camera
    newNode.position = GetNewNodePosition();

    // Create an undo action for the create
    CreateNodeAction action;
    action.Define(newNode);
    SaveEditAction(action);
    SetSceneModified();

    FocusNode(newNode);
}

void CreateComponent(const String&in componentType)
{
    // If this is the root node, do not allow to create duplicate scene-global components
    if (editNode is editorScene && CheckForExistingGlobalComponent(editNode, componentType))
        return;

    // Group for storing undo actions
    EditActionGroup group;

    // For now, make a local node's all components local
    /// \todo Allow to specify the createmode
    for (uint i = 0; i < editNodes.length; ++i)
    {
        Component@ newComponent = editNodes[i].CreateComponent(componentType, editNodes[i].id < FIRST_LOCAL_ID ? REPLICATED : LOCAL);
        if (newComponent !is null)
        {
            // Some components such as CollisionShape do not create their internal object before the first call to ApplyAttributes()
            // to prevent unnecessary initialization with default values. Call now
            newComponent.ApplyAttributes();

            CreateComponentAction action;
            action.Define(newComponent);
            group.actions.Push(action);
        }
    }

    SaveEditActionGroup(group);
    SetSceneModified();

    // Although the edit nodes selection are not changed, call to ensure attribute inspector notices new components of the edit nodes
    HandleHierarchyListSelectionChange();
}

void LoadNode(const String&in fileName)
{
    if (fileName.empty)
        return;

    if (!fileSystem.FileExists(fileName))
    {
        MessageBox("No such node file.\n" + fileName);
        return;
    }

    File file(fileName, FILE_READ);
    if (!file.open)
    {
        MessageBox("Could not open file.\n" + fileName);
        return;
    }

    ui.cursor.shape = CS_BUSY;

    // Before instantiating, add object's resource path if necessary
    SetResourcePath(GetPath(fileName), true, true);

    Ray cameraRay = camera.GetScreenRay(0.5, 0.5); // Get ray at view center
    Vector3 position, normal;
    GetSpawnPosition(cameraRay, newNodeDistance, position, normal, 0, true);

    Node@ newNode = InstantiateNodeFromFile(file, position, Quaternion(), 1, instantiateMode);
    if (newNode !is null)
    {
        FocusNode(newNode);
        instantiateFileName = fileName;
    }
}

Node@ InstantiateNodeFromFile(File@ file, const Vector3& position, const Quaternion& rotation, float scaleMod = 1.0f, CreateMode mode = REPLICATED)
{
    if (file is null)
        return null;

    Node@ newNode;
    uint numSceneComponent = editorScene.numComponents;

    suppressSceneChanges = true;

    String extension = GetExtension(file.name);
    if (extension != ".xml")
        newNode = editorScene.Instantiate(file, position, rotation, mode);
    else
        newNode = editorScene.InstantiateXML(file, position, rotation, mode);

    suppressSceneChanges = false;

    if (newNode !is null)
    {
        newNode.scale = newNode.scale * scaleMod;
        if (alignToAABBBottom)
        {
            Drawable@ drawable = GetFirstDrawable(newNode);
            if (drawable !is null)
            {
                BoundingBox aabb = drawable.worldBoundingBox;
                Vector3 aabbBottomCenter(aabb.center.x, aabb.min.y, aabb.center.z);
                Vector3 offset = aabbBottomCenter - newNode.worldPosition;
                newNode.worldPosition = newNode.worldPosition - offset;
            }
        }

        // Create an undo action for the load
        CreateNodeAction action;
        action.Define(newNode);
        SaveEditAction(action);
        SetSceneModified();

        if (numSceneComponent != editorScene.numComponents)
            UpdateHierarchyItem(editorScene);
        else
            UpdateHierarchyItem(newNode);
    }

    return newNode;
}

bool SaveNode(const String&in fileName)
{
    if (fileName.empty)
        return false;

    ui.cursor.shape = CS_BUSY;

    File file(fileName, FILE_WRITE);
    if (!file.open)
    {
        MessageBox("Could not open file.\n" + fileName);
        return false;
    }

    String extension = GetExtension(fileName);
    bool success = (extension != ".xml" ? editNode.Save(file) : editNode.SaveXML(file));
    if (success)
        instantiateFileName = fileName;
    else
        MessageBox("Could not save node successfully!\nSee Urho3D.log for more detail.");

    return success;
}

void UpdateScene(float timeStep)
{
    if (runUpdate)
        editorScene.Update(timeStep);
}

void StopSceneUpdate()
{
    runUpdate = false;
    audio.Stop();
    toolBarDirty = true;

    // If scene should revert on update stop, load saved data now
    if (revertOnPause && revertData !is null)
    {
        suppressSceneChanges = true;
        editorScene.Clear();
        editorScene.LoadXML(revertData.GetRoot());
        UpdateHierarchyItem(editorScene, true);
        ClearEditActions();
        suppressSceneChanges = false;
    }

    revertData = null;
}

void StartSceneUpdate()
{
    runUpdate = true;
    // Run audio playback only when scene is updating, so that audio components' time-dependent attributes stay constant when
    // paused (similar to physics)
    audio.Play();
    toolBarDirty = true;

    // Save scene data for reverting if enabled
    if (revertOnPause)
    {
        revertData = XMLFile();
        XMLElement root = revertData.CreateRoot("scene");
        editorScene.SaveXML(root);
    }
    else
        revertData = null;
}

bool ToggleSceneUpdate()
{
    if (!runUpdate)
        StartSceneUpdate();
    else
        StopSceneUpdate();
    return true;
}

void SetSceneModified()
{
    if (!sceneModified)
    {
        sceneModified = true;
        UpdateWindowTitle();
    }
}

bool SceneDelete()
{
    ui.cursor.shape = CS_BUSY;

    BeginSelectionModify();

    // Clear the selection now to prevent repopulation of selectedNodes and selectedComponents combo
    hierarchyList.ClearSelection();

    // Group for storing undo actions
    EditActionGroup group;

    // Remove nodes
    for (uint i = 0; i < selectedNodes.length; ++i)
    {
        Node@ node = selectedNodes[i];
        if (node.parent is null || node.scene is null)
            continue; // Root or already deleted

        uint nodeIndex = GetListIndex(node);

        // Create undo action
        DeleteNodeAction action;
        action.Define(node);
        group.actions.Push(action);

        node.Remove();
        SetSceneModified();

        // If deleting only one node, select the next item in the same index
        if (selectedNodes.length == 1 && selectedComponents.empty)
            hierarchyList.selection = nodeIndex;
    }

    // Then remove components, if they still remain
    for (uint i = 0; i < selectedComponents.length; ++i)
    {
        Component@ component = selectedComponents[i];
        Node@ node = component.node;
        if (node is null)
            continue; // Already deleted

        uint index = GetComponentListIndex(component);
        uint nodeIndex = GetListIndex(node);
        if (index == NO_ITEM || nodeIndex == NO_ITEM)
            continue;

        // Do not allow to remove the Octree, DebugRenderer or MaterialCache2D or DrawableProxy2D from the root node
        if (node is editorScene && (component.typeName == "Octree" || component.typeName == "DebugRenderer" ||
            component.typeName == "MaterialCache2D" || component.typeName == "DrawableProxy2D"))
            continue;

        // Create undo action
        DeleteComponentAction action;
        action.Define(component);
        group.actions.Push(action);

        node.RemoveComponent(component);
        SetSceneModified();

        // If deleting only one component, select the next item in the same index
        if (selectedComponents.length == 1 && selectedNodes.empty)
            hierarchyList.selection = index;
    }

    SaveEditActionGroup(group);

    EndSelectionModify();
    return true;
}

bool SceneCut()
{
    return SceneCopy() && SceneDelete();
}

bool SceneCopy()
{
    ui.cursor.shape = CS_BUSY;

    sceneCopyBuffer.Clear();

    // Copy components
    if (!selectedComponents.empty)
    {
        for (uint i = 0; i < selectedComponents.length; ++i)
        {
            XMLFile@ xml = XMLFile();
            XMLElement rootElem = xml.CreateRoot("component");
            selectedComponents[i].SaveXML(rootElem);
            rootElem.SetBool("local", selectedComponents[i].id >= FIRST_LOCAL_ID);
            sceneCopyBuffer.Push(xml);
        }
    }
    // Copy nodes.
    else
    {
        for (uint i = 0; i < selectedNodes.length; ++i)
        {
            // Skip the root scene node as it cannot be copied
            if (selectedNodes[i] is editorScene)
                continue;

            XMLFile@ xml = XMLFile();
            XMLElement rootElem = xml.CreateRoot("node");
            selectedNodes[i].SaveXML(rootElem);
            rootElem.SetBool("local", selectedNodes[i].id >= FIRST_LOCAL_ID);
            sceneCopyBuffer.Push(xml);
        }
    }

    return true;
}

bool ScenePaste()
{
    ui.cursor.shape = CS_BUSY;

    // Group for storing undo actions
    EditActionGroup group;

    for (uint i = 0; i < sceneCopyBuffer.length; ++i)
    {
        XMLElement rootElem = sceneCopyBuffer[i].root;
        String mode = rootElem.name;
        if (mode == "component")
        {
            // If this is the root node, do not allow to create duplicate scene-global components
            if (editNode is editorScene && CheckForExistingGlobalComponent(editNode, rootElem.GetAttribute("type")))
                return false;

            // If copied component was local, make the new local too
            Component@ newComponent = editNode.CreateComponent(rootElem.GetAttribute("type"), rootElem.GetBool("local") ? LOCAL :
                REPLICATED);
            if (newComponent is null)
                return false;

            newComponent.LoadXML(rootElem);
            newComponent.ApplyAttributes();

            // Create an undo action
            CreateComponentAction action;
            action.Define(newComponent);
            group.actions.Push(action);
        }
        else if (mode == "node")
        {
            // Make the paste go always to the root node, no matter of the selected node
            // If copied node was local, make the new local too
            Node@ newNode = editorScene.CreateChild("", rootElem.GetBool("local") ? LOCAL : REPLICATED);
            newNode.LoadXML(rootElem);

            // Create an undo action
            CreateNodeAction action;
            action.Define(newNode);
            group.actions.Push(action);
        }
    }

    SaveEditActionGroup(group);
    SetSceneModified();
    return true;
}

bool SceneUnparent()
{
    if (!CheckHierarchyWindowFocus() || !selectedComponents.empty || selectedNodes.empty)
        return false;

    ui.cursor.shape = CS_BUSY;

    // Group for storing undo actions
    EditActionGroup group;

    // Parent selected nodes to root
    Array<Node@> changedNodes;
    for (uint i = 0; i < selectedNodes.length; ++i)
    {
        Node@ sourceNode = selectedNodes[i];
        if (sourceNode.parent is null || sourceNode.parent is editorScene)
            continue; // Root or already parented to root

        // Perform the reparenting, continue loop even if action fails
        ReparentNodeAction action;
        action.Define(sourceNode, editorScene);
        group.actions.Push(action);

        SceneChangeParent(sourceNode, editorScene, false);
        changedNodes.Push(sourceNode);
    }

    // Reselect the changed nodes at their new position in the list
    for (uint i = 0; i < changedNodes.length; ++i)
        hierarchyList.AddSelection(GetListIndex(changedNodes[i]));

    SaveEditActionGroup(group);
    SetSceneModified();

    return true;
}

bool SceneToggleEnable()
{
    if (!CheckHierarchyWindowFocus())
        return false;

    ui.cursor.shape = CS_BUSY;

    EditActionGroup group;

    // Toggle enabled state of nodes recursively
    for (uint i = 0; i < selectedNodes.length; ++i)
    {
        // Do not attempt to disable the Scene
        if (selectedNodes[i].typeName == "Node")
        {
            bool oldEnabled = selectedNodes[i].enabled;
            selectedNodes[i].SetEnabled(!oldEnabled, true);

            // Create undo action
            ToggleNodeEnabledAction action;
            action.Define(selectedNodes[i], oldEnabled);
            group.actions.Push(action);
        }
    }
    for (uint i = 0; i < selectedComponents.length; ++i)
    {
        // Some components purposefully do not expose the Enabled attribute, and it does not affect them in any way
        // (Octree, PhysicsWorld). Check that the first attribute is in fact called "Is Enabled"
        if (selectedComponents[i].numAttributes > 0 && selectedComponents[i].attributeInfos[0].name == "Is Enabled")
        {
            bool oldEnabled = selectedComponents[i].enabled;
            selectedComponents[i].enabled = !oldEnabled;

            // Create undo action
            EditAttributeAction action;
            action.Define(selectedComponents[i], 0, Variant(oldEnabled));
            group.actions.Push(action);
        }
    }

    SaveEditActionGroup(group);
    SetSceneModified();

    return true;
}

bool SceneChangeParent(Node@ sourceNode, Node@ targetNode, bool createUndoAction = true)
{
    // Create undo action if requested
    if (createUndoAction)
    {
        ReparentNodeAction action;
        action.Define(sourceNode, targetNode);
        SaveEditAction(action);
    }

    sourceNode.parent = targetNode;
    SetSceneModified();

    // Return true if success
    if (sourceNode.parent is targetNode)
    {
        UpdateNodeAttributes(); // Parent change may have changed local transform
        return true;
    }
    else
        return false;
}

bool SceneChangeParent(Node@ sourceNode, Array<Node@> sourceNodes, Node@ targetNode, bool createUndoAction = true)
{
    // Create undo action if requested
    if (createUndoAction)
    {
        ReparentNodeAction action;
        action.Define(sourceNodes, targetNode);
        SaveEditAction(action);
    }

    for (uint i = 0; i < sourceNodes.length; i++)
    {
        Node@ node = sourceNodes[i];
        node.parent = targetNode;
    }
    SetSceneModified();

    // Return true if success
    if (sourceNode.parent is targetNode)
    {
        UpdateNodeAttributes(); // Parent change may have changed local transform
        return true;
    }
    else
        return false;
}

bool SceneResetPosition()
{
    if (editNode !is null)
    {
        Transform oldTransform;
        oldTransform.Define(editNode);

        editNode.position = Vector3(0.0, 0.0, 0.0);

        // Create undo action
        EditNodeTransformAction action;
        action.Define(editNode, oldTransform);
        SaveEditAction(action);
        SetSceneModified();

        UpdateNodeAttributes();
        return true;
    }
    else
        return false;
}

bool SceneResetRotation()
{
    if (editNode !is null)
    {
        Transform oldTransform;
        oldTransform.Define(editNode);

        editNode.rotation = Quaternion();

        // Create undo action
        EditNodeTransformAction action;
        action.Define(editNode, oldTransform);
        SaveEditAction(action);
        SetSceneModified();

        UpdateNodeAttributes();
        return true;
    }
    else
        return false;
}

bool SceneResetScale()
{
    if (editNode !is null)
    {
        Transform oldTransform;
        oldTransform.Define(editNode);

        editNode.scale = Vector3(1.0, 1.0, 1.0);

        // Create undo action
        EditNodeTransformAction action;
        action.Define(editNode, oldTransform);
        SaveEditAction(action);
        SetSceneModified();

        UpdateNodeAttributes();
        return true;
    }
    else
        return false;
}

bool SceneSelectAll()
{
    BeginSelectionModify();
    Array<Node@> rootLevelNodes = editorScene.GetChildren();
    Array<uint> indices;
    for (uint i = 0; i < rootLevelNodes.length; ++i)
        indices.Push(GetListIndex(rootLevelNodes[i]));
    hierarchyList.SetSelections(indices);
    EndSelectionModify();

    return true;
}

bool SceneResetToDefault()
{
    ui.cursor.shape = CS_BUSY;

    // Group for storing undo actions
    EditActionGroup group;

    // Reset selected component to their default
    if (!selectedComponents.empty)
    {
        for (uint i = 0; i < selectedComponents.length; ++i)
        {
            Component@ component = selectedComponents[i];

            ResetAttributesAction action;
            action.Define(component);
            group.actions.Push(action);

            component.ResetToDefault();
            component.ApplyAttributes();
            for (uint j = 0; j < component.numAttributes; ++j)
                PostEditAttribute(component, j);
        }
    }
    // OR reset selected nodes to their default
    else
    {
        for (uint i = 0; i < selectedNodes.length; ++i)
        {
            Node@ node = selectedNodes[i];

            ResetAttributesAction action;
            action.Define(node);
            group.actions.Push(action);

            node.ResetToDefault();
            node.ApplyAttributes();
            for (uint j = 0; j < node.numAttributes; ++j)
                PostEditAttribute(node, j);
        }
    }

    SaveEditActionGroup(group);
    SetSceneModified();
    attributesFullDirty = true;

    return true;
}

bool SceneRebuildNavigation()
{
    ui.cursor.shape = CS_BUSY;

    Array<Component@>@ navMeshes = editorScene.GetComponents("NavigationMesh", true);
    if (navMeshes.empty)
    {
        MessageBox("No NavigationMesh components in the scene, nothing to rebuild.");
        return false;
    }

    bool success = true;
    for (uint i = 0; i < navMeshes.length; ++i)
    {
        NavigationMesh@ navMesh = navMeshes[i];
        if (!navMesh.Build())
            success = false;
    }

    return success;
}

bool LoadParticleData(const String&in fileName)
{
    if (fileName.empty)
        return false;

    XMLFile xmlFile;
    if (!xmlFile.Load(File(fileName, FILE_READ)))
        return false;

    bool needRefresh = false;

    for (uint i = 0; i < editComponents.length; ++i)
    {
        ParticleEmitter@ emitter = cast<ParticleEmitter>(editComponents[i]);
        if (emitter !is null)
        {
            emitter.Load(xmlFile);
            needRefresh = true;
        }
    }
    
    if (needRefresh)
        UpdateAttributeInspector();

    return true;
}

bool SaveParticleData(const String&in fileName)
{
    if (fileName.empty || editComponents.length != 1)
        return false;

    ParticleEmitter@ emitter = cast<ParticleEmitter>(editComponents[0]);
    if (emitter !is null)
    {
        XMLFile xmlFile;
        emitter.Save(xmlFile);
        return xmlFile.Save(File(fileName, FILE_WRITE));
    }

    return false;
}

void UpdateSceneMru(String filename)
{
    while (uiRecentScenes.Find(filename) > -1)
        uiRecentScenes.Erase(uiRecentScenes.Find(filename));

    uiRecentScenes.Insert(0, filename);

    for (uint i = uiRecentScenes.length - 1; i >= maxRecentSceneCount; i--)
        uiRecentScenes.Erase(i);

    PopulateMruScenes();
}

Drawable@ GetFirstDrawable(Node@ node)
{
    Array<Node@> nodes = node.GetChildren(true);
    nodes.Insert(0, node);

    for (uint i = 0; i < nodes.length; ++i)
    {
        Array<Component@> components = nodes[i].GetComponents();
        for (uint j = 0; j < components.length; ++j)
        {
            Drawable@ drawable = cast<Drawable>(components[j]);
            if (drawable !is null)
                return drawable;
        }
    }
    
    return null;
}
