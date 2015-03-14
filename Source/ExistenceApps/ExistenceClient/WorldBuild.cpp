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
#include "BillboardSet.h"

#include <iostream>
#include <cmath>
#include <algorithm>
#include <utility>

#include "WorldBuild.h"
#include "../../Engine/Procedural/Procedural.h"
#include "../../Engine/Procedural/ProceduralTerrain.h"



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
    // Only the physics update event is needed: unsubscribe from the rest for optimization
    SetUpdateEventMask(USE_FIXEDUPDATE);
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

int WorldBuild::Initialize(void)
{
    /// Allocate memory
    CollisionBounds.clear();
    SaveCollisionObjects=0;

    /// Clear rules
    terrainrules.creationtime=0;
    terrainrules.sealevel=0;
    terrainrules.worldtype=0;
    terrainrules.subworldtype=0;

}

/// Computer distance
float  WorldBuild::ComputeDistance(int x1, int y1, int x2, int y2)
{
    float  xrange= x1-x2;
    float  yrange= y1-y2;

    return sqrt((xrange*xrange)+(yrange*yrange));
}


/// Move world building here
int WorldBuild::GenerateWorldObjects(const time_t &timeseed,  terrain_rule terrainrule)
{
    /// Get resources
    /// Get Needed SubSystems
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();

    /// Get scene and terrain node
    Scene * scene_ = this -> GetScene();
    Node* terrainNode = scene_->GetChild("GeneratedTerrainRule_Terrain",true);
    Terrain * terrain = terrainNode -> GetComponent<Terrain>();

    /// Get terrain size
    Vector3 spacing=terrain->GetSpacing();
    int patchSize=terrain->GetPatchSize();
    IntVector2 numPatches= terrain -> GetNumPatches ();

    /// Get actual size
    Vector2 patchWorldSize=Vector2(spacing.x_*(float)(patchSize*numPatches.x_), spacing.z_*(float)(patchSize*numPatches.y_));

    bool success = false;
    bool hasComponent = false;

    /// Check component
    proceduralterrain proceduralterraindefaults;

    /// Check for the current node
    if(terrainNode)
    {
        ProceduralTerrain * proceduralComponent = terrainNode-> GetComponent<ProceduralTerrain>();

        /// Set hasComponent to true
        if(proceduralComponent)
        {
            /// Set has components
            hasComponent=true;
            /// Get component values
            proceduralterraindefaults = proceduralComponent -> Get();
        }
    }

    /// if a component exist use the terrain rule imation
    if(hasComponent)
    {
        terrainrules.sealevel=proceduralterraindefaults.sealevel;
        terrainrules.subworldtype=proceduralterraindefaults.subworldtype;
        terrainrules.worldtype=proceduralterraindefaults.worldtype;
        terrainrules.creationtime=proceduralterraindefaults.creationtime;
    }
    else
    {
        terrainrules.sealevel=terrainrule.sealevel;
        terrainrules.subworldtype=terrainrule.subworldtype;
        terrainrules.worldtype=terrainrule.worldtype;
        terrainrules.creationtime=terrainrule.creationtime;
    }

    /// Generate procedural map
    Procedural * WeightMap = new Procedural();
    Image *WeightMapImage = new Image(context_);

    /// Create a weightmap image
    WeightMap->Initialize(patchWorldSize.x_+1, patchWorldSize.y_+1);
    WeightMapImage->SetSize(patchWorldSize.x_+1, patchWorldSize.y_+1,1,4);
    WeightMap->SetOctaves(4,0.25f,false,1.0,0.6,0.4,0.3,0.2,0.1,0.1,0.1);

    WeightMapImage->SetData(WeightMap->GetImage());

    Image * terrainHeightMap= new Image(context_);

    terrainHeightMap->SetSize(patchWorldSize.x_+1, patchWorldSize.y_+1,1,4);
    terrainHeightMap ->SetData(terrain -> GetHeightMap () -> GetData());

    terrainHeightMap -> FlipVertical();

    int Spotx=0;
    int Spotz=0;

    float randomSpotx=0.0f;
    float randomSpotz=0.0f;

    /// initialize rule on seed
    RandomRule.SetRandomSeed(terrainrules.creationtime+2);

    //Debug
    //cout << "\r\nWorldBuild Seed" << terrainrules.creationtime+2<<endl;
    switch (terrainrules.worldtype)
    {
        /// world type terrain
    case WORLD_TERRAIN:
        /// Generate grass
        GenerateGrass2(patchWorldSize.x_, patchWorldSize.y_,terrainHeightMap);

        // Plant rocks
        for(unsigned int i=0; i<600; i++)
        {

            /// Pick a random spotskx
            Spotx=RandomRule.RandRange(1024);
            Spotz=RandomRule.RandRange(1024);

            randomSpotx=(float)Spotx-512.0f;
            randomSpotz=(float)Spotz-512.0f;

            /// Create rocks on paths
            CreateObjectsAlongPath(WORLDBUILD_TREES, patchWorldSize.x_, patchWorldSize.y_,randomSpotx,randomSpotz, 6, 100.0f, terrainHeightMap);
        }
        break;
    default:
        break;
    }



    return 0;

}

