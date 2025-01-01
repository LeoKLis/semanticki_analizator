#include "sem.h"

Sem::Sem(Tree tree)
{
    this->tree = tree;
}

void Sem::simulate()
{
    Node* inode = tree.next(); // <prijevodna_jedinica>
    prijevodna_jedinica(inode);
}

// ======= Izrazi =======

bool Sem::primarni_izraz(Node* primIzrazNode)
{
    Node* inode = tree.next(); // idn, broj, znak ili niz_znakova
    if (inode->nodeName == "IDN") {
        // kod za provjeru idn u tablici
    }
    else if (inode->nodeName == "BROJ") {
        // kod za provjeru i postavljanje broja
        try {
            int value = stoi(inode->value);
        }
        catch (const out_of_range& oor) {
            cerr << "Out of range error: " << oor.what() << endl;
            cerr << "For value: " << inode->value << endl;
            // Ispis greske
            return false;
        }
        primIzrazNode->tip = Tip::BROJ;
        primIzrazNode->lIzraz = 0;
    }
    else if (inode->nodeName == "ZNAK") {
        // kod za provjeru i postavljanje znaka
        string znak = inode->value;
        if (!Tipovi::jeVazeciZnak(znak)) {
            // Ispis greske
            return false;
        }
        primIzrazNode->tip = Tip::ZNAK;
        primIzrazNode->lIzraz = 0;
    }
    else if (inode->nodeName == "NIZ_ZNAKOVA") {
        string nizZnakova = inode->value;
        if (!Tipovi::jeVazeciNizZnakova(nizZnakova)) {
            // Ispis greske
            return false;
        }
        primIzrazNode->tip = Tip::NIZ_ZNAK;
        primIzrazNode->lIzraz = 0;
    }
    else if (inode->nodeName == "L_ZAGRADA") {
        inode = tree.next(); // <izraz>
        if (!izraz(inode)) return false; // Provjeri izraz
        primIzrazNode->tip = inode->tip; // tip <- izraz.tip
        primIzrazNode->lIzraz = inode->lIzraz; // lizraz <- izraz.lizraz
        tree.next(); // D_ZAGRADA
    }
    else {
        // Ispis greske
        return false;
    }
    return true;
}

bool Sem::postfiks_izraz(Node* posIzrazNode)
{
    Node* inode = tree.next();
    if (inode->nodeName == "<primarni_izraz>") {
        if (!primarni_izraz(inode)) return false;
        posIzrazNode->tip = inode->tip;
        posIzrazNode->lIzraz = inode->lIzraz;
    }
    else if (inode->nodeName == "<postfiks_izraz>") {
        if (!postfiks_izraz(inode)) return false; // Provjeri izraz

        Node* postfiksNode = tree.get(); // <postfiks_izraz>
        inode = tree.next();
        if (inode->nodeName == "L_UGL_ZAGRADA") {
            if (!Tipovi::jeNiz(inode->tip)) {
                // Prijavi gresku
                return false;
            }
            inode = tree.next(); // <izraz>
            if (!izraz(inode)) return false;
            if (!Tipovi::jeImplicitnoPromjenjiv(inode->tip, Tip::BROJ)) {
                // Prijavi gresku
                return false;
            }
            inode = tree.next(); // D_UGL_ZAGRADA

            Tip tip = Tipovi::tipIzNiza(postfiksNode->tip);
            posIzrazNode->tip = tip;
            posIzrazNode->lIzraz = (tip != Tipovi::constTipIzTipa(tip));
        }
        else if (inode->nodeName == "L_ZAGRADA") {
            inode = tree.next();
            if (inode->nodeName == "D_ZAGRADA") {
                if (!Tipovi::jeFunkcija(inode->tip) || !inode->tipovi.empty()) {
                    // Prijavi gresku
                    return false;
                }
                posIzrazNode->tip = Tipovi::tipIzPovratneVrijednosti(inode->tip);
                posIzrazNode->lIzraz = 0;
            }
            else if (inode->nodeName == "<lista_argumenata>") {
                if (!lista_argumenata(inode)) return false;
                if (!Tipovi::jeFunkcija || inode->tipovi.empty()) {
                    // Prijavi gresku
                    return false;
                }
                if (inode->tipovi.size() != postfiksNode->tipovi.size()) {
                    // Prijavi gresku
                    return false;
                }
                for (int i = 0; i < inode->tipovi.size(); i++) {
                    if (!Tipovi::jeImplicitnoPromjenjiv(inode->tipovi[i], postfiksNode->tipovi[i])) {
                        // Prijavi gresku
                        return false;
                    }
                }
                posIzrazNode->tip = Tipovi::tipIzPovratneVrijednosti(inode->tip);
                posIzrazNode->lIzraz = 0;
                tree.next(); // D_ZAGRADA
            }
        }
        else if (inode->nodeName == "OP_INC" || inode->nodeName == "OP_DEC") {
            if (postfiksNode->lIzraz != 1 || !Tipovi::jeImplicitnoPromjenjiv(postfiksNode->tip, Tip::BROJ)) {
                // Prijavi gresku
                return false;
            }
        }
    }
    else {
        // Teska greska...
        return false;
    }
    return true;
}

