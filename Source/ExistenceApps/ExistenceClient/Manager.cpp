/// Urho Related Header Files
#include "CoreEvents.h"
#include <Serializable.h>
#include  "Context.h"
#include "Object.h"
#include "Engine.h"
#include "Font.h"
#include "Input.h"
#include "ProcessUtils.h"
#include "Text.h"
#include "UI.h"
#include "Scene.h"
#include "Component.h"
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
#include "Str.h"
#include "Node.h"
#include <XMLElement.h>

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

using namespace std;


Manager::Manager(Context* context) :
    Object(context)
{
    scene_ = NULL;
}

Manager::~Manager()
{
    //dtor
}

/// Register Subsystem
void Manager::RegisterNewSubsystem(Context* context)
{
    context -> RegisterSubsystem(new Manager(context));


}

/// Set Scene
void Manager::SetScene(Scene *scene)
{
    /// point
    scene_ = scene;

    /// reserve
    ManagedNodes.Reserve(1024);
    ManagedGeneratedNodes.Reserve(1024);

    /// Clear Managed Nodes
    ManagedNodes.Clear();
    ManagedGeneratedNodes.Clear();



}

int Manager::AddGeneratedObject(Node * node)
{
    ManagedGeneratedNodes.Push(node);
}

/// Add Object
int Manager::AddObject(int type, const char * name, float x, float y, float z, const char *filename, bool physics)
{
    /// Get Needed SubSystems
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    int result=false;

    /// Check if a scene exist
    if(!scene_)
    {
        return 0;
    }


    String AddObjectFilename;


    /// Switch type for filename
    switch (type)
    {
    case MANAGESTATICMODEL:
    {
        /// First Check if .mdl is in the filename
        if(!string(filename).find(".mdl"))
        {
            return 0;
        }

        /// Create Urho3D equivalent of the filenames
        AddObjectFilename.Append(filesystem->GetProgramDir().CString());

        AddObjectFilename.Append("Resources/Models/");
        AddObjectFilename.Append(filename);

        /// If the model does not exist
        if(!filesystem->FileExists(AddObjectFilename.CString()))
        {
            return 0;
        }
    }

    break;
    default:
        return 0;


    }

    /// Switch type
    switch (type)
    {
    case MANAGESTATICMODEL:
    {
        /// check if static model exist

        /// create node
        Node * AddObjectNode = 	scene_ -> CreateChild (String(name));

        /// Setup node in scene
        AddObjectNode->SetPosition(Vector3(x,y,z));
        AddObjectNode->SetRotation(Quaternion(0.0f,0.0f,0.0f));
        AddObjectNode->SetScale(1);

        StaticModel* AddObjectNodeObject = AddObjectNode->CreateComponent<StaticModel>();

        /// Addd Static Model component
        AddObjectNodeObject->SetModel(cache->GetResource<Model>(AddObjectFilename));

        /// Create material
        String AddObjectFilenameMaterial;
        AddObjectFilenameMaterial = AddObjectFilename.Replaced (String(".mdl"), String(".txt"), true);

        AddObjectNodeObject->ApplyMaterialList( AddObjectFilenameMaterial.CString());

        ///AddObjectNodeObject->SetMaterial(cache->GetResource<Material>(AddObjectFilenameMaterial));
        /// Add Physics
        if(physics)
        {
            /// Create rigidbody, and set non-zero mass so that the body becomes dynamic
            RigidBody* AddObjectNodeRigid =  AddObjectNode->CreateComponent<RigidBody>();
            AddObjectNodeRigid->SetCollisionLayer(1);

            AddObjectNodeRigid->SetMass(0);

            /// Get static model and bounding box, calculate offset
            BoundingBox  AddObjectNodeObjectBounding = AddObjectNodeObject->GetBoundingBox();

            Vector3 BoundBoxCenter = AddObjectNodeObjectBounding.Center();

            /// Set zero angular factor so that physics doesn't turn the character on its own.
            /// Instead we will control the character yaw manually
            AddObjectNodeRigid->SetAngularFactor(Vector3::ZERO);

            /// Set the rigidbody to signal collision also when in rest, so that we get ground collisions properly
            AddObjectNodeRigid->SetCollisionEventMode(COLLISION_ALWAYS);

            /// Set a capsule shape for collision
            CollisionShape* AddObjectNodeCollisionShape = AddObjectNode->CreateComponent<CollisionShape>();

            /// Set shape collision
            AddObjectNodeCollisionShape->SetBox(Vector3::ONE);
            AddObjectNodeCollisionShape->SetPosition(Vector3(BoundBoxCenter));
            AddObjectNodeCollisionShape->SetLodLevel(1);
        }

        /// Add a component
        GameObject * AddObjectNodeGameComponent = AddObjectNode -> CreateComponent<GameObject>();

        AddObjectNodeGameComponent->SetLifetime(-1.0f);

        /// push
        ManagedNodes.Push(AddObjectNode);

        result=true;
    }
    break;

    case MANAGELIGHT:
    {
        /// Create a directional light to the world. Enable cascaded shadows on it
        Node* AddObjectNode = scene_->CreateChild(name);
        Light* AddObjectNodeLight = AddObjectNode->CreateComponent<Light>();
        AddObjectNodeLight->SetLightType(LIGHT_DIRECTIONAL);
        AddObjectNodeLight->SetCastShadows(true);
        AddObjectNodeLight->SetSpecularIntensity(.3f);
        AddObjectNodeLight->SetBrightness(.6);
        AddObjectNodeLight->SetColor(Color(1.0f, 1.0f,.95f));

        /// Setup node in scene
        AddObjectNode->SetPosition(Vector3(x,y,z));
        AddObjectNode->SetRotation(Quaternion(0.0f,0.0f,0.0f));
        AddObjectNode->SetScale(1);

        /// Add a component
        GameObject * AddObjectNodeGameComponent = AddObjectNode -> CreateComponent<GameObject>();

        AddObjectNodeGameComponent->SetLifetime(-1.0f);


        /// Use filename to change type
        if(filename=="directional")
        {
            AddObjectNodeLight->SetLightType(LIGHT_DIRECTIONAL);
        }

        if(filename=="spot")
        {
            AddObjectNodeLight->SetLightType(LIGHT_SPOT);
        }

        if(filename=="point")
        {
            AddObjectNodeLight->SetLightType(LIGHT_POINT);
        }


        /// push
        ManagedNodes.Push(AddObjectNode);
    }
    break;

    default:
        break;
    }

    return result;
}