/// Old code
/// Create rocks along a path
int WorldBuild::CreateRockObjectAlongPath( float x, float z, float numberofobjects, float length)
{
    /// Get Needed SubSystems
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();

    /// Get scene and terrain node
    Scene * scene_ = this -> GetScene();
    Node* terrainNode = scene_->GetChild("Terrain",true);
    Terrain * terrain = terrainNode -> GetComponent<Terrain>();

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
        int direction=RandomRule.RandRange(8)+1;

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



            /// Try this method to use percentange
            if(olddistance/lengthlimitdistance>(objectsdistance*objectincrement)/lengthlimitdistance)
            {
                objectincrement++;

                /// Add a Rock to the seen - Rock Node
                Node * RockNode = scene_ -> CreateChild("RockNode");

                StaticModel * RockStaticModel = RockNode->CreateComponent<StaticModel>();

                int pick= RandomRule.RandRange(2)+1;

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
                RockNode->SetRotation(Quaternion(RandomRule.RandRange(360),Vector3(0.0f,1.0f,0.0f)));

                /// Output X, Y
                //cout << position_x << " " << position_z << "\r\n";

            }
        }
    }
    while(olddistance<=lengthlimitdistance);

    return 1;
}

/// Old code
/// Create rocks along a path
int WorldBuild::CreateTreeObjectAlongPath(float worldsize_x, float worldsize_y, float x, float z, float numberofobjects, float length, Image * terrainHeightMap)
{
    /// Get Needed SubSystems
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();

    /// Get scene and terrain node
    Scene * scene_ = this -> GetScene();
    Node* terrainNode = scene_->GetChild("Terrain",true);
    Terrain * terrain = terrainNode -> GetComponent<Terrain>();

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

    float worldsize_xlow=-(worldsize_x/2);
    float worldsize_ylow=-(worldsize_y/2);

    float worldsize_xhigh=worldsize_x/2;
    float worldsize_yhigh=worldsize_y/2;


    do
    {
        /// Pick a random directoin
        int direction=RandomRule.RandRange(8)+1;

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

            /// Prevent out of bounds
            if(position_x>worldsize_xhigh||position_x<worldsize_xlow)
            {
                continue;
            }

            if(position_z>worldsize_yhigh||position_z<worldsize_ylow)
            {
                continue;
            }

            float xposition=position_x+1024.0f;
            float zposition=position_z+1024.0f;

            Color terrainHeightvalue=terrainHeightMap->GetPixel(xposition, zposition);

            if(terrainHeightvalue.r_<terrainrules.sealevel)
            {
                continue;
            }

            Vector3 normalvalue=terrain -> GetNormal(Vector3(xposition,0.0f,zposition));

            float steep=1.0f-normalvalue.y_;

            if(steep>.009)
            {
                continue;
            }

            /// Try this method to use percentange
            if(olddistance/lengthlimitdistance>(objectsdistance*objectincrement)/lengthlimitdistance)
            {
                /// Set next pointer
                objectincrement++;

                /// Add a Rock to the seen - Rock Node
                Node * RockNode = scene_ -> CreateChild("RockNode");

                StaticModel * RockStaticModelBase = RockNode->CreateComponent<StaticModel>();
                StaticModel * RockStaticModelLeaves = RockNode->CreateComponent<StaticModel>();

                int pick= RandomRule.RandRange(3)+1;

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

                /// Set Rock position
                RockNode->SetPosition(selectPosition);
                RockNode->SetRotation(Quaternion(RandomRule.RandRange(350),Vector3(0.0f,1.0f,0.0f)));

            }
        }
    }
    while(olddistance<=lengthlimitdistance);


    return 1;
}

