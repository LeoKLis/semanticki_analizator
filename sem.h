#ifndef SEM_H
#define SEM_H

#include "tree.h"

enum Tip {
    IDN,
    BROJ,
    ZNAK,
    NIZ_ZNAKOVA,
    NIZ_BROJ,
    NIZ_ZNAK
};

class Sem {
private:
    Tree tree;
    bool jeVazeciZnak(string znak);
    bool jeVazeciNizZnakova(string nizZnakova);

public:
    Sem(Tree tree);
    void simulate();

    bool prijevodna_jedinica();
    bool vanjska_deklaracija();
    bool primarni_izraz(Tip* tip, bool* lIzraz);
    bool izraz(Tip* tip, bool* lIzraz);
    bool izraz_pridruzivanja(Tip *tip, bool* lIzraz);
    bool log_ili_izraz(Tip* tip, bool* lIzraz);
    bool postfiks_izraz(Tip* tip, bool* lIzraz);
};

#endif