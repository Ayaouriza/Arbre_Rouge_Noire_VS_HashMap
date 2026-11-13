#include <iostream>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <chrono>
#include <new>
#include <unordered_map>
using namespace std;

enum Couleur { ROUGE, NOIR };

struct Noeud {
    int valeur;
    Couleur couleur;
    Noeud *gauche, *droite, *parent;

    Noeud(int v) : valeur(v), couleur(ROUGE),   // un nouveau noeud est toujours ROUGE
                   gauche(NULL), droite(NULL), parent(NULL) {}
};

class ArbreRN {
private:
    Noeud* racine;
    int taille;

    // Un NIL (pointeur NULL) est considere comme NOIR
    Couleur couleurDe(Noeud* n) {
        return (n == NULL) ? NOIR : n->couleur;
    }

    // ---------- Rotation gauche sur x : l'enfant droit y monte ----------
    void rotationGauche(Noeud* x) {
        Noeud* y = x->droite;

        x->droite = y->gauche;
        if (y->gauche != NULL)
            y->gauche->parent = x;

        y->parent = x->parent;
        if (x->parent == NULL)
            racine = y;
        else if (x == x->parent->gauche)
            x->parent->gauche = y;
        else
            x->parent->droite = y;

        y->gauche = x;
        x->parent = y;
    }

    // ---------- Rotation droite sur x : l'enfant gauche y monte ----------
    void rotationDroite(Noeud* x) {
        Noeud* y = x->gauche;

        x->gauche = y->droite;
        if (y->droite != NULL)
            y->droite->parent = x;

        y->parent = x->parent;
        if (x->parent == NULL)
            racine = y;
        else if (x == x->parent->droite)
            x->parent->droite = y;
        else
            x->parent->gauche = y;

        y->droite = x;
        x->parent = y;
    }

    // ---------- Correction apres insertion ----------
    void corrigerInsertion(Noeud* n) {
        while (n != racine && n->parent->couleur == ROUGE) {
            Noeud* p = n->parent;
            Noeud* g = p->parent;

            if (p == g->gauche) {
                Noeud* oncle = g->droite;

                if (couleurDe(oncle) == ROUGE) {
                    // Cas 2 : oncle rouge -> recoloration
                    p->couleur = NOIR;
                    oncle->couleur = NOIR;
                    g->couleur = ROUGE;
                    n = g;
                } else {
                    if (n == p->droite) {       // Cas 3 : zigzag
                        n = p;
                        rotationGauche(n);
                        p = n->parent;
                    }
                    p->couleur = NOIR;          // Cas 4 : ligne
                    g->couleur = ROUGE;
                    rotationDroite(g);
                }
            } else {
                Noeud* oncle = g->gauche;

                if (couleurDe(oncle) == ROUGE) {
                    p->couleur = NOIR;
                    oncle->couleur = NOIR;
                    g->couleur = ROUGE;
                    n = g;
                } else {
                    if (n == p->gauche) {
                        n = p;
                        rotationDroite(n);
                        p = n->parent;
                    }
                    p->couleur = NOIR;
                    g->couleur = ROUGE;
                    rotationGauche(g);
                }
            }
        }
        racine->couleur = NOIR;
    }

    // ---------- Outils pour la suppression ----------

    // Le sous-arbre v prend la place du sous-arbre u (v peut etre NULL)
    void transplanter(Noeud* u, Noeud* v) {
        if (u->parent == NULL)
            racine = v;
        else if (u == u->parent->gauche)
            u->parent->gauche = v;
        else
            u->parent->droite = v;
        if (v != NULL)
            v->parent = u->parent;
    }

    Noeud* minimum(Noeud* n) {
        while (n->gauche != NULL) n = n->gauche;
        return n;
    }

