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

#include "AnimationController.h"
#include "Character.h"
#include "Context.h"
#include "MemoryBuffer.h"
#include "PhysicsEvents.h"
#include "PhysicsWorld.h"
#include "RigidBody.h"
#include "Scene.h"
#include "SceneEvents.h"
#include "Entity.h"
#include <iostream>

using namespace std;

Entity::Entity()
{
    //ctor
}

Entity::~Entity()
{
    //dtor
}

/// Set and get the character health
int Entity::SetHealth(int health)
{
    CharacterEntity.health = health;

    return 1;

}

int Entity::GetHealth(void)
{
    return CharacterEntity.health;
}


/// Don nothing now;
int Entity::Start(void)
{

    /// Initalize information
    CharacterEntity.firstname="";
    CharacterEntity.middlename="";
    CharacterEntity.lastname="";

    CharacterEntity.health= 0;

    /// player moves for surface of planet to a ship so it relations change for the x,y,z for respective object
    CharacterEntity.reputation1= 0;
    CharacterEntity.reputation2= 0;
    CharacterEntity.reputation3= 0;
    CharacterEntity.reputation4= 0;
    CharacterEntity.reputation5= 0;

    /// Set player experience
    CharacterEntity.experience = 0;
    CharacterEntity.personalitytrait=0;

    /// Charcter specs
    CharacterEntity.alienrace=0;
    CharacterEntity.alienalliancealigned=0;
    CharacterEntity.gender=0;


    return true;
}

int Entity::Clear(void)
{
    Start();

    return 1;
}

/// Get player characteristics
playercharacteristics Entity::GetCharacteristics(void)
{
    /// Charcter specs
    playercharacteristics Temporarycharacteristics;

    Temporarycharacteristics.gender = CharacterEntity.gender;
    Temporarycharacteristics.personalitytrait=CharacterEntity.personalitytrait;

    return Temporarycharacteristics;
}


/// Get player alliance
playeralliance Entity::GetAlliance(void)
{
    /// Charcter specs
    playeralliance Temporaryalliance;

    Temporaryalliance.alienrace = CharacterEntity.alienrace;
    Temporaryalliance.alienalliancealigned=CharacterEntity.alienalliancealigned;

    return Temporaryalliance;
}

/// Set player characteristics
int Entity::SetCharacteristics(playercharacteristics TempCharacteristics)
{
    /// Set character charactistics
    CharacterEntity.gender= TempCharacteristics.gender;
    CharacterEntity.personalitytrait= TempCharacteristics.personalitytrait;

    return 1;
}


/// Set player alliance
int Entity::SetAlliance(playeralliance TempAlliance)
{
    /// Set charcter alliance
    CharacterEntity.alienrace=TempAlliance.alienrace;
    CharacterEntity.alienalliancealigned=TempAlliance.alienalliancealigned;

    return 1;
}

int Entity::SetEntityInfo(playerbasicinfo TempEntity)
{
    /// Set charcter alliance
    CharacterEntity.firstname=TempEntity.firstname;
    CharacterEntity.middlename=TempEntity.middlename;
    CharacterEntity.lastname=TempEntity.lastname;

    return 1;
}


playerbasicinfo Entity::GetEntityInfo(void)
{
    /// Charcter specs
    playerbasicinfo Temporarybasicinfo;

    /// Set charcter alliance
    Temporarybasicinfo.firstname = CharacterEntity.firstname;
    Temporarybasicinfo.middlename = CharacterEntity.middlename;
    Temporarybasicinfo.lastname = CharacterEntity.lastname;

    return Temporarybasicinfo;
}
