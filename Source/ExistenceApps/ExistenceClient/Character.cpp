//
// Copyright (c) 2008-2014 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//



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
#include <iostream>

#include "GameObject.h"

using namespace Urho3D;

Character::Character(Context* context) :
    LogicComponent(context),
    onGround_(false),
    okToJump_(true),
    inAirTimer_(0.0f),
    CharacterPlayer()
{
    // Only the physics update event is needed: unsubscribe from the rest for optimization
    SetUpdateEventMask(USE_FIXEDUPDATE);
}

void Character::RegisterObject(Context* context)
{
    context->RegisterFactory<Character>();

    // These macros register the class attributes to the Context for automatic load / save handling.
    ATTRIBUTE("On Ground", bool,onGround_, false, AM_DEFAULT);
    ATTRIBUTE("OK To Jump", bool, okToJump_, true, AM_DEFAULT);
    ATTRIBUTE("In Air Timer", float, inAirTimer_, 0.0f, AM_DEFAULT);

}

void Character::Start()
{
    /// Initalize additional information
    CharacterPlayer.Start();

    /// Component has been inserted into its scene node. Subscribe to events now
    SubscribeToEvent(GetNode(), E_NODECOLLISION, HANDLER(Character, HandleNodeCollision));
}

/// Set character health function
int Character::SetHealth(int health)
{

    /// Set Health in the player class
    CharacterPlayer.SetHealth(health);

    return 1;
}

/// Set character health function
int Character::GetHealth(void)
{

    int health=CharacterPlayer.GetHealth();


    return health;
}

/// Set character first, middle, and lastname
int Character::SetPlayerInfo(playerbasicinfo TempPlayer)
{
    /// Set charcter alliance
    CharacterPlayer.SetPlayerInfo(TempPlayer);

    return 1;
}


/// Get character first, middle, and lastname
playerbasicinfo Character::GetPlayerInfo(void)
{
    /// Charcter specs
    playerbasicinfo Temporarybasicinfo;

    /// Set charcter alliance
    Temporarybasicinfo= CharacterPlayer.GetPlayerInfo();

    return Temporarybasicinfo;
}




/// Get player characteristics
playercharacteristics Character::GetCharacteristics(void)
{
    /// Charcter specs
    playercharacteristics Temporarycharacteristics;

    /// Set charcter alliance
    Temporarycharacteristics = CharacterPlayer.GetCharacteristics();

    return Temporarycharacteristics;
}


/// Get player alliance
playeralliance Character::GetAlliance(void)
{
    /// Charcter specs
    playeralliance Temporaryalliance;

    Temporaryalliance = CharacterPlayer.GetAlliance();

    return Temporaryalliance;
}

/// Set player characteristics
int Character::SetCharacteristics(playercharacteristics TempCharacteristics)
{
    /// Set character charactistics
    CharacterPlayer.SetCharacteristics(TempCharacteristics);

    return 1;
}


/// Set player alliance
int Character::SetAlliance(playeralliance TempAlliance)
{
    /// Set charcter alliance
    CharacterPlayer.SetAlliance(TempAlliance);

    return 1;
}


/// Create a object
void Character::MagicBox(void)
{
    /// Get Needed SubSystems
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();

    Scene * scene_;

    scene_ = this -> GetScene();

    /// Create Node
    Node * mushroomNode = scene_ -> CreateChild("MushroomNode");
    Node * characterNode = scene_ -> GetChild("Character");

    Vector3 characterPosition = characterNode -> GetPosition();

    /// Get Node position
    mushroomNode->SetPosition(characterPosition+Vector3(0.0f,1.0f,3.0f));

    /// Load mushroom
    StaticModel* mushroomObject = mushroomNode->CreateComponent<StaticModel>();
    mushroomObject->SetModel(cache->GetResource<Model>("Models/Mushroom.mdl"));
    mushroomObject->SetMaterial(cache->GetResource<Material>("Materials/Mushroom.xml"));

    /// Create physics
    CollisionShape* mushroomShape = mushroomNode->CreateComponent<CollisionShape>();
    mushroomShape->SetBox(Vector3::ONE);
    mushroomShape ->SetLodLevel(1);

    RigidBody* mushroomBody= mushroomNode ->CreateComponent<RigidBody>();
    mushroomBody->SetCollisionLayer(1);
    mushroomBody->SetCollisionEventMode(COLLISION_ALWAYS);

    /// Set Lifetime
    GameObject * Lifetime = mushroomNode->CreateComponent<GameObject>();
    Lifetime->SetLifetime(20);

    mushroomBody->	SetMass (.5);
    mushroomBody-> SetLinearVelocity(Vector3(0.0f,0.0f,2.0f));

    return;
}

