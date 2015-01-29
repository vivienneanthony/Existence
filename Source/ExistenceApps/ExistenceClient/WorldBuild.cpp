/// Headers and etc
#include "CoreEvents.h"
#include "Engine.h"
#include "ProcessUtils.h"
#include "Octree.h"
#include "Model.h"
#include "Material.h"
#include "ResourceCache.h"
#include "Graphics.h"

#include "AnimationController.h"
#include "Character.h"
#include "Context.h"
#include "MemoryBuffer.h"
#include "PhysicsEvents.h"
#include "PhysicsWorld.h"
#include "RigidBody.h"
#include "Scene.h"
#include "SceneEvents.h"
#include "Player.h"
#include "Renderer.h"
#include "UI.h"
#include "Node.h"
#include "CollisionShape.h"
#include "StaticModel.h"
#include "Terrain.h"


#include <iostream>
#include <cmath>
#include <algorithm>
#include <utility>
#include "WorldBuild.h"

using namespace std;

#define NORTH 1
#define NORTHEAST 2
#define EAST 3
#define SOUTHEAST 4
#define SOUTH  5
#define SOUTHWEST 6
#define WEST 7
#define NORTHWEST 8

WorldBuild::WorldBuild(Context* context) :
    LogicComponent (context)
{
    //ctor
}

WorldBuild::~WorldBuild()
{

    //dtor
}

/// Register a object
void WorldBuild::RegisterObject(Context* context)
{
    context->RegisterFactory<WorldBuild>();
}

int WorldBuild::Init(void)
{
    /// Allocate memory
    //WorldOjectCollisionMap * CollisionBounds = new WorldOjectCollisionMap[300];
    CollisionBounds.clear();
    CollisionBounds.resize(1000);
    SaveCollisionObjects=0;

}

/// Computer distance
float  WorldBuild::ComputeDistance(int x1, int y1, int x2, int y2)
{
    float  xrange= x1-x2;
    float  yrange= y1-y2;

    return sqrt((xrange*xrange)+(yrange*yrange));
}

