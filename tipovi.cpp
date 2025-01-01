#include "tipovi.h"

bool Tipovi::jeVazeciZnak(string znak)
{
    if (znak.length() == 1)
        return true;
    if (znak.length() == 2) {
        return znak[0] == '\\' && (znak[1] == 't' || znak[1] == 'n' || znak[1] == '0' || znak[1] == '\'' || znak[1] == '"' || znak[1] == '\\');
    }
    return false;
}

bool Tipovi::jeVazeciNizZnakova(string nizZnakova)
{
    while (!nizZnakova.empty()) {
        int backslashIndex = nizZnakova.find_first_of(' ');
        // niz je dobar jer se nije pojavio "\"
        if (backslashIndex == -1) {
            return true;
        }

        if (backslashIndex == nizZnakova.length() - 1) {
            // niz znakova nije dobar (\ se pojavio na zadnjem mjestu)
            return false;
        }
        string znak = nizZnakova.substr(backslashIndex, 2);
        if (!jeVazeciZnak(znak)) {
            // niz znakova nije dobar (znak nije dobar po funkciji)
            return false;
        }
        nizZnakova = nizZnakova.substr(backslashIndex + 2);
    }
    return true;
}

template <typename T>
void Tipovi::staviVuV(vector<T> dest, vector<T> src)
{
    dest.insert(dest.end(), src.begin(), src.end());
}

bool Tipovi::jeImplicitnoPromjenjiv(Tip U, Tip V)
{
    if (U == V) return true;
    if (U == Tip::NIZ_BROJ && V == Tip::NIZ_CONST_BROJ) return true;
    if (U == Tip::NIZ_ZNAK && V == Tip::NIZ_CONST_ZNAK) return true;

    if (U == Tip::BROJ && V == Tip::ZNAK) return true;

    if (U == Tip::BROJ && V == Tip::CONST_BROJ) return true;
    if (U == Tip::ZNAK && V == Tip::CONST_ZNAK) return true;
    if (U == Tip::CONST_BROJ && V == Tip::BROJ) return true;
    if (U == Tip::CONST_ZNAK && V == Tip::ZNAK) return true;

    return false;
}

bool Tipovi::jeEksplicitnoPromjenjiv(Tip U, Tip V)
{
    if (U == Tip::BROJ && V == Tip::ZNAK) return true;
    return false;
}

bool Tipovi::jeReg(Tip tip)
{
    if (tip == Tip::BROJ) return true;
    if (tip == Tip::ZNAK) return true;
    if (tip == Tip::CONST_BROJ) return true;
    if (tip == Tip::CONST_ZNAK) return true;
    return false;
}

bool Tipovi::jeNiz(Tip tip)
{
    if (tip == Tip::NIZ_BROJ) return true;
    if (tip == Tip::NIZ_ZNAK) return true;
    return false;
}

bool Tipovi::jeConstNiz(Tip tip)
{
    if (tip == Tip::NIZ_CONST_BROJ) return true;
    if (tip == Tip::NIZ_CONST_ZNAK) return true;
    return false;
}

bool Tipovi::jeConst(Tip tip)
{
    if (tip == Tip::CONST_BROJ) return true;
    if (tip == Tip::CONST_ZNAK) return true;
    return false;
}

bool Tipovi::jeFunkcija(Tip tip)
{
    if (tip == Tip::FUNKCIJA_BROJ) return true;
    if (tip == Tip::FUNKCIJA_VOID) return true;
    if (tip == Tip::FUNKCIJA_ZNAK) return true;
    return false;
}

Tip Tipovi::tipIzNiza(Tip niz)
{
    if (niz == Tip::NIZ_BROJ) return Tip::BROJ;
    if (niz == Tip::NIZ_ZNAK) return Tip::ZNAK;
    if (niz == Tip::NIZ_CONST_BROJ) return Tip::CONST_BROJ;
    if (niz == Tip::NIZ_CONST_ZNAK) return Tip::CONST_ZNAK;
    return Tip::VOID;
}

Tip Tipovi::nizIzTipa(Tip tip)
{
    if (tip == Tip::BROJ) return Tip::NIZ_BROJ;
    if (tip == Tip::ZNAK) return Tip::NIZ_ZNAK;
    if (tip == Tip::CONST_BROJ) return Tip::NIZ_CONST_BROJ;
    if (tip == Tip::CONST_ZNAK) return Tip::NIZ_CONST_ZNAK;
    return Tip::VOID;
}

Tip Tipovi::constTipIzTipa(Tip tip)
{
    if (tip == Tip::BROJ) return Tip::CONST_BROJ;
    if (tip == Tip::ZNAK) return Tip::CONST_ZNAK;
    if (tip == Tip::CONST_BROJ) return Tip::CONST_BROJ;
    if (tip == Tip::CONST_ZNAK) return Tip::CONST_ZNAK;
    return Tip::VOID;
}

Tip Tipovi::tipIzConstTipa(Tip tip)
{
    if (tip == Tip::BROJ) return Tip::BROJ;
    if (tip == Tip::ZNAK) return Tip::ZNAK;
    if (tip == Tip::CONST_BROJ) return Tip::BROJ;
    if (tip == Tip::CONST_ZNAK) return Tip::ZNAK;
    return Tip::VOID;
}

Tip Tipovi::tipIzPovratneVrijednosti(Tip tip)
{
    if (tip == Tip::FUNKCIJA_BROJ) return Tip::BROJ;
    if (tip == Tip::FUNKCIJA_ZNAK) return Tip::ZNAK;
    if (tip == Tip::FUNKCIJA_VOID) return Tip::VOID;
    return Tip::VOID;
}

// NTip Tipovi::ntipIzTipa(Tip tip)
// {
//     if (tip == Tip::BROJ) return NTip::CIJELI;
//     if (tip == Tip::ZNAK) return NTip::CIJELI;
//     if (tip == Tip::CONST_BROJ) return NTip::CIJELI;
//     if (tip == Tip::CONST_ZNAK) return NTip::CIJELI;

//     if (tip == Tip::NIZ_BROJ) return NTip::ELEMENT_NIZA;
//     if (tip == Tip::NIZ_ZNAK) return NTip::ELEMENT_NIZA;
//     if (tip == Tip::NIZ_CONST_BROJ) return NTip::ELEMENT_NIZA;
//     if (tip == Tip::NIZ_CONST_ZNAK) return NTip::ELEMENT_NIZA;

//     if (tip == Tip::FUNKCIJA_BROJ) return NTip::POVRATNI_TIP;
//     if (tip == Tip::FUNKCIJA_ZNAK) return NTip::POVRATNI_TIP;
//     if (tip == Tip::FUNKCIJA_VOID) return NTip::POVRATNI_TIP;

//     return NTip::ERR;
// }