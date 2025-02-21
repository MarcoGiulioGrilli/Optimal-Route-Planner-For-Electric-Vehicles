// Librerie utilizzate
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Costante per la lunghezza massima del comando da input
#define MAXS 19

// Struttura dati per la lista delle autonomie delle auto presenti per ogni stazione di servizio
typedef struct autonomieLista {
    int num;
    struct autonomieLista *next;
} autonomieLista_t;

// Struttura dati per la stazione di servizio con la sua lista di autonomie
typedef struct {
    int distanza;
    autonomieLista_t *autonomie;
} stazioneServizio_t;

// Struttura dati per il nodo del BST che contiene la stazione di servizio
typedef struct node {
    stazioneServizio_t stazione;
    struct node *left;
    struct node *right;
} node_t;

// Struttura dati per l'albero
typedef struct {
    node_t *root;
} autostrada_t;

// Struttura dati per la lista delle tappe fatte durante il percorso
typedef struct tappa {
    int distanzaTappa;
    struct tappa *next;
} tappa_t;

// Prototipi delle funzioni
autonomieLista_t *aggiungiAutonomia(autonomieLista_t *head, int num);
node_t *newNode(int distanza, int numeroAuto, int* autonomie);
node_t *findNode(int distanza, node_t *root);
node_t *findNextNodeAscending(int distanza, node_t *root);
node_t *findNextNodeDescending(int distanza, node_t *root);
node_t *findFurthestReachableStation(int stazionePartenza, node_t *root);
void freeAutonomieLista(autonomieLista_t *head);
void freeBST(node_t *root);
void freeTappe(tappa_t *head);
void freeAutostrada(autostrada_t *autostrada);
void aggiungiStazione(int distanza, int numeroAuto, int* autonomie);
void demolisciStazione(int distanza);
void aggiungiAuto(int distanzaStazione, int autonomiaAuto);
void rottamaAuto(int distanzaStazione, int autonomiaAuto);
tappa_t *inserisciInTesta(tappa_t *head, int num);
tappa_t *inserisciInCoda(tappa_t* head, int num);
tappa_t *invertiLista(tappa_t *head);
void replaceNode(tappa_t **head, int vecchiaTappa, int nuovaTappa);
tappa_t *pianificaPercorsoAndata(int stazionePartenza, int stazioneArrivo, int partenzaProvvisoria, int autonomiaMax, int *found, tappa_t *head, node_t *root);
tappa_t *pianificaPercorsoRitorno(int stazionePartenza, int stazioneArrivo, int *found, tappa_t *head, node_t *root);
void pianificaPercorso(int stazionePartenza, int stazioneArrivo);
tappa_t *verificaPercorso(tappa_t *head, node_t *root, int fine);

// Variabile globale per l'albero
autostrada_t autostrada;

// Main del programma che legge i comandi da input e chiama i sottoprogrammi
int main() {
    char comando[MAXS];
    int d, na, autA, sp, sa;
    int *numA;

    while (scanf("%s", comando)==1) {
        if (strcmp(comando, "aggiungi-stazione") == 0) {
            assert(scanf("%d", &d)==1);
            assert(scanf("%d", &na)==1);
            numA = malloc(sizeof(int) * na);
            for (int k = 0; k < na; k++)
                assert(scanf("%d", &numA[k])==1);
            aggiungiStazione(d, na, numA);
            free(numA);
        } else if (strcmp(comando, "demolisci-stazione") == 0) {
            assert(scanf("%d", &d)==1);
            demolisciStazione(d);
        } else if (strcmp(comando, "aggiungi-auto") == 0) {
            assert(scanf("%d", &d)==1);
            assert(scanf("%d", &autA)==1);
            aggiungiAuto(d, autA);
        } else if (strcmp(comando, "rottama-auto") == 0) {
            assert(scanf("%d", &d)==1);
            assert(scanf("%d", &autA)==1);
            rottamaAuto(d, autA);
        } else if (strcmp(comando, "pianifica-percorso") == 0) {
            assert(scanf("%d", &sp)==1);
            assert(scanf("%d", &sa)==1);
            pianificaPercorso(sp, sa);
        }
    }

    // Libera la memoria dell'albero finti gli input
    freeAutostrada(&autostrada);

    return 0;
}