bool Sem::lista_argumenata(Node* listArgNode)
{
    Node* inode = tree.next();
    if (inode->nodeName == "<izraz_pridruzivanja>") {
        if (!izraz_pridruzivanja(inode)) return false;

        listArgNode->tipovi.push_back(inode->tip);
    }
    else if (inode->nodeName == "<lista_argumenata>") {
        Node* liarNode = inode;
        if (!lista_argumenata(inode)) return false;
        tree.next(); // ZAREZ
        inode = tree.next(); // <izraz_pridruzivanja>
        if (!izraz_pridruzivanja(inode)) return false;

        listArgNode->tipovi.insert(listArgNode->tipovi.end(), liarNode->tipovi.begin(), liarNode->tipovi.end());
        listArgNode->tipovi.push_back(inode->tip);
    }
    else {
        // Teska greska...
        return false;
    }
    return true;
}

bool Sem::unarni_izraz(Node* unarOpNode)
{
    Node* inode = tree.next();
    if (inode->nodeName == "<postfiks_izraz>") {
        if (!postfiks_izraz(inode)) return false;

        unarOpNode->tip = inode->tip;
        unarOpNode->lIzraz = inode->lIzraz;
    }
    else if (inode->nodeName == "OP_INC" || inode->nodeName == "OP_DEC") {
        inode = tree.next(); // <unarni_izraz>
        if (!unarni_izraz(inode)) return false;
        if (inode->lIzraz != 1 || !Tipovi::jeImplicitnoPromjenjiv(inode->tip, Tip::BROJ)) {
            // Prijavi gresku
            return false;
        }

        unarOpNode->tip = Tip::BROJ;
        unarOpNode->lIzraz = 0;
    }
    else if (inode->nodeName == "<unarni_operator>") {
        inode = tree.next(); // <cast_izraz>
        if (!cast_izraz(inode)) return false;
        if (!Tipovi::jeImplicitnoPromjenjiv(inode->tip, Tip::BROJ)) {
            // Prijavi gresku
            return false;
        }

        unarOpNode->tip = Tip::BROJ;
        unarOpNode->lIzraz = 0;
    }
    else {
        // Teska greska...
        return false;
    }
    return true;
}

bool Sem::cast_izraz(Node* castIzrazNode)
{
    Node* inode = tree.next();
    if (inode->nodeName == "<unarni_izraz>") {
        if (!unarni_izraz(inode)) return false;

        castIzrazNode->tip = inode->tip;
        castIzrazNode->lIzraz = inode->lIzraz;
    }
    else if (inode->nodeName == "L_ZAGRADA") {
        inode = tree.next();
        Node* imeTipaNode = inode;
        if (!ime_tipa(inode)) return false;
        tree.next();
        inode = tree.next();
        if (!cast_izraz(inode)) return false;
        if (!Tipovi::jeEksplicitnoPromjenjiv(inode->tip, imeTipaNode->tip) || !Tipovi::jeImplicitnoPromjenjiv(inode->tip, imeTipaNode->tip)) {
            // Prijavi gresku
            return false;
        }

        castIzrazNode->tip = imeTipaNode->tip;
        castIzrazNode->lIzraz = 0;
    }
    else {
        // Teska greska...
        return false;
    }
    return true;
}

bool Sem::ime_tipa(Node* imeTipaNode)
{
    Node* inode = tree.next();
    if (inode->nodeName == "<specifikator_tipa>") {
        if (!specifikator_tipa(inode)) return false;

        imeTipaNode->tip = inode->tip;
    }
    else if (inode->nodeName == "KR_CONST") {
        inode = tree.next();
        if (!specifikator_tipa(inode)) return false;
        if (inode->tip == Tip::VOID) {
            // Prijavi gresku
            return false;
        }

        imeTipaNode->tip = Tipovi::constTipIzTipa(inode->tip);
    }
    else {
        // Teska greska
        return false;
    }
    return true;
}