    // x porte un "noir en trop" (double noir). x peut etre NULL, c'est
    // pourquoi on garde aussi son parent p.
    void corrigerSuppression(Noeud* x, Noeud* p) {
        while (x != racine && couleurDe(x) == NOIR) {
            if (p == NULL) break;

            if (x == p->gauche) {
                Noeud* w = p->droite;                       // le frere de x

                if (couleurDe(w) == ROUGE) {                // Cas 1 : frere rouge
                    w->couleur = NOIR;
                    p->couleur = ROUGE;
                    rotationGauche(p);
                    w = p->droite;
                }
                if (couleurDe(w->gauche) == NOIR && couleurDe(w->droite) == NOIR) {
                    w->couleur = ROUGE;                     // Cas 2 : frere noir, neveux noirs
                    x = p;
                    p = x->parent;
                } else {
                    if (couleurDe(w->droite) == NOIR) {     // Cas 3 : neveu proche rouge
                        if (w->gauche != NULL) w->gauche->couleur = NOIR;
                        w->couleur = ROUGE;
                        rotationDroite(w);
                        w = p->droite;
                    }
                    w->couleur = p->couleur;                // Cas 4 : neveu eloigne rouge
                    p->couleur = NOIR;
                    if (w->droite != NULL) w->droite->couleur = NOIR;
                    rotationGauche(p);
                    x = racine;
                    p = NULL;
                }
            } else {
                // Cas symetriques (x est l'enfant droit)
                Noeud* w = p->gauche;

                if (couleurDe(w) == ROUGE) {
                    w->couleur = NOIR;
                    p->couleur = ROUGE;
                    rotationDroite(p);
                    w = p->gauche;
                }
                if (couleurDe(w->gauche) == NOIR && couleurDe(w->droite) == NOIR) {
                    w->couleur = ROUGE;
                    x = p;
                    p = x->parent;
                } else {
                    if (couleurDe(w->gauche) == NOIR) {
                        if (w->droite != NULL) w->droite->couleur = NOIR;
                        w->couleur = ROUGE;
                        rotationGauche(w);
                        w = p->gauche;
                    }
                    w->couleur = p->couleur;
                    p->couleur = NOIR;
                    if (w->gauche != NULL) w->gauche->couleur = NOIR;
                    rotationDroite(p);
                    x = racine;
                    p = NULL;
                }
            }
        }
        if (x != NULL) x->couleur = NOIR;
    }

    Noeud* chercher(int v) {
        Noeud* c = racine;
        while (c != NULL && c->valeur != v) {
            if (v < c->valeur) c = c->gauche;
            else               c = c->droite;
        }
        return c;
    }

    // ---------- Affichage ----------
    void infixe(Noeud* n) {
        if (n == NULL) return;
        infixe(n->gauche);
        cout << n->valeur << (n->couleur == ROUGE ? "(R) " : "(N) ");
        infixe(n->droite);
    }

    int hauteurNoeud(Noeud* n) {
        if (n == NULL) return 0;
        int a = hauteurNoeud(n->gauche);
        int b = hauteurNoeud(n->droite);
        return 1 + (a > b ? a : b);
    }

    int remplirGrille(Noeud* n, int prof, int& rang, char** g, int W) {
        if (n == NULL) return -1;

        int cg = remplirGrille(n->gauche, prof + 1, rang, g, W);

        int debut = rang * W;
        rang++;

        char etiq[24];
        sprintf(etiq, "%d(%c)", n->valeur, n->couleur == ROUGE ? 'R' : 'N');
        int len = strlen(etiq);
        for (int i = 0; i < len; i++)
            g[2 * prof][debut + i] = etiq[i];
        int centre = debut + len / 2;

        int cd = remplirGrille(n->droite, prof + 1, rang, g, W);

        int r = 2 * prof + 1;
        if (cg >= 0) {
            for (int x = cg; x <= centre; x++) g[r][x] = '-';
            g[r][cg] = '+';
            g[r][centre] = '+';
        }
        if (cd >= 0) {
            for (int x = centre; x <= cd; x++) g[r][x] = '-';
            g[r][cd] = '+';
            g[r][centre] = '+';
        }
        return centre;
    }

