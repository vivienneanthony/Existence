#ifndef ENTITY_H_INCLUDED
#define ENTITY_H_INCLUDED

#include <iostream>
#include <string>

#include <Player.h>

#define PLAYERFILE "character.xml"


/// Define player levels
using namespace  std;

/// Create a player class
class Entity
{
public:
    Entity();
    virtual ~Entity();
    int Start(void);
    int Clear(void);

    /// Set and get the character health
    int SetHealth(int health);
    int GetHealth(void);

    /// Set and get player characteristc and alliances (Primarily for temporary usage)
    playercharacteristics GetCharacteristics(void);
    playeralliance GetAlliance(void);
    int SetCharacteristics(playercharacteristics TempCharacteristics);
    int SetAlliance(playeralliance TempAlliance);

    /// Set basic information
    int SetEntityInfo(playerbasicinfo TempEntity);
    playerbasicinfo GetEntityInfo(void);

protected:
    playerobject CharacterEntity;

private:

};



#endif // ENTITY_H_INCLUDED