bool Sem::specifikator_tipa(Node* specTipaNode)
{
    Node* inode = tree.next();
    if (inode->nodeName == "KR_VOID") {
        specTipaNode->tip = Tip::VOID;
    }
    else if (inode->nodeName == "KR_CHAR") {
        specTipaNode->tip = Tip::ZNAK;
    }
    else if (inode->nodeName == "KR_INT") {
        specTipaNode->tip = Tip::BROJ;
    }
    else {
        // Teska greska
        return false;
    }
    return true;
}

bool Sem::multiplikativni_izraz(Node* mulIzrazNode)
{
    Node* inode = tree.next();
    if (inode->nodeName == "<cast_izraz>") {
        if (!cast_izraz(inode)) return false;

        mulIzrazNode->tip = inode->tip;
        mulIzrazNode->lIzraz = inode->lIzraz;
    }
    else if (inode->nodeName == "<multiplikativni_izraz>") {
        if (!multiplikativni_izraz(inode)) return false;
        if (!Tipovi::jeImplicitnoPromjenjiv(inode->tip, Tip::BROJ)) {
            // Prijavi gresku
            return false;
        }
        tree.next();
        inode = tree.next();
        if (!cast_izraz(inode)) return false;
        if (!Tipovi::jeImplicitnoPromjenjiv(inode->tip, Tip::BROJ)) {
            // Prijavi gresku
            return false;
        }

        mulIzrazNode->tip = Tip::BROJ;
        mulIzrazNode->lIzraz = 0;
    }
    else {
        // Teska greska
        return false;
    }
    return true;
}

bool Sem::aditivni_izraz(Node* aditIzrazNode)
{
    Node* inode = tree.next();
    if (inode->nodeName == "<multiplikativni_izraz>") {
        if (!multiplikativni_izraz(inode)) return false;

        aditIzrazNode->tip = inode->tip;
        aditIzrazNode->lIzraz = inode->lIzraz;
    }
    else if (inode->nodeName == "<aditivni_izraz>") {
        if (!aditivni_izraz(inode)) return false;
        if (!Tipovi::jeImplicitnoPromjenjiv(inode->tip, Tip::BROJ)) {
            // Prijavi gresku
            return false;
        }
        tree.next();
        inode = tree.next();
        if (!multiplikativni_izraz(inode)) return false;
        if (!Tipovi::jeImplicitnoPromjenjiv(inode->tip, Tip::BROJ)) {
            // Prijavi gresku
            return false;
        }

        aditIzrazNode->tip = Tip::BROJ;
        aditIzrazNode->lIzraz = 0;
    }
    else {
        // Teska greska
        return false;
    }
    return true;
}

bool Sem::odnosni_izraz(Node* odnIzrazNode)
{
    Node* inode = tree.next();
    if (inode->nodeName == "<aditivni_izraz>") {
        if (!aditivni_izraz(inode)) return false;

        odnIzrazNode->tip = inode->tip;
        odnIzrazNode->lIzraz = inode->lIzraz;
    }
    else if (inode->nodeName == "<odnosni_izraz>") {
        if (!odnosni_izraz(inode)) return false;
        if (!Tipovi::jeImplicitnoPromjenjiv(inode->tip, Tip::BROJ)) {
            // Prijavi gresku
            return false;
        }
        tree.next();
        inode = tree.next();
        if (!aditivni_izraz(inode)) return false;
        if (!Tipovi::jeImplicitnoPromjenjiv(inode->tip, Tip::BROJ)) {
            // Prijavi gresku
            return false;
        }

        odnIzrazNode->tip = Tip::BROJ;
        odnIzrazNode->lIzraz = 0;
    }
    else {
        // Teska greska
        return false;
    }
    return true;
}

bool Sem::jednakosni_izraz(Node* jednIzrazNode)
{
    Node* inode = tree.next();
    if (inode->nodeName == "<odnosni_izraz>") {
        if (!odnosni_izraz(inode)) return false;

        jednIzrazNode->tip = inode->tip;
        jednIzrazNode->lIzraz = inode->lIzraz;
    }
    else if (inode->nodeName == "<jednakosni_izraz>") {
        if (!jednakosni_izraz(inode)) return false;
        if (!Tipovi::jeImplicitnoPromjenjiv(inode->tip, Tip::BROJ)) {
            // Prijavi gresku
            return false;
        }
        tree.next();
        inode = tree.next();
        if (!odnosni_izraz(inode)) return false;
        if (!Tipovi::jeImplicitnoPromjenjiv(inode->tip, Tip::BROJ)) {
            // Prijavi gresku
            return false;
        }

        jednIzrazNode->tip = Tip::BROJ;
        jednIzrazNode->lIzraz = 0;
    }
}

