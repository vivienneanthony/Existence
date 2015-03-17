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

}

/// Add Object
int Manager::AddObject(int type, const char * name, float x, float y, float z, const char *filename)
{
    int test = scene_->GetNumChildren();

    cout << test << endl;
    /*
    //// Get Needed SubSystems
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();

    /// Check if a scene exist
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

    return 1;*/
}


/// Main Save scene
int Manager::SaveScene(int mode)
{

    /// Check if scene exist

    if(scene_==NULL)
    {
        return 1;
    }


    String savesceneexport;

    ResourceCache * cache = GetSubsystem<ResourceCache>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();

    savesceneexport.Append(filesystem->GetProgramDir().CString());
    savesceneexport.Append("CoreData/");
    savesceneexport.Append("testing.xml");

    File saveFile(context_, savesceneexport.CString(), FILE_WRITE);

    /// Check if the account file information exist
    if(!filesystem->FileExists(savesceneexport.CString()))
    {
        //cout << "\r\nAccount file ("<< savesceneexport.CString() << ") does not exist.";
    }

    XMLFile * savesceneexportxml= new XMLFile(context_);

    XMLElement configElem = savesceneexportxml-> CreateRoot("node");

    /// point
    unsigned int childrencount=scene_->GetNumChildren();

    /// Weak Pointer children
    Vector<SharedPtr<Node> > children_;

    children_ = scene_->GetChildren();

    /// loop each child
    for (Vector<SharedPtr<Node> >::Iterator i = children_.Begin(); i != children_.End(); ++i)
    {
        /// Create a new child instance
        Node* childnode = *i;

        /// Get node infomration, check for children, and check components
         if(childnode->GetName().Find("Generated",0,false)==String::NPOS)
         {
        XMLElement NodeElement = configElem. CreateChild ("node");

        const Vector<AttributeInfo>* attributes = childnode->GetAttributes();

        /// loop through attributes
        for (Vector<AttributeInfo>::ConstIterator j = attributes->Begin(); j != attributes->End(); ++j)
        {
            XMLElement AttributeElement;

            AttributeElement = NodeElement. CreateChild ("attribute");
            AttributeElement.SetAttribute ("name", j -> name_);
            AttributeElement.SetAttribute ("value", j -> defaultValue_.ToString());

        }

        if(childnode->GetNumChildren())
        {
            SaveSceneNode(childnode, NodeElement);
        }
        else
        {
            SaveSceneNodeComponents(childnode,NodeElement);
        }

        }
    }
    savesceneexportxml->Save(saveFile);

}

/// Recursive
int Manager::SaveSceneNode(Node * node, XMLElement parentelement)
{
    /// Define a temporary pointer
    Vector<SharedPtr<Node> > subchildren_;

    /// Get children node
    subchildren_ = node->GetChildren();

    for (Vector<SharedPtr<Node> >::Iterator i = subchildren_.Begin(); i != subchildren_.End(); ++i)
    {
        /// Create a new child instance
        Node* childnode = *i;

        /// Get node infomration, check for children, and check components
        if(childnode->GetName().Find("Generated",0,false)==String::NPOS)
        {
            ///cout << "SubNode :" << childnode->GetName().CString() <<endl;
            XMLElement NodeElement = parentelement. CreateChild ("node");

            if(childnode->GetNumChildren())
            {
                SaveSceneNode(childnode, parentelement);
            }
            else
            {
                SaveSceneNodeComponents(childnode,parentelement);
            }
        }
    }
}
int Manager::SaveSceneNodeComponents(Node *node, XMLElement parentelement)
{
    /// Define temporary pointer for components
    Vector< SharedPtr< Component > > 	subcomponents_;

    /// If node has no components
    if(node->	GetNumComponents ()==0)
    {
        cout << " Node has no components" << endl;

        return 1;
    }

    /// Get children node
    subcomponents_ = node->GetComponents();

    /// Loop through components
    for (Vector<SharedPtr<Component> >::Iterator i = subcomponents_.Begin(); i != subcomponents_.End(); ++i)
    {
        Component * subcomponent = *i;

        XMLElement componentElement = parentelement.CreateChild ("component");
        componentElement.SetAttribute("Type", subcomponent->GetTypeName());

        /// READ EACH COMPONENT AND GET ATTRIBUTES
        if(subcomponent->GetNumAttributes ())
        {
            /// set virtual const
            const Vector<AttributeInfo>* attributes = subcomponent->GetAttributes();

            /// loop through attributes
            for (Vector<AttributeInfo>::ConstIterator i = attributes->Begin(); i != attributes->End(); ++i)
            {
                /// output info
                ///cout << i -> name_.CString() << " type " << i -> defaultValue_. GetTypeName ().CString() <<" value " << i -> defaultValue_.ToString().CString()<< endl;
                XMLElement AttributeElement =  componentElement. CreateChild ("attribute");
                AttributeElement.SetAttribute ("name", i -> name_);
                AttributeElement.SetAttribute ("value", i -> defaultValue_.ToString());
            }

        }
    }

    return 1;
}