void Character::FixedUpdate(float timeStep)
{
    /// Could cache the components for faster access instead of finding them each frame
    RigidBody* body = GetComponent<RigidBody>();
    AnimationController* animCtrl = GetComponent<AnimationController>();

    /// Update the in air timer. Reset if grounded
    if (!onGround_)
    {

        inAirTimer_ += timeStep;
    }
    else
    {

        inAirTimer_ = 0.0f;
    }
    /// When character has been in air less than 1/10 second, it's still interpreted as being on ground
    bool softGrounded = inAirTimer_ < INAIR_THRESHOLD_TIME;

    /// Update movement & animation
    const Quaternion& rot = node_->GetRotation();
    Vector3 moveDir = Vector3::ZERO;
    const Vector3& velocity = body->GetLinearVelocity();
    /// Velocity on the XZ plane
    Vector3 planeVelocity(velocity.x_, 0.0f, velocity.z_);

    /// Create quaternion for a new rotation
    Quaternion  newrotation;

    /// Controls force of movement
    if (controls_.IsDown(CTRL_FORWARD))
    {
        moveDir += Vector3::FORWARD;
    }
    if (controls_.IsDown(CTRL_BACK))
    {
        moveDir += Vector3::BACK;
    }
    if (controls_.IsDown(CTRL_LEFT))
    {
        newrotation = rot * Quaternion(-1.0f, Vector3(0.0f, 1.0f, 0.0f)); // Pitch
    }

    if (controls_.IsDown(CTRL_RIGHT))
    {
        newrotation = rot * Quaternion(1.0f, Vector3(0.0f, 1.0f, 0.0f)); // Pitch
    }

    if (controls_.IsDown(CTRL_FIRE))
    {
        MagicBox();
    }

    /// Normalize move vector so that diagonal strafing is not faster
    if (moveDir.LengthSquared() > 0.0f)
        moveDir.Normalize();

    /// If in air, allow control, but slower than when on ground
    body->ApplyImpulse(rot * moveDir * (softGrounded ? MOVE_FORCE : INAIR_MOVE_FORCE));

    /// Set Rotation if left and right is pressed
    if (controls_.IsDown(CTRL_RIGHT)||controls_.IsDown(CTRL_LEFT))
    {
        body->SetRotation(newrotation);
    }

    if (softGrounded)
    {
        // When on ground, apply a braking force to limit maximum ground velocity
        Vector3 brakeForce = -planeVelocity * BRAKE_FORCE;
        body->ApplyImpulse(brakeForce);
        // Jump. Must release jump control inbetween jumps
        if (controls_.IsDown(CTRL_JUMP))
        {
            if (okToJump_)
            {
                body->ApplyImpulse(Vector3::UP * JUMP_FORCE);
                okToJump_ = false;
            }
        }
        else
        {
            okToJump_ = true;
        }

    }

    // Reset grounded flag for next frame
    onGround_ = false;
}

/// Handler for Node Collisions
void Character::HandleNodeCollision(StringHash eventType, VariantMap& eventData)
{

    using namespace NodeCollision;

    /// Get the other colliding body, make sure it is moving (has nonzero mass)
    RigidBody* otherBody = (RigidBody*)eventData[P_OTHERBODY].GetPtr();

    Node* otherNode = (Node*)eventData[P_OTHERNODE].GetPtr();

    /// If the other collision shape belongs to static geometry, perform world collision
    /// If collision is on the collision layer
    if (otherBody->GetCollisionLayer() == 1)
    {

    }   WorldCollision(eventData);

    /// Check collision with artificial space
    if (otherBody->GetCollisionLayer() == 2)
    {
        ArtificialSpaceCollision(eventData);
    }

    return;
}


/// Handle World Collisions
void Character::WorldCollision(VariantMap& eventData)
{

    /// Check collision contacts and see if character is standing on ground (look for a contact that has near vertical normal)
    using namespace NodeCollision;

    /// Could cache the components for faster access instead of finding them each frame
    RigidBody* body = GetComponent<RigidBody>();

    MemoryBuffer contacts(eventData[P_CONTACTS].GetBuffer());

    /// Loop through contacts
    while (!contacts.IsEof())
    {
        Vector3 contactPosition = contacts.ReadVector3();
        Vector3 contactNormal = contacts.ReadVector3();
        float contactDistance = contacts.ReadFloat();
        float contactImpulse = contacts.ReadFloat();
        Vector3 nodePosition = node_->GetPosition();

        // If contact is below node center and mostly vertical, assume it's a ground contact
        if (contactPosition.y_ < (node_->GetPosition().y_)+1.0f)
        {
            float level = Abs(contactNormal.y_);

            if (level > 0.75)
            {
                onGround_ = true;

            }

        }

    }
}

/// Change world gravity based on collision
void Character::ArtificialSpaceCollision(VariantMap& eventData)
{
    /// Get Rigid Body
    RigidBody* body = GetComponent<RigidBody>();

    /// Get the Rigid Body Physics World
    PhysicsWorld * World = 	body -> GetPhysicsWorld ();

    /// Set World Gravity
    if(World -> GetGravity()==Vector3(0.0f,0.0f,0.0f))
    {

        World -> SetGravity (Vector3(0.0f,-9.0f,0.0f));
    }

}