bool Sem::bin_i_izraz(Node* binNode)
{
    Node* inode = tree.next();
    if (inode->nodeName == "<jednakosni_izraz>") {
        if (!odnosni_izraz(inode)) return false;

        binNode->tip = inode->tip;
        binNode->lIzraz = inode->lIzraz;
    }
    else if (inode->nodeName == "<bin_i_izraz>") {
        if (!bin_i_izraz(inode)) return false;
        if (!Tipovi::jeImplicitnoPromjenjiv(inode->tip, Tip::BROJ)) {
            // Prijavi gresku
            return false;
        }
        tree.next();
        inode = tree.next();
        if (!jednakosni_izraz(inode)) return false;
        if (!Tipovi::jeImplicitnoPromjenjiv(inode->tip, Tip::BROJ)) {
            // Prijavi gresku
            return false;
        }

        binNode->tip = Tip::BROJ;
        binNode->lIzraz = 0;
    }
}

bool Sem::bin_xili_izraz(Node* binXiliNode)
{
    Node* inode = tree.next();
    if (inode->nodeName == "<bin_i_izraz>") {
        if (!bin_i_izraz(inode)) return false;

        binXiliNode->tip = inode->tip;
        binXiliNode->lIzraz = inode->lIzraz;
    }
    else if (inode->nodeName == "<bin_xili_izraz>") {
        if (!bin_xili_izraz(inode)) return false;
        if (!Tipovi::jeImplicitnoPromjenjiv(inode->tip, Tip::BROJ)) {
            // Prijavi gresku
            return false;
        }
        tree.next();
        inode = tree.next();
        if (!bin_i_izraz(inode)) return false;
        if (!Tipovi::jeImplicitnoPromjenjiv(inode->tip, Tip::BROJ)) {
            // Prijavi gresku
            return false;
        }

        binXiliNode->tip = Tip::BROJ;
        binXiliNode->lIzraz = 0;
    }
}

bool Sem::bin_ili_izraz(Node* binIliNode)
{
    Node* inode = tree.next();
    if (inode->nodeName == "<bin_xili_izraz>") {
        if (!bin_xili_izraz(inode)) return false;

        binIliNode->tip = inode->tip;
        binIliNode->lIzraz = inode->lIzraz;
    }
    else if (inode->nodeName == "<bin_ili_izraz>") {
        if (!bin_ili_izraz(inode)) return false;
        if (!Tipovi::jeImplicitnoPromjenjiv(inode->tip, Tip::BROJ)) {
            // Prijavi gresku
            return false;
        }
        tree.next();
        inode = tree.next();
        if (!bin_xili_izraz(inode)) return false;
        if (!Tipovi::jeImplicitnoPromjenjiv(inode->tip, Tip::BROJ)) {
            // Prijavi gresku
            return false;
        }

        binIliNode->tip = Tip::BROJ;
        binIliNode->lIzraz = 0;
    }
}

bool Sem::log_i_izraz(Node* logINode)
{
    Node* inode = tree.next();
    if (inode->nodeName == "<bin_ili_izraz>") {
        if (!bin_ili_izraz(inode)) return false;

        logINode->tip = inode->tip;
        logINode->lIzraz = inode->lIzraz;
    }
    else if (inode->nodeName == "<log_i_izraz>") {
        if (!log_i_izraz(inode)) return false;
        if (!Tipovi::jeImplicitnoPromjenjiv(inode->tip, Tip::BROJ)) {
            // Prijavi gresku
            return false;
        }
        tree.next();
        inode = tree.next();
        if (!bin_ili_izraz(inode)) return false;
        if (!Tipovi::jeImplicitnoPromjenjiv(inode->tip, Tip::BROJ)) {
            // Prijavi gresku
            return false;
        }

        logINode->tip = Tip::BROJ;
        logINode->lIzraz = 0;
    }
}