    // ---------- Verification des proprietes ----------
    // Retourne la hauteur noire du sous-arbre, ou -1 si une propriete est violee
    int verifierNoeud(Noeud* n, long long min, long long max) {
        if (n == NULL) return 1;                            // NIL noir
        if (n->valeur <= min || n->valeur >= max) return -1; // ordre ABR
        if (n->gauche != NULL && n->gauche->parent != n) return -1;
        if (n->droite != NULL && n->droite->parent != n) return -1;
        if (n->couleur == ROUGE &&
            (couleurDe(n->gauche) == ROUGE || couleurDe(n->droite) == ROUGE))
            return -1;                                      // rouge-rouge interdit

        int a = verifierNoeud(n->gauche, min, n->valeur);
        int b = verifierNoeud(n->droite, n->valeur, max);
        if (a == -1 || b == -1 || a != b) return -1;        // meme hauteur noire
        return a + (n->couleur == NOIR ? 1 : 0);
    }

    void detruire(Noeud* n) {
        if (n == NULL) return;
        detruire(n->gauche);
        detruire(n->droite);
        delete n;
    }

public:
    ArbreRN() : racine(NULL), taille(0) {}
    ~ArbreRN() { detruire(racine); }

    int getTaille() { return taille; }
    int hauteur() { return hauteurNoeud(racine); }

    // ---------- Insertion ----------
    void inserer(int v) {
        Noeud* parent = NULL;
        Noeud* courant = racine;

        while (courant != NULL) {
            parent = courant;
            if (v < courant->valeur)       courant = courant->gauche;
            else if (v > courant->valeur)  courant = courant->droite;
            else return;                   // doublon : on ignore
        }

        Noeud* n = new Noeud(v);
        n->parent = parent;

        if (parent == NULL)          racine = n;
        else if (v < parent->valeur) parent->gauche = n;
        else                         parent->droite = n;

        taille++;
        corrigerInsertion(n);
    }

    // ---------- Recherche ----------
    bool rechercher(int v) {
        return chercher(v) != NULL;
    }

    // ---------- Suppression ----------
    bool supprimer(int v) {
        Noeud* z = chercher(v);
        if (z == NULL) return false;

        Noeud* y = z;
        Couleur couleurOriginale = y->couleur;
        Noeud* x;           // le noeud qui prend la place (peut etre NULL)
        Noeud* xParent;     // son parent (necessaire si x est NULL)

        if (z->gauche == NULL) {                 // 0 ou 1 enfant (droit)
            x = z->droite;
            xParent = z->parent;
            transplanter(z, z->droite);
        } else if (z->droite == NULL) {          // 1 enfant (gauche)
            x = z->gauche;
            xParent = z->parent;
            transplanter(z, z->gauche);
        } else {                                 // 2 enfants : on prend le successeur
            y = minimum(z->droite);
            couleurOriginale = y->couleur;
            x = y->droite;

            if (y->parent == z) {
                xParent = y;
            } else {
                xParent = y->parent;
                transplanter(y, y->droite);
                y->droite = z->droite;
                y->droite->parent = y;
            }
            transplanter(z, y);
            y->gauche = z->gauche;
            y->gauche->parent = y;
            y->couleur = z->couleur;
        }

        delete z;
        taille--;

        // Si on a retire un noeud NOIR, il manque un noir sur ce chemin
        if (couleurOriginale == NOIR)
            corrigerSuppression(x, xParent);
        return true;
    }

    // ---------- Verification ----------
    bool estValide() {
        if (racine == NULL) return true;
        if (racine->couleur != NOIR) return false;
        if (racine->parent != NULL) return false;
        return verifierNoeud(racine, -2147483648LL - 1, 2147483647LL + 1) != -1;
    }

    // ---------- Affichage ----------
    void afficher() {
        infixe(racine);
        cout << endl;
    }

