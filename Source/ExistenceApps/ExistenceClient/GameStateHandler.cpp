#include "GameStateHandler.h"


using namespace std;

Gamestatehandler::Gamestatehandler()
{
    return; //ctor
}

Gamestatehandler::~Gamestatehandler()
{
    return;
    //dtor
}

void Gamestatehandler::Start(void)
{

    // set initialize of state
    gamestate=STATE_NONE;
    uistate=UI_NONE;

    consolestate=UI_CONSOLEOFF;
    cameramode=CAMERAMODE_DEFAULT;

    debughud=false;


    return;
}

bool Gamestatehandler::GetConsoleState(void)
{
    bool flag;

    flag = consolestate;

    return flag;
}

bool Gamestatehandler::SetConsoleState(int flag)
{

    consolestate = flag;

    return 1;
}

int Gamestatehandler::GetUIState(void)
{
    int flag;

    flag = uistate;

    return flag;
}

int Gamestatehandler::SetUIState(int flag)
{

    uistate = flag;

    return 1;
}


int Gamestatehandler::GetGameState(void)
{
    int flag;

    flag = gamestate;

    return flag;
}

int Gamestatehandler::SetGameState(int flag)
{

    gamestate = flag;

    return 1;
}


int Gamestatehandler::GetCameraMode(void)
{
    int flag;

    flag = cameramode;

    return flag;;
}

int Gamestatehandler::SetCameraMode(int flag)
{

    cameramode = flag;

    return 1;
}



int Gamestatehandler::GetDebugHudMode(void)
{
    int flag;

    flag = debughud;

    return flag;;
}

int Gamestatehandler::SetDebugHudMode(int flag)
{

    debughud = flag;

    return 1;
}
