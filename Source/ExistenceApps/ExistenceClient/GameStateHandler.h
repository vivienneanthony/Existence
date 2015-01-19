#ifndef GAMESTATEHANDLER_H
#define GAMESTATEHANDLER_H


#define         UI_NONE                             0
#define         UI_LOGININTERFACE                   1
#define         UI_ACCOUNTCREATIONINTERFACE         2
#define         UI_CHARACTERCREATIONINTERFACE       3
#define         UI_CHARACTERSELECTIONINTERFACE      4
#define         UI_PROGRESSINTERFACE                5
#define         UI_GAMECONSOLE                      6

#define         STATE_NONE                          10
#define         STATE_MAIN                          11
#define         STATE_GAME                          12

#define         UI_CONSOLEOFF                     0
#define         UI_CONSOLEON                      1

#define         CAMERAMODE_DEFAULT                  0
#define         CAMERAMODE_FIRSTPERSON              1
#define         CAMERAMODE_FLY                      2


class Gamestatehandler
{
public:
    Gamestatehandler(void);
    virtual ~Gamestatehandler(void);
    void virtual Start(void);

    bool GetConsoleState(void);
    bool SetConsoleState(int flag);

    int GetUIState(void);
    int SetUIState(int flag);


    int GetGameState(void);
    int SetGameState(int flag);


    int GetCameraMode(void);
    int SetCameraMode(int flag);

    int GetDebugHudMode(void);
    int SetDebugHudMode(int flag);


protected:

private:

    bool consolestate;
    int gamestate;
    int uistate;
    int cameramode;
    bool debughud;

};

#endif // GAMESTATEHANDLER_H