    void afficherArbre() {
        if (racine == NULL) {
            cout << "(arbre vide)" << endl;
            return;
        }
        if (taille > 31) {
            cout << "(arbre trop grand pour l'affichage graphique : "
                 << taille << " noeuds, utilisez le parcours infixe)" << endl;
            return;
        }

        const int W = 9;
        int h = hauteurNoeud(racine);
        int lignes = 2 * h - 1;
        int colonnes = taille * W + W;

        char** g = new char*[lignes];
        for (int i = 0; i < lignes; i++) {
            g[i] = new char[colonnes + 1];
            for (int j = 0; j < colonnes; j++) g[i][j] = ' ';
            g[i][colonnes] = '\0';
        }

        int rang = 0;
        remplirGrille(racine, 0, rang, g, W);

        for (int i = 0; i < lignes; i++) {
            int fin = colonnes - 1;
            while (fin >= 0 && g[i][fin] == ' ') fin--;
            g[i][fin + 1] = '\0';
            cout << g[i] << endl;
            delete[] g[i];
        }
        delete[] g;
    }
};

// =====================================================================
//                    COMPARAISON ArbreRN  /  std::map
// =====================================================================

typedef chrono::steady_clock Horloge;

double secondes(Horloge::time_point debut, Horloge::time_point fin) {
    return chrono::duration<double>(fin - debut).count();
}

struct Mesures {
    double tIns, tSup, tRec;       // temps cumules (secondes)
    long long nIns, nSup, nRec;    // nombre d'operations effectuees
    long long trouves;             // valeurs trouvees lors des recherches
    int attendu;                   // nombre de valeurs qui doivent rester dans la structure
    int taille, hauteur;
    double pctTrie;                // % de valeurs de la suite d'insertion qui sont a leur place triee
    bool valide;
    bool ok;                       // false si la memoire a manque
};

// Generateur pseudo-aleatoire (xorshift) : meme graine => memes valeurs,
// donc l'ArbreRN et le map recoivent exactement les memes operations.
// (rand() de MinGW ne depasse pas 32767, on ne peut pas l'utiliser pour N grand.)
static unsigned long long etatAlea = 88172645463325252ULL;

unsigned int alea() {
    etatAlea ^= etatAlea << 13;
    etatAlea ^= etatAlea >> 7;
    etatAlea ^= etatAlea << 17;
    return (unsigned int)(etatAlea >> 32);
}

// Adaptateur pour l'ArbreRN
struct SArbre {
    ArbreRN a;
    void ins(int v)   { a.inserer(v); }
    void sup(int v)   { a.supprimer(v); }
    bool rech(int v)  { return a.rechercher(v); }
    int  taille()     { return a.getTaille(); }
    int  hauteur()    { return a.hauteur(); }
    bool valide()     { return a.estValide(); }
};

// Adaptateur pour std::map
/*struct SMap {
    map<int, int> m;
    void ins(int v)   { m.insert(make_pair(v, v)); }
    void sup(int v)   { m.erase(v); }
    bool rech(int v)  { return m.find(v) != m.end(); }
    int  taille()     { return (int)m.size(); }
    int  hauteur()    { return 0; }
    bool valide()     { return true; }
};*/
// Adaptateur pour std::unordered_map (table de hachage)
struct SHash {
    unordered_map<int, int> m;
    SHash() { m.max_load_factor(1.0f); }   // facteur de charge par defaut, explicite
    void ins(int v)   { m.insert(make_pair(v, v)); }
    void sup(int v)   { m.erase(v); }
    bool rech(int v)  { return m.find(v) != m.end(); }
    int  taille()     { return (int)m.size(); }
    int  hauteur()    { return 0; }
    bool valide()     { return true; }
};

// Deux tableaux :
//   seq[]  : la suite des valeurs a inserer, dans l'ordre (triee, presque triee ou aleatoire)
//            seq[suiv], seq[suiv+1], ... sont les prochaines valeurs a inserer.
//   pres[] : les valeurs actuellement presentes dans la structure (pres[0 .. cnt-1]),
//            sert a choisir au hasard les valeurs a supprimer ou a rechercher.

