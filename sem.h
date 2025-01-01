#ifndef SEM_H
#define SEM_H

#include "tipovi.h"
#include "tree.h"

class Sem {
private:
    Tree tree;

public:
    Sem(Tree tree);
    void simulate();

    // Izrazi
    bool primarni_izraz(Node* primIzrazNode);
    bool postfiks_izraz(Node* posIzrazNode);
    bool lista_argumenata(Node* listArgNode);
    bool unarni_izraz(Node* unarOpNode);
    bool cast_izraz(Node* castIzrazNode);
    bool ime_tipa(Node* imeTipaNode);
    bool specifikator_tipa(Node* specTipaNode);
    bool multiplikativni_izraz(Node* mulIzrazNode);
    bool aditivni_izraz(Node* aditIzrazNode);
    bool odnosni_izraz(Node* odnIzrazNode);
    bool jednakosni_izraz(Node* jednIzrazNode);
    bool bin_i_izraz(Node* binNode);
    bool bin_xili_izraz(Node* binXiliNode);
    bool bin_ili_izraz(Node* binIliNode);
    bool log_i_izraz(Node* logINode);
    bool log_ili_izraz(Node* logIliNode);
    bool izraz_pridruzivanja(Node* izrazPridNode);
    bool izraz(Node* izrazNode);

    // Naredbena struktura programa
    bool slozena_naredba(Node* slozNaredbaNode);
    bool lista_naredbi(Node* listaNaredbiNode);
    bool naredba(Node* naredbaNode);
    bool izraz_naredba(Node* izrazNaredbaNode);
    bool naredba_grananja(Node* narGrananjaNode);
    bool naredba_petlje(Node* narPetljeNode);
    bool naredba_skoka(Node* narSkokaNode);
    bool prijevodna_jedinica(Node* prijJedNode);
    bool vanjska_deklaracija(Node* vanjDeklNode);

    // Deklaracije i definicije
    bool definicija_funkcije(Node* defFunkNode);
    bool lista_parametara(Node* lisParNode);
    bool deklaracija_parametra(Node* deklParNode);
    bool lista_deklaracija(Node* lisDekNode);
    bool deklaracija(Node* deklNode);
    bool lista_init_deklaratora(Node* listInitDeklNode);
    bool init_deklarator(Node* initDeklNode);
    bool izravni_deklarator(Node* izrDeklNode);
    bool inicijalizator(Node* inicNode);
    bool lista_izraza_pridruzivanja(Node* lipNode);
};

#endif