int WorldBuild::CreateObjectsAlongPath(int objecttypes, float worldsize_x, float worldsize_y, float x, float z, float numberofobjects, float length, Image * terrainHeightMap)
{
    /// Get Needed SubSystems
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();

    /// Get scene and terrain node
    Scene * scene_ = this -> GetScene();
    Node* terrainNode = scene_->GetChild("Terrain",true);
    Terrain * terrain = terrainNode -> GetComponent<Terrain>();

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

    float worldsize_xlow=-(worldsize_x/2);
    float worldsize_ylow=-(worldsize_y/2);

    float worldsize_xhigh=worldsize_x/2;
    float worldsize_yhigh=worldsize_y/2;

    do
    {
        /// Pick a random directoin
        int direction=RandomRule.RandRange(8)+1;

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

            /// Prevent out of bounds
            if(position_x>worldsize_xhigh||position_x<worldsize_xlow)
            {
                continue;
            }

            if(position_z>worldsize_yhigh||position_z<worldsize_ylow)
            {
                continue;
            }

            float xposition=position_x+1024.0f;
            float zposition=position_z+1024.0f;

            Color terrainHeightvalue=terrainHeightMap->GetPixel(xposition, zposition);

            if(terrainHeightvalue.r_<terrainrules.sealevel+0.01f)
            {
                continue;
            }

            Vector3 normalvalue=terrain -> GetNormal(Vector3(xposition,0.0f,zposition));

            float steep=1.0f-normalvalue.y_;

            if(steep>.009)
            {
                continue;
            }


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
                    pick= RandomRule.RandRange(2)+1;

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
                    pick= RandomRule.RandRange(3)+1;

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
                    return 1;
                    break;
                }

                /// Add name
                ObjectStaticNode->SetName("GeneratedAsset_Object");

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
                /*            CollisionBounds.at(SaveCollisionObjects).size_x=ObjectStaticModelCenter.x_;
                            CollisionBounds.at(SaveCollisionObjects).size_y=ObjectStaticModelCenter.y_;
                            CollisionBounds.at(SaveCollisionObjects).size_z=ObjectStaticModelCenter.z_;
                            CollisionBounds.at(SaveCollisionObjects).origin_x=position_x;
                            CollisionBounds.at(SaveCollisionObjects).origin_y=terrain->GetHeight(Vector3(position_x,0.0f,position_z))+ObjectStaticModelCenter.y_;
                            CollisionBounds.at(SaveCollisionObjects).origin_z=position_z;
                            CollisionBounds.at(SaveCollisionObjects).lod=0.0f;

                            /// Save object
                            SaveCollisionObjects++;*/

                /// Set Rock position
                ObjectStaticNode->SetPosition(selectPosition);
                ObjectStaticNode->SetRotation(Quaternion(Vector3::UP, terrain->GetNormal(Vector3(position_x,0.0f,position_z))));
                ObjectStaticNode->SetRotation(Quaternion(RandomRule.RandRange(360),Vector3(0.0f,1.0f,0.0f)));
            }
        }
    }
    while(olddistance<=lengthlimitdistance);


    return 1;
}


