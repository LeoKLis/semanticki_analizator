#ifndef TIPOVI_H
#define TIPOVI_H

#include <string>
#include <vector>

using namespace std;

enum class Tip {
    // IDN,
    VOID,

    BROJ,
    ZNAK,
    CONST_BROJ,
    CONST_ZNAK,

    NIZ_BROJ,
    NIZ_ZNAK,
    NIZ_CONST_BROJ,
    NIZ_CONST_ZNAK,

    FUNKCIJA_BROJ,
    FUNKCIJA_ZNAK,
    FUNKCIJA_VOID
};

// enum class NTip {
//     CIJELI,
//     ELEMENT_NIZA,
//     POVRATNI_TIP,
//     ERR
// };

class Tipovi {
public:
    // Provjera po 4.3.2 sekciji u uputama
    static bool jeVazeciZnak(string znak);

    // Provjera po 4.3.2 sekciji u uputama
    static bool jeVazeciNizZnakova(string nizZnakova);

    // Stavlja vektor tipova u vektor tipova
    template <typename T>
    static void staviVuV(vector<T> dest, vector<T> src);

    // Provjera po 4.3.1 Implicitne promjene tipa
    static bool jeImplicitnoPromjenjiv(Tip U, Tip V);

    // Provjera po 4.3.1 Eksplicitne promjene tipa
    static bool jeEksplicitnoPromjenjiv(Tip U, Tip V);

    static bool jeReg(Tip tip);

    // Provjera je li tip podatka niz
    static bool jeNiz(Tip tip);

    // Provjera je li tip podatka const niz
    static bool jeConstNiz(Tip tip);

    // Provjera je li tip podatka konstanta
    static bool jeConst(Tip tip);

    // Pretvara niz(X) u X
    static Tip tipIzNiza(Tip niz);

    // Pretvara X u niz(X)
    static Tip nizIzTipa(Tip tip);

    // Pretvara T u const(T)
    static Tip constTipIzTipa(Tip tip);

    static Tip tipIzConstTipa(Tip tip);

    // Provjera je li tip funkcija
    static bool jeFunkcija(Tip tip);

    // Pretvara funkcija(params, povratna_vrijednost) u povratna_vrijednost
    static Tip tipIzPovratneVrijednosti(Tip tip);

    // static NTip ntipIzTipa(Tip tip);
};

#endif