template<class S>
double phaseInsertion(S& s, int* seq, int& suiv, int* pres, int& cnt, int k) {
    Horloge::time_point t0 = Horloge::now();
    for (int i = 0; i < k; i++) s.ins(seq[suiv + i]);
    double t = secondes(t0, Horloge::now());
    for (int i = 0; i < k; i++) pres[cnt++] = seq[suiv + i];   // hors mesure
    suiv += k;
    return t;
}

template<class S>
double phaseSuppression(S& s, int* pres, int& cnt, int k, int* tampon) {
    for (int i = 0; i < k && cnt > 0; i++) {        // choix des valeurs (non mesure)
        int idx = alea() % cnt;
        tampon[i] = pres[idx];
        pres[idx] = pres[cnt - 1];
        cnt--;
    }
    Horloge::time_point t0 = Horloge::now();
    for (int i = 0; i < k; i++) s.sup(tampon[i]);
    return secondes(t0, Horloge::now());
}

template<class S>
double phaseRecherche(S& s, int* pres, int cnt, int k, int* tampon, long long& trouves) {
    for (int i = 0; i < k; i++)                     // choix des valeurs (non mesure)
        tampon[i] = pres[alea() % cnt];
    Horloge::time_point t0 = Horloge::now();
    for (int i = 0; i < k; i++) if (s.rech(tampon[i])) trouves++;
    return secondes(t0, Horloge::now());
}

const int TOURS = 10;
const int ALEATOIRE = -1;     // valeur speciale de pctTrie : suite totalement melangee

// Protocole : 10 tours. A chaque tour, avec B = N / 10 :
//   insertion de B/2 valeurs, suppression de B/4, insertion de B/2,
//   suppression de B/4, puis recherche de B valeurs.
// Au total : N insertions, N/2 suppressions, N recherches.
//
// Ordre des valeurs inserees (pctTrie) :
//   ALEATOIRE : permutation aleatoire de 1..N
//   100       : 1, 2, 3, ..., N (triee)
//   p < 100   : on part de 1..N puis on echange au hasard des paires de valeurs,
//               de facon a ce qu'environ p % des valeurs restent a leur place triee.
template<class S>
Mesures tester(int N, int pctTrie) {
    Mesures r;
    r.tIns = r.tSup = r.tRec = 0;
    r.nIns = r.nSup = r.nRec = 0;
    r.trouves = 0;
    r.attendu = r.taille = r.hauteur = 0;
    r.pctTrie = 0;
    r.valide = false;
    r.ok = false;

    int* seq = NULL;
    int* pres = NULL;
    int* tampon = NULL;
    try {
        S s;
        seq = new int[N];
        pres = new int[N];
        for (int i = 0; i < N; i++) seq[i] = i + 1;

        etatAlea = 88172645463325252ULL;            // meme graine a chaque appel

        if (pctTrie == ALEATOIRE) {
            for (int i = N - 1; i > 0; i--) {       // melange complet (Fisher-Yates)
                int j = alea() % (i + 1);
                int tmp = seq[i]; seq[i] = seq[j]; seq[j] = tmp;
            }
        } else if (pctTrie < 100) {
            // chaque echange deplace 2 valeurs : N*(100-p)/200 echanges
            long long nbEchanges = (long long)N * (100 - pctTrie) / 200;
            for (long long e = 0; e < nbEchanges; e++) {
                int i = alea() % N;
                int j = alea() % N;
                int tmp = seq[i]; seq[i] = seq[j]; seq[j] = tmp;
            }
        }

        long long alaPlace = 0;                     // pourcentage reellement trie
        for (int i = 0; i < N; i++) if (seq[i] == i + 1) alaPlace++;
        r.pctTrie = 100.0 * alaPlace / N;

        int B = N / TOURS;
        int insA = B / 2, insB = B - insA;
        int supA = B / 4, supB = B / 2 - supA;
        tampon = new int[B];
        int suiv = 0, cnt = 0;

        for (int t = 0; t < TOURS; t++) {
            r.tIns += phaseInsertion(s, seq, suiv, pres, cnt, insA);
            r.tSup += phaseSuppression(s, pres, cnt, supA, tampon);
            r.tIns += phaseInsertion(s, seq, suiv, pres, cnt, insB);
            r.tSup += phaseSuppression(s, pres, cnt, supB, tampon);
            r.tRec += phaseRecherche(s, pres, cnt, B, tampon, r.trouves);

            r.nIns += insA + insB;
            r.nSup += supA + supB;
            r.nRec += B;
        }

        r.attendu = cnt;
        r.taille = s.taille();
        r.hauteur = s.hauteur();
        r.valide = s.valide();
        r.ok = true;
    } catch (bad_alloc&) {
        r.ok = false;
    }
    delete[] seq;
    delete[] pres;
    delete[] tampon;
    return r;
}