int WorldBuild::GenerateGrass2(float worldsize_x, float worldsize_y, Image * terrainHeightMap)
{

/// Get Needed SubSystems
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();

    /// Get scene and terrain node
    Scene * scene_ = this -> GetScene();
    Node* terrainNode = scene_->GetChild("Terrain",true);
    Terrain * terrain = terrainNode -> GetComponent<Terrain>();

    /// Define random point variables
    float GrowthSpotx=0;
    float GrowthSpotz=0;
    int InitialSpotx=0;
    int InitialSpotz=0;

    int TerrainInitialSpotx=0;
    int TerrainInitialSpotz=0;
    float TerrainGrowthSpotx=0.0f;
    float TerrainGrowthSpotz=0.0f;

    float GowthRandomSpotx=0;
    float GowthRandomSpotz=0;
    int InitialrandomSpotx=0;
    int InitialrandomSpotz=0;

    int NumberOfPlantingsGrowth=0;
    int NumberOfPlantings=0;
    unsigned int InitialRange=0;
    unsigned int  SpreadRange=0;

    float HeightMapxposition;
    float HeightMapzposition;


    int j=0;
    bool failattempts;

    /// Change parameters based on type
    switch (terrainrules.worldtype)
    {

    case WORLD_DESERT:
        NumberOfPlantingsGrowth=25;
        NumberOfPlantings=50;
        InitialRange=100;
        SpreadRange=100;
        break;
    case WORLD_TERRAIN:
        NumberOfPlantingsGrowth=500;
        NumberOfPlantings=1200;
        InitialRange=1024;
        SpreadRange=32;
        break;
    default:
        break;
    }

    float steep=0.0f;

    /// create billboards
    for(unsigned int i=0; i<NumberOfPlantings; ++i)
    {

        /// Pick random values
        InitialSpotx=RandomRule.RandRange(InitialRange);
        InitialSpotz=RandomRule.RandRange(InitialRange);

        TerrainInitialSpotx=InitialSpotx+(InitialRange/2);
        TerrainInitialSpotz=InitialSpotz+(InitialRange/2);

        Node* GrassInitialNode = scene_->CreateChild("GrassBillboardSetNode");
        GrassInitialNode->SetPosition(Vector3(InitialrandomSpotx,0.0f,InitialrandomSpotz));
        GrassInitialNode->SetName("GeneratedAsset_GrassBillNode");

        BillboardSet* billboardObject = GrassInitialNode->CreateComponent<BillboardSet>();
        billboardObject->SetNumBillboards(NumberOfPlantingsGrowth);

        billboardObject->SetMaterial(cache->GetResource<Material>("Resources/Materials/Grass.xml"));
        billboardObject->SetSorted(true);
        billboardObject->SetCastShadows(true);

        /// reset
        failattempts=false;
        j=0;

        HeightMapxposition=floor(TerrainInitialSpotx+.5)+0.0f;
        HeightMapzposition=floor(TerrainInitialSpotz+.5)+0.0f;

        Color terrainHeightvalue=terrainHeightMap->GetPixel((int)HeightMapxposition,(int)HeightMapzposition);

        Vector3 normalvalue=terrain -> GetNormal(Vector3(TerrainGrowthSpotx,0.0f,TerrainGrowthSpotz));

        float steep=(float)1.0f-normalvalue.y_;

        if(steep>.099)
        {
            failattempts=true;
        }


        if(terrainrules.sealevel>0.0f)
        {
            if(terrainHeightvalue.r_<terrainrules.sealevel+0.01f)
            {
                failattempts=true;
            }
        }

        if(failattempts==false)
        {
            for(unsigned int j=0; j<NumberOfPlantingsGrowth; ++j)
            {
                GrowthSpotx=RandomRule.RandRange(SpreadRange*100);
                GrowthSpotz=RandomRule.RandRange(SpreadRange*100);

                GowthRandomSpotx=((float)GrowthSpotx/100)-(SpreadRange/2);
                GowthRandomSpotz=((float)GrowthSpotz/100)-(SpreadRange/2);

                TerrainGrowthSpotx=TerrainInitialSpotx+GowthRandomSpotx-1024;
                TerrainGrowthSpotz=TerrainInitialSpotz+GowthRandomSpotz-1024;

                Billboard* bb = billboardObject->GetBillboard(j);

                /// Select a possible position to place a plant
                Vector3 selectPosition=Vector3(TerrainGrowthSpotx,terrain->GetHeight(Vector3(TerrainGrowthSpotx,0.0f,TerrainGrowthSpotz)),TerrainGrowthSpotz);

                bb->position_ =selectPosition;
                bb->size_ = Vector2(Random(0.3f) + 0.1f, Random(0.3f) + 0.1f);

                bb->enabled_ = true;
            }

        }


    }

    return 1;
}