int Manager::SaveManagedNodes(const char *filename)
{
    /// Grab resources
    ResourceCache * cache = GetSubsystem<ResourceCache>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();

    /// Check if scene exist
    if(scene_==NULL)
    {
        return 0;
    }

    /// Check if Node is empty
    if (ManagedNodes.Empty())
    {
        return 0;
    }

    /// Force .xml on save
    if(!string(filename).find(".xml"))
    {
        return 0;
    }

    /// Create String
    String savesceneexport;

    /// Set directory
    savesceneexport.Append(filesystem->GetProgramDir().CString());
    savesceneexport.Append("Resources/ScenesData/");
    savesceneexport.Append(filename);

    savesceneexport = savesceneexport.Replaced(String(".xml"),String("diff.xml"),true);

    File saveFile(context_, savesceneexport.CString(), FILE_WRITE);

    XMLFile * savesceneexportxml= new XMLFile(context_);

    XMLElement xmfileelement = savesceneexportxml-> CreateRoot("scene");

    /// Loop through components
    for(Vector<Node *>:: Iterator it = ManagedNodes.Begin(); it != ManagedNodes.End(); ++it)
    {
        XMLElement xmfileelementnode = xmfileelement.CreateChild("node");
        (*it) -> SaveXML(xmfileelementnode);
    }

    savesceneexportxml->Save(saveFile);

    return 1;
}


int Manager::SaveManagedGeneratedNodes(const char *filename)
{
    /// Grab resources
    ResourceCache * cache = GetSubsystem<ResourceCache>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();

    /// Check if scene exist
    if(scene_==NULL)
    {
        return 0;
    }

    /// Check if Node is empty
    if (ManagedGeneratedNodes.Empty())
    {
        return 0;
    }

    /// Force .xml on save
    if(!string(filename).find(".xml"))
    {
        return 0;
    }

    /// Create String
    String savescenegenerateexport;

    /// Set directory
    savescenegenerateexport.Append(filesystem->GetProgramDir().CString());

    savescenegenerateexport.Append("Resources/ScenesData/");

    savescenegenerateexport.Append(filename);

    savescenegenerateexport = savescenegenerateexport.Replaced(String(".xml"),String("gene.xml"),true);

    File saveFile(context_, savescenegenerateexport.CString(), FILE_WRITE);

    XMLFile * savescenegenerateexportxml= new XMLFile(context_);

    XMLElement xmfileelement = savescenegenerateexportxml-> CreateRoot("scene");

    /// Loop through components
    for(Vector<Node *>:: Iterator it = ManagedGeneratedNodes.Begin(); it != ManagedGeneratedNodes.End(); ++it)
    {
        XMLElement xmfileelementnode = xmfileelement.CreateChild("node");
        (*it) -> SaveXML(xmfileelementnode);
    }

    savescenegenerateexportxml->Save(saveFile);

    return 1;
}


/// Load account information from a account file
int Manager::LoadManagedNodes(const char *filename)
{

    /// Grab resources
    ResourceCache * cache = GetSubsystem<ResourceCache>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();

    /// Check if scene exist
    if(scene_==NULL)
    {
        return 1;
    }

    /// Force .xml on Load
    if(!string(filename).find(".xml"))
    {
        return 0;
    }

    /// Create String
    String Loadsceneexport;

    /// Set directory
    Loadsceneexport.Append(filesystem->GetProgramDir().CString());
    Loadsceneexport.Append("Resources/ScenesData/");
    Loadsceneexport.Append(filename);

    /// Check if the account file information exist
    if(!filesystem->FileExists(Loadsceneexport.CString()))
    {
        return 0;
    }

    XMLFile * loadingfile = new XMLFile(context_);

    /// Create a file in current context
    File LoadFile(context_, Loadsceneexport.CString(), FILE_READ);

    loadingfile->Load(LoadFile);

    XMLElement nextElement = loadingfile->GetRoot("scene");

    do
    {
        /// Create pointer
        Node * newNode;

        /// get node attributes
        XMLElement AttributesPosition;

        /// Is enabled
        AttributesPosition = nextElement.GetChild ("attribute");

        /// name
        AttributesPosition = AttributesPosition.GetNext ("attribute");

        /// position
        AttributesPosition = AttributesPosition.GetNext ("attribute");
        Vector3 NodePosition= AttributesPosition .GetVector3("value");

        /// rotation
        AttributesPosition = AttributesPosition.GetNext ("attribute");
        Quaternion NodeRotation =  AttributesPosition.GetQuaternion ("value");


        float nodeScale = nextElement.GetFloat("Scale");

        cout << "position" << NodePosition.ToString().CString();
        newNode=scene_->InstantiateXML (nextElement,(const Vector3 &)Vector3(NodePosition),(const Quaternion &)Quaternion(NodeRotation),REPLICATED);

        cout << nextElement.GetName().CString() << endl;

        /// push
        ManagedNodes.Push(newNode);

        nextElement=nextElement.GetNext();

    }
    while(nextElement.NotNull());

    return 1;
}