void ligneTableau(const char* nom, long long nbOps, double moyArbre, double moyHash) {
    double rapport = (moyHash > 0) ? moyArbre / moyHash : 0;
    // les moyennes arrivent en secondes : on affiche en millisecondes
    printf("| %-12s | %11lld | %13.7f | %13.7f | %11.2f |\n",
           nom, nbOps, moyArbre * 1000.0, moyHash * 1000.0, rapport);
}

void comparer(const int* tailles, int nb, int pctTrie) {
    printf("Compilateur en %d bits, taille d'un noeud : %d octets\n",
           (int)(sizeof(void*) * 8), (int)sizeof(Noeud));

    for (int k = 0; k < nb; k++) {
        int N = tailles[k];
        printf("\nCalcul pour N = %d ...\n", N);
        fflush(stdout);

        // Une seule structure en memoire a la fois
        Mesures a = tester<SArbre>(N, pctTrie);
        Mesures h = tester<SHash>(N, pctTrie);

        if (!a.ok || !h.ok) {
            printf("Memoire insuffisante pour N = %d (ArbreRN : %s, hash : %s)\n",
                   N, a.ok ? "ok" : "echec", h.ok ? "ok" : "echec");
            continue;
        }

        int B = N / TOURS;
        if (pctTrie == ALEATOIRE)
            printf("\nN = %d (valeurs aleatoires : permutation de 1..N), %d tours\n", N, TOURS);
        else
            printf("\nN = %d (valeurs presque triees : %.1f %% a leur place triee), %d tours\n",
                   N, a.pctTrie, TOURS);
        printf("Par tour : %d insertions, %d suppressions, %d recherches\n",
               B, B / 2, B);
        printf("Temps moyen d'UNE operation, en millisecondes (ms)\n");
        printf("+--------------+-------------+---------------+---------------+-------------+\n");
        printf("| Operation    | Nb d'ops    | ArbreRN (ms)  | hash (ms)     | ArbreRN/hash|\n");
        printf("+--------------+-------------+---------------+---------------+-------------+\n");
        ligneTableau("Insertion",   a.nIns, a.tIns / a.nIns, h.tIns / h.nIns);
        ligneTableau("Suppression", a.nSup, a.tSup / a.nSup, h.tSup / h.nSup);
        ligneTableau("Recherche",   a.nRec, a.tRec / a.nRec, h.tRec / h.nRec);
        printf("+--------------+-------------+---------------+---------------+-------------+\n");
        printf("Temps total mesure (ms) : ArbreRN %.3f | hash %.3f\n",
               (a.tIns + a.tSup + a.tRec) * 1000.0, (h.tIns + h.tSup + h.tRec) * 1000.0);
        printf("Taille finale attendue : %d | ArbreRN : %d | hash : %d\n",
               a.attendu, a.taille, h.taille);
        printf("Hauteur finale de l'ArbreRN : %d (maximum theorique 2*log2(n+1) = %.1f)\n",
               a.hauteur, 2 * log((double)a.taille + 1) / log(2.0));
        printf("Proprietes rouge-noir : %s\n", a.valide ? "valides" : "VIOLEES");
        printf("Valeurs trouvees : ArbreRN %lld/%lld, hash %lld/%lld\n",
               a.trouves, a.nRec, h.trouves, h.nRec);
    }
}
// =====================================================================