// Sottoprogramma per creare un nodo della lista delle autonomie
autonomieLista_t *aggiungiAutonomia(autonomieLista_t *head, int num) {
    autonomieLista_t *nuovaAutonomia = (autonomieLista_t*)malloc(sizeof(autonomieLista_t));

    nuovaAutonomia->num = num;
    nuovaAutonomia->next = NULL;

    if (head == NULL || head->num < num) {
        nuovaAutonomia->next = head;
        return nuovaAutonomia;
    }

    autonomieLista_t *curr = head;
    while (curr->next != NULL && curr->next->num > num) {
        curr = curr->next;
    }

    nuovaAutonomia->next = curr->next;
    curr->next = nuovaAutonomia;

    return head;
}

// Sottoprogramma per creare un nodo del bst come stazione di servizio con le sue autonomie in ordine decrescente
node_t *newNode(int distanza, int numeroAuto, int* autonomie) {
    node_t *nuovaStazione = malloc(sizeof(node_t));

    nuovaStazione->stazione.distanza = distanza;
    nuovaStazione->stazione.autonomie = NULL;

    for(int i=0; i<numeroAuto; i++)
        nuovaStazione->stazione.autonomie = aggiungiAutonomia(nuovaStazione->stazione.autonomie, autonomie[i]);

    nuovaStazione->left = NULL;
    nuovaStazione->right = NULL;
    return nuovaStazione;
}

// Sottoprogramma per trovare la stazione di servizio all'interno dell'albero
node_t *findNode(int distanza, node_t *root) {
    if (root == NULL || root->stazione.distanza == distanza)
        return root;
    if (root->stazione.distanza < distanza) return findNode(distanza, root->right);

    return findNode(distanza, root->left);
}

// Sottoprogramma per trovare la stazione di servizio successiva a quella passata come parametro in ordine crescente
node_t *findNextNodeAscending(int distanza, node_t *root) {
    if (root == NULL) return NULL;

    if (root->stazione.distanza <= distanza) {
        return findNextNodeAscending(distanza, root->right);
    } else {
        // Abbiamo un nodo con una distanza maggiore, ma dobbiamo controllare il sottoalbero sinistro
        // per essere sicuri che non ci siano nodi con una distanza leggermente maggiore di 'distanza'.
        node_t *leftNode = findNextNodeAscending(distanza, root->left);
        if (leftNode) return leftNode;
        else return root;
    }
}

// Sottoprogramma per trovare la stazione di servizio successiva a quella passata come parametro in ordine decrescente
node_t *findNextNodeDescending(int distanza, node_t *root) {
    node_t *risultato = NULL;

    while (root != NULL) {
        if (root->stazione.distanza < distanza) {
            // Se la distanza corrente è minore della distanza data,
            // abbiamo un nuovo candidato per il nodo massimo raggiungibile.
            risultato = root;

            // Ora dobbiamo cercare nel sottoalbero destro per vedere se c'è un nodo con una distanza ancora più vicina, ma comunque minore di 'distanza'.
            root = root->right;
        } else {
            // Altrimenti, passiamo al sottoalbero sinistro per cercare nodi con distanze minori.
            root = root->left;
        }
    }

    return risultato;
}

// Sottoprogramma per trovare la stazione di servizio successiva a quella passata come parametro in ordine decrescente
node_t *findFurthestReachableStation(int stazionePartenza,  node_t *root) {
    node_t *risultato = NULL;
    int autonomiaMax;
    node_t *cerca = findNode(stazionePartenza, root);
    if(cerca == NULL){
        return NULL;
    }else {
        if(cerca->stazione.autonomie == NULL){
            return NULL;
        }else {
            autonomiaMax = cerca->stazione.autonomie[0].num;
        }
    }
    int distanzaMassimaRaggiungibile = stazionePartenza - autonomiaMax; // calcolo la distanza massima raggiungibile

    while (root != NULL) {
        if (root->stazione.distanza <= stazionePartenza && root->stazione.distanza >= distanzaMassimaRaggiungibile) {
            // Se la distanza corrente è minore o uguale alla stazione di partenza e maggiore o uguale alla distanza massima raggiungibile,
            // abbiamo un nuovo candidato per la stazione più lontana raggiungibile.
            risultato = root;

            // Proseguiamo nel sottoalbero sinistro per cercare stazioni ancora più lontane (ma sempre raggiungibili).
            root = root->left;
        } else if (root->stazione.distanza > stazionePartenza) {
            // Se la stazione corrente è troppo lontana, proseguiamo nel sottoalbero sinistro.
            root = root->left;
        } else {
            // Se la stazione corrente è troppo vicina, proseguiamo nel sottoalbero destro.
            root = root->right;
        }
    }

    // Controllo se la stazione trovata è effettivamente raggiungibile entro l'autonomia massima e non è la stazione di partenza.
    if (risultato && risultato->stazione.distanza >= distanzaMassimaRaggiungibile && risultato->stazione.distanza != stazionePartenza) {
        return risultato;
    } else {
        return NULL;
    }
}