/// Create rocks along a path
int WorldBuild::CreateRockObjectAlongPath(float x, float z, float numberofobjects, float length)
{
    /// Get Needed SubSystems
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();

    /// Try to get the node information;
    Scene * scene_;

    scene_ = this -> GetScene();

    Node* terrainNode = scene_ ->GetChild("Terrain",true);

    Terrain* terrain = terrainNode->GetComponent<Terrain>();

    //cout << "\r\nSave" << SaveCollisionObjects;
    //cout << "\r\nBounds" << CollisionBounds.size();
    //cout << "another path";

    /// Need variables
    float lengthlimitdistance= length;

    float objectsalongpath=numberofobjects;
    float objectsdistance=lengthlimitdistance/objectsalongpath;
    float objectincrement=1;

    float origin_x=x;
    float origin_z=z;

    float difference_z=0.0f;
    float difference_x=0.0f;

    float position_x=0.0f;
    float position_z=0.0f;

    float newposition_x=0.0f;
    float newposition_z=0.0f;
    float olddistance=0.0f;


    position_x=origin_x;
    position_z=origin_z;

    do
    {
        /// Pick a random directoin
        int direction=rand()%8+1;

        /// Select coordinate change based on random direction
        switch (direction)
        {
        case NORTH:
            difference_x=0;
            difference_z=1;
            break;
        case NORTHEAST:
            difference_x=1;
            difference_z=1;
            break;
        case EAST:
            difference_x=+1;
            difference_z=0;
            break;
        case SOUTHEAST:
            difference_x=1;
            difference_z=-1;
            break;
        case SOUTH:
            difference_x=0;
            difference_z=-1;
            break;
        case SOUTHWEST:
            difference_x=-1;
            difference_z=-1;
            break;
        case WEST:
            difference_x=-1;
            difference_z=0;
            break;
        case NORTHWEST:
            difference_x=-1;
            difference_z=1;
            break;
        }

        /// If distance less then current distance then while continue loop
        if(ComputeDistance(position_x+difference_x, origin_x, position_z+difference_z,origin_z)<olddistance)
        {
            continue;
        }
        else
        {
            /// Create a new position
            newposition_x=position_x+difference_x;
            newposition_z=position_z+difference_z;

            ///  Copy newposition to current positon
            position_x=newposition_x;
            position_z=newposition_z;

            /// Get distance
            olddistance=ComputeDistance(position_x, origin_x, position_z, origin_z);

            /// Try this method to use percentange
            if(olddistance/lengthlimitdistance>(objectsdistance*objectincrement)/lengthlimitdistance)
            {
                objectincrement++;

                /// Add a Rock to the seen - Rock Node
                Node * RockNode = scene_ -> CreateChild("RockNode");

                StaticModel * RockStaticModel = RockNode->CreateComponent<StaticModel>();

                int pick= rand()%2+1;

                if(pick==1)
                {
                    RockStaticModel->SetModel(cache->GetResource<Model>("Resources/Models/Rock1.mdl"));
                    RockStaticModel->ApplyMaterialList("Resources/Models/Rock1.txt");

                }
                else
                {
                    RockStaticModel->SetModel(cache->GetResource<Model>("Resources/Models/Rock2.mdl"));
                    RockStaticModel->ApplyMaterialList("Resources/Models/Rock2.txt");

                }

                /// Create Nodes and COmponents
                RockStaticModel->SetCastShadows(true);

                BoundingBox  staticmodelbox = RockStaticModel->GetBoundingBox();
                Vector3  staticmodelboxcenter= staticmodelbox.HalfSize();


                /// Select a possible position to place a Rock
                Vector3 selectPosition=Vector3(position_x,terrain->GetHeight(Vector3(position_x,0.0f,position_z))+staticmodelboxcenter.y_,position_z);

                /// Save collisions
                CollisionBounds.at(SaveCollisionObjects).size_x=staticmodelboxcenter.x_;
                CollisionBounds.at(SaveCollisionObjects).size_y=staticmodelboxcenter.y_;
                CollisionBounds.at(SaveCollisionObjects).size_z=staticmodelboxcenter.z_;
                CollisionBounds.at(SaveCollisionObjects).origin_x=position_x;
                CollisionBounds.at(SaveCollisionObjects).origin_y=terrain->GetHeight(Vector3(position_x,0.0f,position_z))+staticmodelboxcenter.y_;
                CollisionBounds.at(SaveCollisionObjects).origin_z=position_z;
                CollisionBounds.at(SaveCollisionObjects).lod=0.0f;

                /// Save object
                SaveCollisionObjects++;

                /// Set Rock position
                RockNode->SetPosition(selectPosition);
                RockNode->SetRotation(Quaternion(Vector3::UP, terrain->GetNormal(Vector3(position_x,0.0f,position_z))));
                RockNode->SetRotation(Quaternion(Random(360),Vector3(0.0f,1.0f,0.0f)));

                /// Output X, Y
                //cout << position_x << " " << position_z << "\r\n";

            }
        }
    }
    while(olddistance<=lengthlimitdistance);

    return 1;
}


