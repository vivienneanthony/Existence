/// Urho Related Header Files
#include "CoreEvents.h"
#include "Engine.h"
#include "Font.h"
#include "Input.h"
#include "ProcessUtils.h"
#include "Text.h"
#include "UI.h"
#include "Scene.h"
#include "StaticModel.h"
#include "Octree.h"
#include "Model.h"
#include "Material.h"
#include "Camera.h"
#include "ResourceCache.h"
#include "Renderer.h"
#include "Camera.h"
#include "Window.h"
#include "Button.h"
#include "LineEdit.h"
#include "UIElement.h"
#include "BoundingBox.h"
#include "UIEvents.h"
#include "DebugRenderer.h"
#include "File.h"
#include "FileSystem.h"
#include "XMLFile.h"
#include "XMLElement.h"
#include "Deserializer.h"
#include "Cursor.h"
#include "FileSystem.h"
#include "ListView.h"
#include "Console.h"
#include "RigidBody.h"
#include "CollisionShape.h"
#include "PhysicsWorld.h"
#include "Animation.h"
#include "AnimatedModel.h"
#include "AnimationController.h"
#include "Character.h"
#include "Terrain.h"
#include "EngineEvents.h"
#include "Zone.h"
#include "Log.h"
#include "Skybox.h"
#include "Sprite.h"
#include "StaticModelGroup.h"
#include "BillboardSet.h"
#include "Random.h"
#include "RenderPath.h"
#include "Color.h"
#include "Graphics.h"

///C/C++ related header files
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <locale>
#include <ctime>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <utility>
#include <algorithm>

/// Existence Header files
#include "GameStateHandler.h"
#include "Account.h"
#include "GameObject.h"
#include "WorldBuild.h"
#include "Manager.h"

#include "../../Engine/Procedural/RandomNumberGenerator.h"

#include "DebugNew.h"

Manager::Manager(Context* context) :
    Object(context)
{
    scene_=NULL;
}

Manager::~Manager()
{
    //dtor
}

int Manager::SetScene(Scene* scene)
{
    scene_=scene;
}

int Manager::AddObject(int type, const char * name, float x, float y, float z, const char *filename)
{
    /// Get Needed SubSystems
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();

    /// if no scene return 0
    if(!scene_)
    {
        return 0;
    }

    /// Get scene and terrain node
    Node* terrainNode = scene_->GetChild("GeneratedTerrainRule_Terrain",true);
    Terrain * terrain = terrainNode -> GetComponent<Terrain>();

    Vector3 terrainposition = terrainNode ->GetPosition();
    IntVector2 terrainsize = terrain->GetNumVertices();

    cout << terrainsize.ToString().CString() <<endl;
    cout << "test" << endl;

    return 1;
}

