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
#include "Player.h"
#include <iostream>

using namespace std;

Player::Player()
{
    //ctor
}

Player::~Player()
{
    //dtor
}

/// Set and get the character health
int Player::SetHealth(int health)
{
    CharacterPlayer.health = health;

    return 1;

}

int Player::GetHealth(void)
{
    return CharacterPlayer.health;
}


/// Don nothing now;
int Player::Start(void)
{

    /// Initalize information
    CharacterPlayer.firstname="";
    CharacterPlayer.middlename="";
    CharacterPlayer.lastname="";

    CharacterPlayer.health= 0;

    /// player moves for surface of planet to a ship so it relations change for the x,y,z for respective object
    CharacterPlayer.reputation1= 0;
    CharacterPlayer.reputation2= 0;
    CharacterPlayer.reputation3= 0;
    CharacterPlayer.reputation4= 0;
    CharacterPlayer.reputation5= 0;

    /// Set player experience
    CharacterPlayer.experience = 0;
    CharacterPlayer.personalitytrait=0;

    /// Charcter specs
    CharacterPlayer.alienrace=0;
    CharacterPlayer.alienalliancealigned=0;
    CharacterPlayer.gender=0;


    return true;
}

int Player::Clear(void)
{
    Start();

    return 1;
}

/// Get player characteristics
playercharacteristics Player::GetCharacteristics(void)
{
    /// Charcter specs
    playercharacteristics Temporarycharacteristics;

    Temporarycharacteristics.gender = CharacterPlayer.gender;
    Temporarycharacteristics.personalitytrait=CharacterPlayer.personalitytrait;

    return Temporarycharacteristics;
}


/// Get player alliance
playeralliance Player::GetAlliance(void)
{
    /// Charcter specs
    playeralliance Temporaryalliance;

    Temporaryalliance.alienrace = CharacterPlayer.alienrace;
    Temporaryalliance.alienalliancealigned=CharacterPlayer.alienalliancealigned;

    return Temporaryalliance;
}

/// Set player characteristics
int Player::SetCharacteristics(playercharacteristics TempCharacteristics)
{
    /// Set character charactistics
    CharacterPlayer.gender= TempCharacteristics.gender;
    CharacterPlayer.personalitytrait= TempCharacteristics.personalitytrait;

    return 1;
}


/// Set player alliance
int Player::SetAlliance(playeralliance TempAlliance)
{
    /// Set charcter alliance
    CharacterPlayer.alienrace=TempAlliance.alienrace;
    CharacterPlayer.alienalliancealigned=TempAlliance.alienalliancealigned;

    return 1;
}

int Player::SetPlayerInfo(playerbasicinfo TempPlayer)
{
    /// Set charcter alliance
    CharacterPlayer.firstname=TempPlayer.firstname;
    CharacterPlayer.middlename=TempPlayer.middlename;
    CharacterPlayer.lastname=TempPlayer.lastname;

    return 1;
}


playerbasicinfo Player::GetPlayerInfo(void)
{
    /// Charcter specs
    playerbasicinfo Temporarybasicinfo;

    /// Set charcter alliance
    Temporarybasicinfo.firstname = CharacterPlayer.firstname;
    Temporarybasicinfo.middlename = CharacterPlayer.middlename;
    Temporarybasicinfo.lastname = CharacterPlayer.lastname;

    return Temporarybasicinfo;
}