int lireEntier(const char* message) {
    int v;
    cout << message;
    while (!(cin >> v)) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Saisie invalide, entrez un entier : ";
    }
    return v;
}

int main() {
    ArbreRN arbre;
    int valeurs[] = {7, 58, 48, 2, 17, 84, 82};   // l'exemple fait a la main
    int n = sizeof(valeurs) / sizeof(valeurs[0]);

    for (int i = 0; i < n; i++) arbre.inserer(valeurs[i]);
    cout << "Arbre de depart (7, 58, 48, 2, 17, 84, 82) :" << endl;
    arbre.afficherArbre();

    int choix;
    do {
        cout << endl;
        cout << "========== MENU ==========" << endl;
        cout << "1. Inserer un nombre" << endl;
        cout << "2. Rechercher un nombre" << endl;
        cout << "3. Supprimer un nombre" << endl;
        cout << "4. Afficher l'arbre" << endl;
        cout << "5. Comparer ArbreRN et hashmap (1000, 10000, 100000, 1000000)" << endl;
        cout << "6. Comparer ArbreRN et hashmap (10000000, 100000000) - beaucoup de memoire" << endl;
        cout << "7. Experience : valeurs presque triees (1000 a 10000000)" << endl;
        cout << "0. Quitter" << endl;
        choix = lireEntier("Votre choix : ");
        cout << endl;

        if (choix == 1) {
            int v = lireEntier("Nombre a inserer : ");
            arbre.inserer(v);
            cout << "Apres insertion de " << v << " :" << endl;
            arbre.afficherArbre();
        } else if (choix == 2) {
            int v = lireEntier("Nombre a rechercher : ");
            if (arbre.rechercher(v)) cout << v << " est present dans l'arbre." << endl;
            else                     cout << v << " n'est pas dans l'arbre." << endl;
        } else if (choix == 3) {
            int v = lireEntier("Nombre a supprimer : ");
            if (arbre.supprimer(v)) {
                cout << "Apres suppression de " << v << " :" << endl;
                arbre.afficherArbre();
            } else {
                cout << v << " n'est pas dans l'arbre, rien a supprimer." << endl;
            }
        } else if (choix == 4) {
            arbre.afficherArbre();
            cout << "Parcours infixe : ";
            arbre.afficher();
        } else if (choix == 5) {
            int tailles[] = {1000, 10000, 100000, 1000000};
            cout << "Comparaison en cours (compiler en mode Release pour des temps realistes)..." << endl;
            comparer(tailles, 4, ALEATOIRE);
        } else if (choix == 6) {
            int tailles[] = {10000000, 100000000};
            cout << "Attention : environ 0.5 Go pour N = 10^7 et 5 Go pour N = 10^8" << endl;
            cout << "(compilateur 64 bits obligatoire). Continuer ? (1 = oui, 0 = non) : ";
            int rep;
            cin >> rep;
            if (rep == 1) comparer(tailles, 2, ALEATOIRE);
        } else if (choix == 7) {
            int tailles[] = {1000, 10000, 100000, 1000000};
            int p = lireEntier("Pourcentage de valeurs triees (90 pour 90%, 100 = totalement triee) : ");
            if (p < 0) p = 0;
            if (p > 100) p = 100;
            cout << "Experience en cours (la derniere taille peut prendre quelques minutes)..." << endl;
            comparer(tailles, 5, p);
        }
    } while (choix != 0);

    return 0;
}