bool Sem::log_ili_izraz(Node* logIliNode)
{
    Node* inode = tree.next();
    if (inode->nodeName == "<log_i_izraz>") {
        if (!log_ili_izraz(inode)) return false;

        logIliNode->tip = inode->tip;
        logIliNode->lIzraz = inode->lIzraz;
    }
    else if (inode->nodeName == "<log_ili_izraz>") {
        if (!log_ili_izraz(inode)) return false;
        if (!Tipovi::jeImplicitnoPromjenjiv(inode->tip, Tip::BROJ)) {
            // Prijavi gresku
            return false;
        }
        tree.next();
        inode = tree.next();
        if (!log_i_izraz(inode)) return false;
        if (!Tipovi::jeImplicitnoPromjenjiv(inode->tip, Tip::BROJ)) {
            // Prijavi gresku
            return false;
        }

        logIliNode->tip = Tip::BROJ;
        logIliNode->lIzraz = 0;
    }
}

bool Sem::izraz_pridruzivanja(Node* izrazPridNode)
{
    Node* inode = tree.next();
    if (inode->nodeName == "<log_ili_izraz>") {
        if (!log_ili_izraz(inode)) return false;
        izrazPridNode->tip = inode->tip;
        izrazPridNode->lIzraz = inode->lIzraz;
    }
    else if (inode->nodeName == "<postfiks_izraz>") {
        Node* posIzrazNode = tree.get(); // <postfiks_izraz>
        if (!postfiks_izraz(inode)) return false;
        if (inode->lIzraz == 0) {
            // Prijavi gresku
            return false;
        }
        tree.next(); // OP_PRIDRUZI
        inode = tree.next(); // izraz_pridruzivanja
        if (!izraz_pridruzivanja(inode)) return false;
        if (!Tipovi::jeImplicitnoPromjenjiv(inode->tip, posIzrazNode->tip)) {
            // Prijavi gresku
            return false;
        }
        izrazPridNode->tip = posIzrazNode->tip;
        izrazPridNode->lIzraz = posIzrazNode->lIzraz;
    }
    else {
        // Prijavi gresku
        return false;
    }
    return true;
}

bool Sem::izraz(Node* izrazNode)
{
    Node* inode = tree.next();
    if (inode->nodeName == "<izraz_pridruzivanja>") {
        if (!izraz_pridruzivanja(inode)) return false;
        izrazNode->tip = inode->tip;
        izrazNode->lIzraz = inode->lIzraz;
    }
    else if (inode->nodeName == "<izraz>") {
        if (!izraz(inode)) return false; // Provjeri izraz
        tree.next(); // ZAREZ
        inode = tree.next(); // izraz_pridruzivanja
        if (!izraz_pridruzivanja(inode)) return false; // Provjeri izraz
        izrazNode->tip = inode->tip;
        izrazNode->lIzraz = 0;
    }
    else {
        // Prijavi gresku
        return false;
    }
    return true;
}

// ====== Naredbena struktura programa ======

bool Sem::slozena_naredba(Node* slozNaredbaNode)
{
    tree.next(); // L_VIT_ZAGRADA
    Node* inode = tree.next();
    if (inode->nodeName == "<lista_naredbi>") {
        if (!lista_naredbi(inode)) return false;
        tree.next();
    }
    else if (inode->nodeName == "<lista_deklaracija>") {
        if (!lista_deklaracija(inode)) return false;
        inode = tree.next();
        if (!lista_naredbi(inode)) return false;
        tree.next();
    }
    else {
        // Teska greska
        return false;
    }
    return true;
}

bool Sem::lista_naredbi(Node* listaNaredbiNode)
{
    Node* inode = tree.next();
    if (inode->nodeName == "<naredba>") {
        if (!naredba(inode)) return false;
    }
    else if (inode->nodeName == "<lista_naredbi>") {
        if (!lista_naredbi(inode)) return false;
        inode = tree.next();
        if (!naredba(inode)) return false;
    }
    else {
        // Teska greska
        return false;
    }
    return true;
}

bool Sem::naredba(Node* naredbaNode)
{
    Node* inode = tree.next();
    if (inode->nodeName == "<slozena_naredba>") {
        if (!slozena_naredba(inode)) return false;
    }
    else if (inode->nodeName == "<izraz_naredba>") {
        if (!izraz_naredba(inode)) return false;
    }
    else if (inode->nodeName == "<naredba_grananja>") {
        if (!naredba_grananja(inode)) return false;
    }
    else if (inode->nodeName == "<naredba_petlje>") {
        if (!naredba_petlje(inode)) return false;
    }
    else if (inode->nodeName == "<naredba_skoka>") {
        if (!naredba_skoka(inode)) return false;
    }
    else {
        // Teska greska
        return false;
    }
    return true;
}

