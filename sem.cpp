#include "sem.h"

Sem::Sem(Tree tree)
{
    this->tree = tree;
}

void Sem::simulate()
{
    tree.next(); // <prijevodna_jedinica>
    prijevodna_jedinica();
}

bool Sem::prijevodna_jedinica()
{
}

bool Sem::vanjska_deklaracija()
{
}

bool Sem::primarni_izraz(Tip* tip, bool* lIzraz)
{
    Node* node = tree.next(); // idn, broj, znak, niz_znakova
    if (node->tip == "IDN") {
        // kod za provjeru idn u tablici

    } else if (node->tip == "BROJ") {
        // kod za provjeru i postavljanje broja
        try {
            int value = stoi(node->value);
        } catch (const out_of_range& oor) {
            cerr << "Out of range error: " << oor.what() << endl;
            cerr << "For value: " << node->value << endl;
            // Ispis greske
            return false;
        }
        *tip = BROJ;
        *lIzraz = 0;

    } else if (node->tip == "ZNAK") {
        // kod za provjeru i postavljanje znaka
        string znak = node->value;
        if (!jeVazeciZnak(znak)) {
            // Ispis greske
            return false;
        }
        *tip = ZNAK;
        *lIzraz = 0;

    } else if (node->tip == "NIZ_ZNAKOVA") {
        string nizZnakova = node->value;
        if (!jeVazeciNizZnakova(nizZnakova)) {
            // Ispis greske
            return false;
        }
        *tip = NIZ_ZNAK;
        *lIzraz = 0;

    } else if (node->tip == "L_ZAGRADA") {
        node = tree.next(); // <izraz>
        if (!izraz(tip, lIzraz))
            return false; // Provjeri izraz
        node = tree.next(); // D_ZAGRADA

    } else {
        // Ispis greske
        return false;
    }
    return true;
}

bool Sem::izraz(Tip* tip, bool* lIzraz)
{
    Node* node = tree.next();
    if (node->tip == "<izraz_pridruzivanja>") {
        node = tree.next();
        izraz_pridruzivanja(tip, lIzraz);
        // Provjera izraza pridruzivanja...
    } else if (node->tip == "<izraz>") {
        node = tree.next();
        izraz(tip, lIzraz);
    } else {
        // Prijavi gresku
    }
}

bool Sem::izraz_pridruzivanja(Tip* tip, bool* lIzraz)
{
    Node* node = tree.next();
    if (node->tip == "<log_ili_izraz>") {
        node = tree.next();
        log_ili_izraz(tip, lIzraz);
        // Provjera izraza
    } else if (node->tip == "<postfiks_izraz>") {
        node = tree.next();
        postfiks_izraz(tip, lIzraz);
        // Provjera izraza
        if (*lIzraz == false) {
            // Prijavi gresku
            return;
        }
        node = tree.next();
        node = tree.next();
        izraz_pridruzivanja(tip, lIzraz);
        // Provjera izraza

    } else {
        // Prijavi gresku
    }
}

bool Sem::postfiks_izraz(Tip* tip, bool* lIzraz)
{
    Node* node = tree.next();
    if (node->tip == "<primarni_izraz>") {
        if (!primarni_izraz(tip, lIzraz)) // Provjeri izraz
            return false; 

    } else if (node->tip == "<postfiks_izraz>") {
        if(!postfiks_izraz(tip, lIzraz))
            return false;

        node = tree.next();
        if(node->tip == "L_UGL_ZAGRADA") {
            if(!izraz(tip, lIzraz))
                return false;
            node = tree.next();
        } else if(node->tip == "L_ZAGRADA") {
            node = tree.next();
            if(node->tip == "D_ZAGRADA"){
                return true;
            }
        }

    } else {
        // Greska...
        return false;
    }
    return true;
}

// Provjera po 4.3.2 sekciji u uputama
bool Sem::jeVazeciZnak(string znak)
{
    if (znak.length() == 1)
        return true;
    if (znak.length() == 2) {
        return znak[0] == '\\' && (znak[1] == 't' || znak[1] == 'n' || znak[1] == '0' || znak[1] == '\'' || znak[1] == '"' || znak[1] == '\\');
    }
    return false;
}

// Provjera po 4.3.2 sekciji u uputama
bool Sem::jeVazeciNizZnakova(string nizZnakova)
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