// Sottoprogramma per liberare la memoria della lista delle autonomie
void freeAutonomieLista(autonomieLista_t *head) {
    autonomieLista_t *temp;

    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

// Sottoprogramma per liberare la memoria dei nodi dell'albero
void freeBST(node_t *root) {
    if (root == NULL) {
        return;
    }

    // Libero prima i sottoalberi sinistro e destro
    freeBST(root->left);
    freeBST(root->right);

    // Libero la lista delle autonomie
    freeAutonomieLista(root->stazione.autonomie);

    // Libero il nodo stesso
    free(root);
}

// Sottoprogramma per liberare la memoria della lista delle tappe
void freeTappe(tappa_t *head) {
    tappa_t *temp;

    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

// Sottoprogramma per liberare la memoria dell'albero
void freeAutostrada(autostrada_t *autostradaFinale) {
    freeBST(autostradaFinale->root);
    autostradaFinale->root = NULL; // opzionale, per buona misura
}

// Sottoprogramma per aggiungere una stazione di servizio all'albero
void aggiungiStazione(int distanza, int numeroAuto, int* autonomie) {
    node_t *curr = autostrada.root;
    node_t *padre = NULL;

    // Posiziona il puntatore curr alla posizione corretta nel bst
    while (curr != NULL) {
        padre = curr;
        if (distanza < curr->stazione.distanza) {
            curr = curr->left;
        } else if (distanza > curr->stazione.distanza) {
            curr = curr->right;
        } else {
            // Stampa il messaggio di errore se la stazione è già presente
            printf("non aggiunta\n");
            return;
        }
    }

    // Inserisce la nuova stazione di servizio nel bst
    node_t *nuovaStazione = newNode(distanza, numeroAuto, autonomie);
    if (padre == NULL) {
        autostrada.root = nuovaStazione; // Se l'albero è vuoto inserisce la nuova stazione come radice
    } else if (distanza < padre->stazione.distanza) {
        padre->left = nuovaStazione;
    } else {
        padre->right = nuovaStazione;
    }

    // Stampa il messaggio di aggiunta
    printf("aggiunta\n");
}

//sottoprogramma per eliminare una stazione di servizio
void demolisciStazione(int distanza) {
    node_t *curr = autostrada.root;
    node_t *padre = NULL;
    node_t *figlio;

    // Trova il nodo della stazione e il suo rispettivo padre
    while (curr != NULL && curr->stazione.distanza != distanza) {
        padre = curr;
        if (distanza < curr->stazione.distanza) {
            curr = curr->left;
        } else {
            curr = curr->right;
        }
    }

    //la stazione non è presente nell'albero quindi stampo il messaggio che la stazione non è stata demolita
    if (curr == NULL) {
        printf("non demolita\n");
        return;
    }

    // Liberare la memoria della lista di autonomie
    autonomieLista_t *tmp;
    while (curr->stazione.autonomie != NULL) {
        tmp = curr->stazione.autonomie;
        curr->stazione.autonomie = curr->stazione.autonomie->next;
        free(tmp);
    }

    // La stazione è presente nel bst e la elimino
    if (curr->left == NULL || curr->right == NULL){
        if (curr->left != NULL) {
            figlio = curr->left;
        } else {
            figlio = curr->right;
        }

        if (padre == NULL) {
            autostrada.root = figlio;
        } else if (padre->left == curr) {
            padre->left = figlio;
        } else {
            padre->right = figlio;
        }
    } else {
        node_t *next = curr->right;
        node_t *padreNext = curr;
        while (next->left != NULL){
            padreNext = next;
            next = next->left;
        }

        //sostituisce la stazione con il prossimo nodo
        curr->stazione = next->stazione;
        if (padreNext->left == next) {
            padreNext->left = next->right;
        } else {
            padreNext->right = next->right;
        }
        curr = next;
    }

    //libera la memoria e stampo il messaggio che la stazione è stata demolita
    free(curr);
    printf("demolita\n");
}

// Sottoprogramma per aggiungere un auto e la sua rispettiva autonomia ad una stazione di servizio presente nell'albero
void aggiungiAuto(int distanzaStazione, int autonomiaAuto) {
    //cerco la stazione di servizio all'interno dell'albero
    node_t *stazioneCercata = findNode(distanzaStazione, autostrada.root);

    //se la stazione non esiste stampo il messaggio che l'auto non è stata aggiunta
    if (stazioneCercata == NULL) {
        printf("non aggiunta\n");
        return;
    }

    //aggiungo l'autonomia alla stazione di servizio e stampo il messaggio che l'auto è stata aggiunta
    stazioneCercata->stazione.autonomie = aggiungiAutonomia(stazioneCercata->stazione.autonomie, autonomiaAuto);
    printf("aggiunta\n");
}

// Sottoprogramma per eliminare un auto da una stazione di servizio presente nell'albero
void rottamaAuto(int distanzaStazione, int autonomiaAuto) {
    //cerco la stazione di servizio all'interno dell'albero
    node_t *stazione = findNode(distanzaStazione, autostrada.root);

    //se la stazione non esiste o non ha auto stampo il messaggio che l'auto non è stata rottamata
    if (stazione == NULL || stazione->stazione.autonomie == NULL) {
        printf("non rottamata\n");
        return;
    }

    autonomieLista_t *current = stazione->stazione.autonomie;
    autonomieLista_t *prev = NULL;

    while (current != NULL && current->num != autonomiaAuto) {
        prev = current;
        current = current->next;
    }

    // Se abbiamo trovato l'autonomia
    if (current != NULL) {
        // Se è il primo nodo
        if (prev == NULL) {
            stazione->stazione.autonomie = current->next;
        } else {
            prev->next = current->next;
        }
        free(current);
        printf("rottamata\n");
    } else {
        printf("non rottamata\n");
    }
}

// Sottoprogramma per inserire un elemento in testa alla lista delle tappe fatte durante il percorso di andata
tappa_t *inserisciInTesta(tappa_t *head, int num){
    tappa_t * tmp;
    tmp = malloc(sizeof(tappa_t));
    if(tmp){
        tmp->distanzaTappa = num;
        tmp->next = head;
        head = tmp;
    }
    return head;
}

// Sottoprogramma per inserire un elemento in coda alla lista delle tappe fatte durante il percorso di ritorno
tappa_t *inserisciInCoda(tappa_t* head, int num){
    tappa_t *prec;
    tappa_t *tmp;
    tmp = malloc(sizeof(tappa_t));
    if(tmp != NULL){
        tmp->next = NULL;
        tmp->distanzaTappa = num;
        if(head == NULL)
            head = tmp;
        else{
            for(prec=head;prec->next!=NULL;prec=prec->next);
            prec->next = tmp;
        }
    }
    return head;
}

// Sottoprogramma per invertire la lista delle tappe
tappa_t *invertiLista(tappa_t *head) {
    tappa_t *prev = NULL;    // Puntatore al nodo precedente
    tappa_t *current = head; // Puntatore al nodo corrente
    tappa_t *next = NULL;    // Puntatore al prossimo nodo

    while (current != NULL) {
        // Salva il nodo successivo
        next = current->next;
        // Cambia la direzione del nodo corrente
        current->next = prev;
        // Sposta i puntatori prev e current di un passo in avanti
        prev = current;
        current = next;
    }

    head = prev;
    return head;
}

// Sottoprogramma per rimpiazzare un nodo della lista delle tappe
void replaceNode(tappa_t **head, int vecchiaTappa, int nuovaTappa) {
    tappa_t *current = *head;
    tappa_t *prev = NULL;

    // Scorri la lista finché non trovi il nodo da rimuovere o raggiungi la fine della lista
    while (current != NULL && current->distanzaTappa != vecchiaTappa) {
        prev = current;
        current = current->next;
    }

    // Se il nodo da rimuovere è stato trovato
    if (current != NULL) {
        // Crea il nuovo nodo
        tappa_t *newNode = malloc(sizeof(tappa_t));
        newNode->distanzaTappa = nuovaTappa;
        newNode->next = current->next;

        // Se il nodo da rimuovere è il primo nodo
        if (prev == NULL) {
            *head = newNode;
        } else {
            prev->next = newNode;
        }

        // Libera la memoria del nodo rimosso
        free(current);
    }
}

// La funzione ausiliaria iterativa che pianifica il percorso tra la stazione di partenza e arrivo per l'andata
tappa_t *pianificaPercorsoAndata(int stazionePartenza, int stazioneArrivo, int partenzaProvvisoria, int autonomiaMax, int *found, tappa_t *head, node_t *root) {
    int autMax = autonomiaMax;
    int stop = 0;
    *found = 0;

    while (1) {
        while (autonomiaMax < stazioneArrivo - partenzaProvvisoria && !stop) {
            node_t *prossimaStazione = findNextNodeAscending(partenzaProvvisoria, root);
            partenzaProvvisoria = prossimaStazione->stazione.distanza;
            if (prossimaStazione->stazione.autonomie == NULL) {
                stop = 1;
            } else {
                autonomiaMax = prossimaStazione->stazione.autonomie[0].num;
            }
            if (partenzaProvvisoria == stazioneArrivo) {
                stop = 1;
            }
        }

        if (!stop) {
            head = inserisciInTesta(head, partenzaProvvisoria);
            stazioneArrivo = partenzaProvvisoria;
            partenzaProvvisoria = stazionePartenza;
            autonomiaMax = autMax;

            if (stazioneArrivo == partenzaProvvisoria) {
                *found = 1;
                break;
            }
        } else {
            *found = 0;
            freeTappe(head);
            head = NULL;
            break;
        }
    }

    return head;
}

// La funzione ausiliaria iterativa che pianifica il percorso tra la stazione di partenza e arrivo quando la stazione di partenza è maggiore di quella di arrivo
tappa_t *pianificaPercorsoRitorno(int stazionePartenza, int stazioneArrivo, int *found, tappa_t *head, node_t *root){
    int stazioneMaxProvvisoria = stazionePartenza;
    int fine = stazionePartenza;
    int trovato = 0;
    int stop = 0;
    int skip = 0;
    int stazioneMax;
    int stazioneNextMax1;
    int stazioneNextMax2;
    int maxProx;

    //Trovo la stazione massima raggiungibile dalla stazione di partenza
    node_t *stazioneMaxRaggiungibile = findFurthestReachableStation(stazionePartenza, root);

    //Se non esiste una stazione massima raggiungibile stampo nessun percorso
    if(stazioneMaxRaggiungibile == NULL){
        *found = 0;
        free(head);
        head = NULL;
        stop =1;
    }else {
        stazioneMax = stazioneMaxRaggiungibile->stazione.distanza;
        maxProx = stazioneMax;
    }

    if(!stop) {
        //Trovo la prossima stazione
        node_t *prossimaStazione = findNextNodeDescending(stazionePartenza, root);
        int stazioneNext = prossimaStazione->stazione.distanza;
        if (findFurthestReachableStation(stazioneNext, root) == NULL) {
            stop = 1;
        } else {
            stazioneNextMax1 = findFurthestReachableStation(stazioneNext, root)->stazione.distanza;
        }

        // Ciclo finchè la stazione massima raggiungibile è maggiore della stazione di arrivo (essendo in ordine decrescente)
        while ((maxProx > stazioneArrivo && !stop)) {
            // Ciclo finchè la prossima stazione è maggiore della stazione massima raggiungibile (essendo in ordine decrescente)
            while (stazioneNext >= stazioneMax) {
                // Entro nell'if se la stazione provvisoria può raggiungere una stazione più lontana della stazione massima raggiungibile precedente
                stazioneMaxRaggiungibile = findFurthestReachableStation(stazioneNext, root);
                if (stazioneMaxRaggiungibile == NULL) {
                    skip = 1;
                } else {
                    stazioneNextMax2 = stazioneMaxRaggiungibile->stazione.distanza;
                }
                if ((stazioneNextMax2 <= stazioneNextMax1 && !skip) || (stazioneNextMax2 > stazioneNextMax1 && stazioneNextMax2 <= stazioneArrivo && !skip)) {
                    stazioneMaxProvvisoria = stazioneNext;
                    trovato = 1;
                    stazioneNextMax1 = stazioneNextMax2;
                }
                // Controllo che esista un altra stazione successiva
                prossimaStazione = findNextNodeDescending(stazioneNext, root);
                skip = 0;
                if (prossimaStazione == NULL) {
                    stop = 1;
                    break;
                }
                stazioneNext = findNextNodeDescending(stazioneNext, root)->stazione.distanza;
            }
            // Esco dal ciclo una volta controllato tutte le stazioni raggiungibili



            // Se ho trovato una tappa la inserisco nella lista e itero il ciclo
            if (trovato) {
                head = inserisciInTesta(head, stazioneMaxProvvisoria);
                stazionePartenza = stazioneMaxProvvisoria;
                stazioneMax = findFurthestReachableStation(stazionePartenza, root)->stazione.distanza;
                trovato = 0;
                // Se non l'ho trovata non esiste un percorso
            } else {
                *found = 0;
                freeTappe(head);
                head = NULL;
                stop = 1;
            }
            // La nuova stazione massima raggiungibile è la stazione provvisoria
            maxProx = findFurthestReachableStation(stazioneMaxProvvisoria, root)->stazione.distanza;
        }

        // Se la stazione massima raggiungibile è minore della stazione di arrivo aggiungo la stazione di arrivo alla lista
        if (!stop) {
            head = inserisciInTesta(head, stazioneArrivo);
            head = inserisciInCoda(head, fine);
            *found = 1;
            // Controllo se il percorso ha le stazioni più vicine al km 0
            head = verificaPercorso(head, root, fine);
        }
    }

    // Ritorno la lista
    return head;
}
// Sottoprogramma per pianificare il percorso le stazione di partenza e di arrivo sono sicuramente presenti nell'albero
void pianificaPercorso(int stazionePartenza, int stazioneArrivo) {
    int found = 0;
    node_t *root = autostrada.root;
    tappa_t *tappeFatte = NULL;
    int stop = 0;

    // Se la stazione di partenza è uguale a quella di arrivo stampa la stazione di partenza
    if(stazionePartenza == stazioneArrivo) {
        printf("%d\n", stazionePartenza);
        return;
    }

    // Cerca la stazione di partenza e salva l'autonomia massima
    node_t *nodoPartenza = findNode(stazionePartenza, root);
    if(nodoPartenza->stazione.autonomie == NULL){
        stop = 1;
    }

    if(!stop) {
        int autonomiaMax = nodoPartenza->stazione.autonomie[0].num;

        // Controllo se la stazione di partenza è minore o maggiore di quella di arrivo
        if (stazionePartenza < stazioneArrivo) {
            tappeFatte = inserisciInTesta(tappeFatte, stazioneArrivo);
            tappeFatte = pianificaPercorsoAndata(stazionePartenza, stazioneArrivo, stazionePartenza, autonomiaMax, &found, tappeFatte, root);
        } else {
            tappeFatte = pianificaPercorsoRitorno(stazionePartenza, stazioneArrivo, &found, tappeFatte, root);
        }
    }
    // Stampa il percorso se è stato trovato
    if (found) {
        tappa_t *tmp = NULL;
        while (tappeFatte) {
            printf("%d", tappeFatte->distanzaTappa);

            // Se l'elemento successivo esiste, stampa uno spazio
            if (tappeFatte->next != NULL) {
                printf(" ");
            }

            tmp = tappeFatte;
            tappeFatte = tappeFatte->next;
            free(tmp);
        }
        printf("\n");
    } else {
        printf("nessun percorso\n");
    }

}

// Sottoprogramma per verificare se il percorso tra la stazione di partenza e quella di arrivo ha le stazioni piu vicine al km 0
tappa_t *verificaPercorso(tappa_t *head, node_t *root, int fine){
    tappa_t *tmp, *tmp2, *tmp3;
    node_t *nodo, *nodo2;

    nodo = findNode(head->distanzaTappa, root);
    tmp = head;
    tmp2 = tmp->next;
    tmp3 = tmp2->next;

    while(tmp2->distanzaTappa != fine){
        nodo2 = findNode(tmp3->distanzaTappa, root);
        while (nodo->stazione.distanza != tmp2->distanzaTappa){
            nodo = findNextNodeAscending(nodo->stazione.distanza, root);
            if (nodo->stazione.distanza - nodo->stazione.autonomie->num <= tmp->distanzaTappa && nodo->stazione.distanza >= nodo2->stazione.distanza - nodo2->stazione.autonomie->num){
                replaceNode(&head, tmp2->distanzaTappa, nodo->stazione.distanza);
                tmp = tmp->next;
                tmp2 = tmp3;
                tmp3 = tmp3->next;
                break;
            }
        }
    }

    // Inverto la lista e la ritorno
    head = invertiLista(head);
    return head;
}