bool Sem::izraz_naredba(Node* izrazNaredbaNode)
{
    Node* inode = tree.next();
    if (inode->nodeName == "TOCKAZAREZ") {
        izrazNaredbaNode->tip = Tip::BROJ;
    }
    else if (inode->nodeName == "<izraz>") {
        if (!izraz(inode)) return false;
        izrazNaredbaNode->tip = inode->tip;
    }
    else {
        // Teska greska
        return false;
    }
    return true;
}

bool Sem::naredba_grananja(Node* narGrananjaNode)
{
    tree.next(); // KR_IF
    tree.next(); // L_ZAGRADA
    Node* inode = tree.next();
    if (!izraz(inode)) return false;
    if (!Tipovi::jeImplicitnoPromjenjiv(inode->tip, Tip::BROJ)) {
        // Prijavi gresku
        return false;
    }
    tree.next(); // D_ZAGRADA
    inode = tree.next();
    if (!naredba(inode)) return false;
    if (tree.peek()->nodeName == "KR_ELSE") {
        tree.next(); // KR_ELSE
        inode = tree.next();
        if (!naredba(inode)) return false;
    }
    return true;
}

bool Sem::naredba_petlje(Node* narPetljeNode)
{
    Node* inode = tree.next();
    if (inode->nodeName == "KR_WHILE") {
        tree.next();
        inode = tree.next();
        if (!izraz(inode)) return false;
        if (!Tipovi::jeImplicitnoPromjenjiv(inode->tip, Tip::BROJ)) {
            // Prijavi gresku
            return false;
        }
        tree.next();
        inode = tree.next();
        if (!naredba(inode)) return false;
    }
    else if (inode->nodeName == "KR_FOR") {
        tree.next();
        inode = tree.next();
        if (!izraz_naredba(inode)) return false;
        inode = tree.next();
        if (!izraz_naredba(inode)) return false;
        if (!Tipovi::jeImplicitnoPromjenjiv(inode->tip, Tip::BROJ)) {
            // Prijavi gresku
            return false;
        }
        inode = tree.next();
        if (inode->nodeName == "D_ZAGRADA") {
            inode = tree.next();
            if (!naredba(inode)) return false;
        }
        else if (inode->nodeName == "<izraz>") {
            if (!izraz(inode)) return false;
            tree.next();
            inode = tree.next();
            if (!naredba(inode)) return false;
        }
        else {
            // Teska greska
            return false;
        }
    }
    else {
        // Teska greska
        return false;
    }
    return true;
}

bool Sem::naredba_skoka(Node* narSkokaNode)
{
    Node* inode = tree.next();
    if (inode->nodeName == "KR_CONTINUE" || inode->nodeName == "KR_BREAK") {
        // Wow
    }
    else if (inode->nodeName == "KR_RETURN") {
        inode = tree.next();
        if (inode->nodeName == "TOCKAZAREZ") {
            // Wow
        }
        else if (inode->nodeName == "<izraz>") {
            if (!izraz(inode)) return false;
            // Wow
        }
        else {
            // Teska greska
            return false;
        }
    }
    else {
        // Teska greska
        return false;
    }
    return true;
}

bool Sem::prijevodna_jedinica(Node* prijJedNode)
{
    Node* inode = tree.next();
    if (inode->nodeName == "<vanjska_deklaracija>") {
        if (!vanjska_deklaracija(inode)) return false;
    }
    else if (inode->nodeName == "<prijevodna_jedinica>") {
        if (!prijevodna_jedinica(inode)) return false;
        inode = tree.next();
        if (!vanjska_deklaracija(inode)) return false;
    }
    else {
        // Teska greska
        return false;
    }
    return true;
}

bool Sem::vanjska_deklaracija(Node* vanjDeklNode)
{
}

// ====== Deklaracije i definicije ======

bool Sem::definicija_funkcije(Node* defFunkNode)
{
    Node* inode = tree.next();
    if (!ime_tipa(inode)) return false;
    if (Tipovi::jeConst(inode->tip)) {
        // Prijavi gresku
        return false;
    }
    inode = tree.next(); // IDN
    // Ne postoji prije definirana funkcija imena IDN.value
    tree.next();
    inode = tree.next();
    if (inode->nodeName == "KR_VOID") {
        // Zabiljezi definiciju i deklaraciju funkcije
        tree.next();
        inode = tree.next();
        if (!slozena_naredba(inode)) return false;
    }
    else if (inode->nodeName == "<lista_parametara>") {
        if (!lista_parametara(inode)) return false;
        // Zabiljezi definiciju i deklaraciju funkcije
        tree.next();
        inode = tree.next();
        if (!slozena_naredba(inode)) return false;
        // Jos neka provjera
    }
    else {
        // Teska greska
        return false;
    }
    return true;
}

