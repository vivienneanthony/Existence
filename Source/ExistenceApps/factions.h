#ifndef FACTIONS_H_INCLUDED
#define FACTIONS_H_INCLUDED

#define FACTIONSLIMIT   4
#define FACTIONSALIENS  4

// select player part
  // select player part
    struct factions
    {
        int idx;
        long int uniqueid;
        std::string name;
        std::string alliedraces;
        std::string button;
    };

    struct alienraces
    {
        int idx;
        long int uniqueid;
        std::string name;
        std::string logo;
        std::string button;
    };
;

#endif // FACTIONS_H_INCLUDED