/// Create rocks along a path
int WorldBuild::CreateTreeObjectAlongPath(float x, float z, float numberofobjects, float length)
{
    /// Get Needed SubSystems
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();

    /// Try to get the node information;
    Scene * scene_;

    scene_ = this -> GetScene();

    Node* terrainNode = scene_ ->GetChild("Terrain",true);

    Terrain* terrain = terrainNode->GetComponent<Terrain>();

    //cout << "\r\nSave" << SaveCollisionObjects;
    //cout << "\r\nBounds" << CollisionBounds.size();
    //cout << "another path";


    /// Need variables
    float lengthlimitdistance= length;

    float objectsalongpath=numberofobjects;
    float objectsdistance=lengthlimitdistance/objectsalongpath;
    float objectincrement=1;

    float origin_x=x;
    float origin_z=z;

    float difference_z=0.0f;
    float difference_x=0.0f;

    float position_x=0.0f;
    float position_z=0.0f;

    float newposition_x=0.0f;
    float newposition_z=0.0f;
    float olddistance=0.0f;

    position_x=origin_x;
    position_z=origin_z;

    do
    {
        /// Pick a random directoin
        int direction=rand()%8+1;

        /// Select coordinate change based on random direction
        switch (direction)
        {
        case NORTH:
            difference_x=0;
            difference_z=1;
            break;
        case NORTHEAST:
            difference_x=1;
            difference_z=1;
            break;
        case EAST:
            difference_x=+1;
            difference_z=0;
            break;
        case SOUTHEAST:
            difference_x=1;
            difference_z=-1;
            break;
        case SOUTH:
            difference_x=0;
            difference_z=-1;
            break;
        case SOUTHWEST:
            difference_x=-1;
            difference_z=-1;
            break;
        case WEST:
            difference_x=-1;
            difference_z=0;
            break;
        case NORTHWEST:
            difference_x=-1;
            difference_z=1;
            break;
        }

        /// If distance less then current distance then while continue loop
        if(ComputeDistance(position_x+difference_x, origin_x, position_z+difference_z,origin_z)<olddistance)
        {
            continue;
        }
        else
        {
            /// Create a new position
            newposition_x=position_x+difference_x;
            newposition_z=position_z+difference_z;

            ///  Copy newposition to current positon
            position_x=newposition_x;
            position_z=newposition_z;

            /// Get distance
            olddistance=ComputeDistance(position_x, origin_x, position_z, origin_z);

            /// Try this method to use percentange
            if(olddistance/lengthlimitdistance>(objectsdistance*objectincrement)/lengthlimitdistance)
            {
                /// Set next pointer
                objectincrement++;

                /// Add a Rock to the seen - Rock Node
                Node * RockNode = scene_ -> CreateChild("RockNode");

                StaticModel * RockStaticModelBase = RockNode->CreateComponent<StaticModel>();
                StaticModel * RockStaticModelLeaves = RockNode->CreateComponent<StaticModel>();

                int pick= rand()%3+1;

                switch (pick)
                {

                case 1:

                    RockStaticModelBase->SetModel(cache->GetResource<Model>("Resources/Models/AlienTree1.mdl"));
                    RockStaticModelBase->ApplyMaterialList("Resources/Models/AlienTree1.txt");

                    RockStaticModelLeaves->SetModel(cache->GetResource<Model>("Resources/Models/AlienTree1Leaves.mdl"));
                    RockStaticModelLeaves->ApplyMaterialList("Resources/Models/AlienTree1Leaves.txt");
                    break;

                case 2:
                    RockStaticModelBase->SetModel(cache->GetResource<Model>("Resources/Models/AlienTree2.mdl"));
                    RockStaticModelBase->ApplyMaterialList("Resources/Models/AlienTree2.txt");

                    RockStaticModelLeaves->SetModel(cache->GetResource<Model>("Resources/Models/AlienTree2Leaves.mdl"));
                    RockStaticModelLeaves->ApplyMaterialList("Resources/Models/AlienTree2Leaves.txt");

                    break;
                default:

                    RockStaticModelBase->SetModel(cache->GetResource<Model>("Resources/Models/AlienTree3.mdl"));
                    RockStaticModelBase->ApplyMaterialList("Resources/Models/AlienTree3.txt");

                    RockStaticModelLeaves->SetModel(cache->GetResource<Model>("Resources/Models/AlienTree3Leaves.mdl"));
                    RockStaticModelLeaves->ApplyMaterialList("Resources/Models/AlienTree3Leaves.txt");
                }

                /// Create Nodes and COmponents
                RockStaticModelBase->SetCastShadows(true);
                RockStaticModelLeaves->SetCastShadows(true);

                BoundingBox  staticmodelbox = RockStaticModelBase->GetBoundingBox();
                Vector3  staticmodelboxcenter= staticmodelbox.HalfSize();

                /// Select a possible position to place a Rock
                Vector3 selectPosition=Vector3(position_x,terrain->GetHeight(Vector3(position_x,0.0f,position_z))+staticmodelboxcenter.y_,position_z);

                /// Save coordinates
                CollisionBounds.at(SaveCollisionObjects).size_x=staticmodelboxcenter.x_;
                CollisionBounds.at(SaveCollisionObjects).size_y=staticmodelboxcenter.y_;
                CollisionBounds.at(SaveCollisionObjects).size_z=staticmodelboxcenter.z_;
                CollisionBounds.at(SaveCollisionObjects).origin_x=position_x;
                CollisionBounds.at(SaveCollisionObjects).origin_z=terrain->GetHeight(Vector3(position_x,0.0f,position_z))+staticmodelboxcenter.y_;
                CollisionBounds.at(SaveCollisionObjects).origin_z=position_z;
                CollisionBounds.at(SaveCollisionObjects).lod=0.0f;

                /// Save object
                SaveCollisionObjects++;

                /// Set Rock position
                RockNode->SetPosition(selectPosition);
                RockNode->SetRotation(Quaternion(Random(360),Vector3(0.0f,1.0f,0.0f)));

                /// Output X, Y
                //cout << position_x << " " << position_z << "\r\n";
            }
        }
    }
    while(olddistance<=lengthlimitdistance);


    return 1;
}