bool Sem::lista_parametara(Node* lisParNode)
{
    Node* inode = tree.next();
    if (inode->nodeName == "<deklaracija_parametra>") {
        if (!deklaracija_parametra(inode)) return false;
        lisParNode->tipovi.push_back(inode->tip);
        lisParNode->imena.push_back(inode->value);
    }
    else if (inode->nodeName == "<lista_parametara>") {
        Node* lpNode = tree.get();
        if (!lista_parametara(inode)) return false;
        tree.next(); // ZAREZ
        inode = tree.next();
        if (!deklaracija_parametra(inode)) return false;
        if (find(lpNode->imena.begin(), lpNode->imena.end(), inode->value) != lpNode->imena.end()) {
            // Prijavi gresku
            return false;
        }

        Tipovi::staviVuV(lisParNode->tipovi, lpNode->tipovi);
        lisParNode->tipovi.push_back(inode->tip);

        Tipovi::staviVuV(lisParNode->imena, lpNode->imena);
        lisParNode->imena.push_back(inode->value);
    }
    else {
        // Teska greska
        return false;
    }
    return true;
}

bool Sem::deklaracija_parametra(Node* deklParNode)
{
    Node* inode = tree.next();
    Node* imeTipaNode = tree.get();
    if (!ime_tipa(inode)) return false;
    if (inode->tip == Tip::VOID) {
        // Prijavi gresku
        return false;
    }
    inode = tree.next(); // IDN
    if (tree.peek()->nodeName == "L_UGL_ZAGRADA") {
        deklParNode->tip = Tipovi::nizIzTipa(imeTipaNode->tip);
        deklParNode->value = inode->value;
        tree.next(); // L_UGL_ZAGRADA
        tree.next(); // D_UGL_ZAGRADA
    }
    else {
        deklParNode->tip = imeTipaNode->tip;
        deklParNode->value = inode->value;
    }
    return true;
}

bool Sem::lista_deklaracija(Node* lisDekNode)
{
    Node* inode = tree.next();
    if (inode->nodeName == "<deklaracija>") {
        if (!deklaracija(inode)) return false;
    }
    else if (inode->nodeName == "<lista_deklaracija>") {
        if (!lista_deklaracija(inode)) return false;
        inode = tree.next();
        if (!deklaracija(inode)) return false;
    }
    else {
        // Teska greska
        return false;
    }
    return true;
}

bool Sem::deklaracija(Node* deklNode)
{
    Node* inode = tree.next();
    Node* itNode = tree.get(); // <ime_tipa>
    if (!ime_tipa(inode)) return false;
    inode = tree.next();
    if (!lista_init_deklaratora(inode)) return false; // Dodati nasljedno svojstvo (fin)
    inode->ntip = itNode->tip; // To je valjda to
}

bool Sem::lista_init_deklaratora(Node* listInitDeklNode)
{
    Node* inode = tree.next();
    if (inode->nodeName == "<init_deklarator>") {
        if (!init_deklarator(inode)) return false; // Dodati nasljedno svojstvo
        inode->ntip = listInitDeklNode->ntip; // To je valjda to
    }
    else if (inode->nodeName == "<lista_init_deklaratora>") {
        if (!lista_init_deklaratora(inode)) return false; // Dodati nasljedno svojstvo
        inode->ntip = listInitDeklNode->ntip; // To je valjda to
        tree.next();
        inode = tree.next();
        if (!init_deklarator(inode)) return false; // Dodati nasljedno svojstvo
        inode->ntip = listInitDeklNode->ntip; // To je valjda to
    }
    else {
        // Teska greska
        return false;
    }
    return true;
}

bool Sem::init_deklarator(Node* initDeklNode)
{
    Node* inode = tree.next();
    Node* izrDeklNode = tree.get();
    if (!izravni_deklarator(inode)) return false; // Dodati nasljedno svojstvo
    inode->ntip = initDeklNode->ntip; // To je valjda to
    if (tree.peek()->nodeName == "OP_PRIDRUZI") {
        tree.next();
        inode = tree.next();
        if (!inicijalizator(inode)) return false;
        if (Tipovi::jeReg(izrDeklNode->tip)) {
            if (!Tipovi::jeImplicitnoPromjenjiv(inode->tip, Tipovi::tipIzConstTipa(izrDeklNode->tip))) {
                // Prijavi gresku
                return false;
            }
        }
        else if (Tipovi::jeNiz(izrDeklNode->tip) || Tipovi::jeConstNiz(izrDeklNode->tip)) {
            if (inode->tipovi.size() > izrDeklNode->tipovi.size()) {
                // Prijavi gresku
                return false;
            }
            for (Tip el : inode->tipovi) {
                if (!Tipovi::jeImplicitnoPromjenjiv(el, Tipovi::tipIzConstTipa(Tipovi::tipIzNiza(izrDeklNode->tip)))) {
                    // Prijavi gresku
                    return false;
                }
            }
        }
        else {
            // Prijavi gresku
            return false;
        }
    }
    else {
        if (Tipovi::jeConst(inode->tip)) {
            // Prijavi gresku
            return false;
        }
        if (Tipovi::jeConstNiz(inode->tip)) {
            // Prijavi gresku
            return false;
        }
    }
    return true;
}

bool Sem::izravni_deklarator(Node* izrDeklNode)
{
    Node* idnNode = tree.next();
    if (tree.peek()->nodeName == "L_UGL_ZAGRADA") {
        if (izrDeklNode->tip == Tip::VOID) {
            // Prijavi gresku
            return false;
        }
        // IDN.ime nije deklarirano u lokalnom djelokrugu
        tree.next(); // L_UGL_ZAGRADA
        Node* inode = tree.next(); // BROJ
        int value = stoi(inode->value);
        if (value <= 0 || value > 1024) {
            // Prijavi gresku
            return false;
        }
        // zabiljezi deklaraciju IDN.ime s odgovarajucim tipom
        izrDeklNode->tip = Tipovi::nizIzTipa(izrDeklNode->ntip); // Mozda nije taj ntip
        izrDeklNode->brElem = value;
    }
    else if (tree.peek()->nodeName == "L_ZAGRADA") {
        tree.next();
        Node* inode = tree.next();
        if (inode->nodeName == "KR_VOID") {
        }
        else if (inode->nodeName == "<lista_parametara>") {
        }
        else {
            // Teska greska
            return false;
        }
    }
    else {
        if (izrDeklNode->tip == Tip::VOID) {
            // Prijavi gresku
            return false;
        }
        // IDN.ime nije deklarirano u lokalnom djelokrugu
        // zabiljezi deklaraciju IDN.ime s odgovarajucim tipom
        izrDeklNode->tip = izrDeklNode->ntip; // Mozda nije taj ntip
    }
    return true;
}

bool Sem::inicijalizator(Node* inicNode)
{
    Node* inode = tree.next();
    if (inode->nodeName == "<izraz_pridruzivanja>") {
        if (!izraz_pridruzivanja(inode)) return false;
        if (inode->tip == Tip::NIZ_ZNAK) {
            inicNode->brElem = inode->brElem + 1; // Jer se dodaje '\0' na kraj
            inicNode->tipovi.insert(inicNode->tipovi.end(), inode->brElem, Tip::ZNAK);
        }
        else {
            inicNode->tip = inode->tip;
        }
    }
    else if (inode->nodeName == "L_VIT_ZAGRADA") {
        inode = tree.next();
        if (!lista_izraza_pridruzivanja(inode)) return false;

        inicNode->brElem = inode->brElem;
        inicNode->tipovi.insert(inicNode->tipovi.end(), inode->tipovi.begin(), inode->tipovi.end());
    }
    else {
        // Teska greska
        return false;
    }
    return true;
}

bool Sem::lista_izraza_pridruzivanja(Node* lipNode)
{
    Node* inode = tree.next();
    if (inode->nodeName == "<izraz_pridruzivanja>") {
        if (!izraz_pridruzivanja(inode)) return false;

        lipNode->tipovi.push_back(inode->tip);
        lipNode->brElem = 1;
    }
    else if (inode->nodeName == "<lista_izraza_pridruzivanja>") {
        if (!lista_izraza_pridruzivanja(inode)) return false;
        tree.next();
        inode = tree.next();
        if (!izraz_pridruzivanja(inode)) return false;

        lipNode->tipovi.insert(lipNode->tipovi.end(), inode->tipovi.begin(), inode->tipovi.end());
        lipNode->tipovi.push_back(inode->tip);
        lipNode->brElem = inode->brElem + 1;
    }
    else {
        // Teska greska
        return false;
    }
    return true;
}