int WorldBuild::CreateObjectsAlongPath(int objecttypes, float x, float z, float numberofobjects, float length)
{
    /// Get Needed SubSystems
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();

    /// Try to get the node information;
    Scene * scene_;

    scene_ = this -> GetScene();

    Node* terrainNode = scene_ ->GetChild("Terrain",true);

    Terrain* terrain = terrainNode->GetComponent<Terrain>();

    //cout << "\r\nSave" << SaveCollisionObjects;
    //cout << "\r\nBounds" << CollisionBounds.size();
    //cout << "another path";


    /// Need variables
    float lengthlimitdistance= length;

    float objectsalongpath=numberofobjects;
    float objectsdistance=lengthlimitdistance/objectsalongpath;
    float objectincrement=1;

    float origin_x=x;
    float origin_z=z;

    float difference_z=0.0f;
    float difference_x=0.0f;

    float position_x=0.0f;
    float position_z=0.0f;

    float newposition_x=0.0f;
    float newposition_z=0.0f;
    float olddistance=0.0f;

    position_x=origin_x;
    position_z=origin_z;

    do
    {
        /// Pick a random directoin
        int direction=rand()%8+1;

        /// Select coordinate change based on random direction
        switch (direction)
        {
        case NORTH:
            difference_x=0;
            difference_z=1;
            break;
        case NORTHEAST:
            difference_x=1;
            difference_z=1;
            break;
        case EAST:
            difference_x=+1;
            difference_z=0;
            break;
        case SOUTHEAST:
            difference_x=1;
            difference_z=-1;
            break;
        case SOUTH:
            difference_x=0;
            difference_z=-1;
            break;
        case SOUTHWEST:
            difference_x=-1;
            difference_z=-1;
            break;
        case WEST:
            difference_x=-1;
            difference_z=0;
            break;
        case NORTHWEST:
            difference_x=-1;
            difference_z=1;
            break;
        }


        /// If distance less then current distance then while continue loop
        if(ComputeDistance(position_x+difference_x, origin_x, position_z+difference_z,origin_z)<olddistance)
        {
            continue;
        }
        else
        {
            /// Create a new position
            newposition_x=position_x+difference_x;
            newposition_z=position_z+difference_z;

            ///  Copy newposition to current positon
            position_x=newposition_x;
            position_z=newposition_z;

            /// Get distance
            olddistance=ComputeDistance(position_x, origin_x, position_z, origin_z);

            /// Try this method to use percentange
            if(olddistance/lengthlimitdistance>(objectsdistance*objectincrement)/lengthlimitdistance)
            {
                /// Set next pointer
                objectincrement++;

                /// Add a object node
                Node * ObjectStaticNode= scene_ -> CreateChild("RockNode");

                /// Create nodes
                StaticModel * ObjectStaticModelBase = ObjectStaticNode->CreateComponent<StaticModel>();
                StaticModel * ObjectStaticModelBaseLeaves = ObjectStaticNode->CreateComponent<StaticModel>();

                unsigned int pick=0;

                switch (objecttypes)
                {
                case WORLDBUILD_ROCKS:
                    /// Pick random
                    pick= rand()%2+1;

                    if(pick==1)
                    {
                        ObjectStaticModelBase->SetModel(cache->GetResource<Model>("Resources/Models/Rock1.mdl"));
                        ObjectStaticModelBase->ApplyMaterialList("Resources/Models/Rock1.txt");

                    }
                    else
                    {
                        ObjectStaticModelBase->SetModel(cache->GetResource<Model>("Resources/Models/Rock2.mdl"));
                        ObjectStaticModelBase->ApplyMaterialList("Resources/Models/Rock2.txt");

                    }
                    break;
                case WORLDBUILD_TREES:
                    /// Pick Random
                    pick= rand()%3+1;

                    switch (pick)
                    {

                    case 1:
                        ObjectStaticModelBase->SetModel(cache->GetResource<Model>("Resources/Models/AlienTree1.mdl"));
                        ObjectStaticModelBase->ApplyMaterialList("Resources/Models/AlienTree1.txt");

                        ObjectStaticModelBaseLeaves->SetModel(cache->GetResource<Model>("Resources/Models/AlienTree1Leaves.mdl"));
                        ObjectStaticModelBaseLeaves->ApplyMaterialList("Resources/Models/AlienTree1Leaves.txt");
                        break;

                    case 2:
                        ObjectStaticModelBase->SetModel(cache->GetResource<Model>("Resources/Models/AlienTree2.mdl"));
                        ObjectStaticModelBase->ApplyMaterialList("Resources/Models/AlienTree2.txt");

                        ObjectStaticModelBaseLeaves->SetModel(cache->GetResource<Model>("Resources/Models/AlienTree2Leaves.mdl"));
                        ObjectStaticModelBaseLeaves->ApplyMaterialList("Resources/Models/AlienTree2Leaves.txt");

                        break;
                    case 3:
                        ObjectStaticModelBase->SetModel(cache->GetResource<Model>("Resources/Models/AlienTree3.mdl"));
                        ObjectStaticModelBase->ApplyMaterialList("Resources/Models/AlienTree3.txt");

                        ObjectStaticModelBaseLeaves->SetModel(cache->GetResource<Model>("Resources/Models/AlienTree3Leaves.mdl"));
                        ObjectStaticModelBaseLeaves->ApplyMaterialList("Resources/Models/AlienTree3Leaves.txt");
                        break;
                    default:
                        /// Do nothing
                        break;
                    }
                default:
                    /// Do nothing
                    break;
                }


                /// Create Nodes and COmponents
                ObjectStaticModelBase->SetCastShadows(true);

                if(objecttypes==WORLDBUILD_TREES)
                {
                    ObjectStaticModelBaseLeaves->SetCastShadows(true);
                }

                /// Get dimensions
                BoundingBox  ObjectStaticModelBox = ObjectStaticModelBase ->GetBoundingBox();
                Vector3  ObjectStaticModelCenter= ObjectStaticModelBox.HalfSize();

                /// Select a possible position to place a Rock
                Vector3 selectPosition=Vector3(position_x,terrain->GetHeight(Vector3(position_x,0.0f,position_z))+ObjectStaticModelCenter.y_,position_z);

                /// Save collisions
                CollisionBounds.at(SaveCollisionObjects).size_x=ObjectStaticModelCenter.x_;
                CollisionBounds.at(SaveCollisionObjects).size_y=ObjectStaticModelCenter.y_;
                CollisionBounds.at(SaveCollisionObjects).size_z=ObjectStaticModelCenter.z_;
                CollisionBounds.at(SaveCollisionObjects).origin_x=position_x;
                CollisionBounds.at(SaveCollisionObjects).origin_y=terrain->GetHeight(Vector3(position_x,0.0f,position_z))+ObjectStaticModelCenter.y_;
                CollisionBounds.at(SaveCollisionObjects).origin_z=position_z;
                CollisionBounds.at(SaveCollisionObjects).lod=0.0f;

                /// Save object
                SaveCollisionObjects++;

                /// Set Rock position
                ObjectStaticNode->SetPosition(selectPosition);
                ObjectStaticNode->SetRotation(Quaternion(Vector3::UP, terrain->GetNormal(Vector3(position_x,0.0f,position_z))));
                ObjectStaticNode->SetRotation(Quaternion(Random(360),Vector3(0.0f,1.0f,0.0f)));
            }
        }
    }
    while(olddistance<=lengthlimitdistance);


    return 1;
}

