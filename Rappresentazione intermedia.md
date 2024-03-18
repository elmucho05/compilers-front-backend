# Rappresentazione intermedia

Il middle-end è organizzato come una sequenza di *passi*.

Per analizzare il codice e trasformarmo, occorre una IR espressiva che mantenga tutte le informazioni importanti da *trasmettere* da un passo all'altro. Quindi ad ogni passo la IR cambia.

*Proprietà di una IR*

- facilità di generazione

- facilità di manipolazione

- costo di manipolazione

- livello di astrazione, è imporante perché quando arrivo a livello basso, è importante capire bene e vedere se è corretta.

- livello di dettagli esposto

Sottili decisioni di progetto di una IR possono avere effetti molto intricati sulla velocità ed efficienza di un compilatore. 

### Tipi di IR

- AST 

- Directed acyclic graphs DAG

- 3-address code (3AC)

- Stati single assignment (SSA)

- Call graph (CG)

- Program dependece graphs (PDG)

#### Categorie di IR

**Grafiche**: AST, DAG 

- orientati ai grafi

- molto sate nella source-to-source translation

- tendono ad essere voluminose

**Lineari** : 3 address code e SSA

- pseudocodice per macchine astratte, è meno vincolante, ho un numero di registri infinito, nel backend penserò di tradurre questi registri virtuali

- il livello di astrazione varia

- strutture dati semplici e compatte

- facile da riarrangiare

**Ibride**: control flow graph

#### Sintassi concerta o astratta

**Concreta**: sintassi semplice e vicina agli umani. Ma pessima per analizzare e trasformare codice.

#### AST

La sintassi è astratta, cerco di descrivere ogni cosa come un albero/grafo. Ogni nodo può avere figli e vedi qualcosa di regolare che posso applicare a tutte le istruzioni.

Per ognuna delle tipologie di istruzioni, i nodi figli sono diversi fra loro. 

Ma non è conveniente perché ci sono tantissimi tipi di nodo.

I vari rami descrivono come gli operandi descrivono le varie cose delle mie espressioni. C'è una regolarità ma è molto macchinoso.

#### DAG

è un AST ma che evita la dupilcazione dell'espressione. Es, il *b-c* appare due volte nell'albero. Potrebbe essere conveniente per l'ottimizazione *Common Subexpression Elimination*

![](/home/gerti/.var/app/com.github.marktext.marktext/config/marktext/images/2024-03-04-15-34-56-image.png)

Ma la *Common Subexpression Elimination* richiede che nessuno degli operandi abbia cambiato valore, per cui c'è bisogno di sapere quando una cosa cambia valore. Un DAG non sa quando nel tempo verranno eseguiti quelli stmt. 

Appunto il DAG potrebbe essere ambiguo e quindi sbagliato, perché se usi più volte una var, **il DAG non ha modo di valutare in che modo/ordine le istruzioni sono eseguite**.

#### 3 Address code

Le istruzioni hanno la forma x=y op z, dove un operatore ha un massimo di 3 operandi. 

PRO:

- Espressioni complesse vengono spezzate

- Forma compatta e molto simile all'assembly

- registri virtuali

![](/home/gerti/.var/app/com.github.marktext.marktext/config/marktext/images/2024-03-04-15-42-11-image.png)

Dal punto di vista di rappresentare ogni singola espressione:

![](/home/gerti/.var/app/com.github.marktext.marktext/config/marktext/images/2024-03-04-15-45-02-image.png)

Per le quadruple, ce la caviamo con una semplice struct di 4 attributi.

La tripla, associa l'operando destinazione all'indice dell'istruzione. 

Una delle cose che questa rappresentazione migliora è la **constant propagation**. La forma 3 address code, non ci dà troppe informazioni

![](/home/gerti/.var/app/com.github.marktext.marktext/config/marktext/images/2024-03-08-09-43-06-image.png)

Per esempio se ci fossero due dichiarazioni della variabile **b**, il compilatore non ha modo nella forma di base per capire quale sia l'ultima definizione della variabile.

- Soluzione: algoritmo di valute numbering --> SSA (Single Static Assignment). è una forma di 3address code che ci da garanzia che ogni var del programma sia definita una volta sola.

#### SSA

*è una forma di 3address code che ci da garanzia che ogni var del programma sia definita una volta sola.*

- Ogni definizione ha associata una lista di tutti i suoi usi.

- ogni variabile che opera su un'istruzione è un uso di qualche definizione, ed è essa direttamente collegata.

- Semplifica analisi e trasformazione del codice.

![](/home/gerti/.var/app/com.github.marktext.marktext/config/marktext/images/2024-03-08-09-47-32-image.png)

### Scelta della IR

Non esiste una risposta per ogni cosa ma dipende da quello che vogliamo fare. In questo corso ci occupiamo di trasformazione e ottimizzazione. Di norma non basta una sola forma IR, perché possiamo approcciare la IR sotto vari punti di vista. 

##### Control Flow graph(CFG)

Modella il trasferimento (flusso) del controllo di un programma tra **blocchi di istruzioni.** I suoi nodi sono dei **basic blocks** che contengono una sequenza lineare di istruzioni, terminata da un'istruzione di trasferimento del controllo.

I suoi archi rappresentano il flusso di controllo (loop, if,else, goto, ecc)

Le istruzioni distano fra loro di 4 byte. Quindi il PC aumenta di 4 per ogni istruzione. 

La seconda ALU, prende l'immediato(la label dove slatare) e lo moltiplica per 2 con *slli --> immediato<<1*

L'insime di istruzioni è una *Linked list* connessa in più punti. Sopra ci aggiungiamo la CFG dicendo che la lista è un insieme di BB che sono *sequenze di isturuzioni*. 

Esempio, un BB, finisce con **un'istruzione di exit**.

il CFG è un grafo dove i nodi sono BB e archi rapprsentano il trasferimento di controllo 

**DEF**: un BB è una sequenza di istruzioni in forma 3AC.

- solo la prima istruzione può essere raggiunta dall'esterno, gli archi non possono puntare a un'istruzione nel mezzo del blocco --> **single entry point**

- tutte le istruzioni nel blocco sono eseguite se viene eseguita la prima, quindi non si può eseguire un'istruzione di salto se non come ultima istruzione del blocco, single EXIT point.

Se due blocchi uno dopo l'altro, quello prima non ha un salto, dopo tocca al PC dire che deve passare al prossimo blocco --> **arco fallthrough**.

Un CFG normalizzato se ha i BB massimali, ovvero che quando l'unica ragione per cui esiste un arco fallthrough è per non violare. 

##### **Algoritmo per la costruzione del CFG**

1. Identificare il LEADER di ogni BB
   
   1. la prima istruzione
   
   2. il target di un salto
   
   3. ogni istruzione che viene dopo un salto

2. Il basic block comincia con il leader e termilan com l'istruzione immediatamnete precedente ad un nuovo LEADER

3. Connettere i basic blocks tramite archi di tre tipi
   • fallthrough (o fallthru): esiste solo un percorso che collega i due blocchi
   • true: il secondo blocco è raggiungibile dal primo se un condizionale è TRUE
   • false: il secondo blocco è raggiungibile dal primo se un condizionale è FALSE

![](/home/gerti/.var/app/com.github.marktext.marktext/config/marktext/images/2024-03-08-10-09-09-image.png)

NOTA BB5, contine l'if perché non ha una label.

L'arco del BB1 ha solo un arco di fallthrough.

IL BB2 ha due archi: archo TRUE e FALSE(fallthrough).

#### Dependency Graph

I nodi di un DG sono istruzioni, che usano dei valori e ne definiscono un altro

Gli archi connettono due nodi, uno dei quali usa i risultati definiti dall'altro.

Ordina le istruzioni (**instruction scheduling**) in modo da evitare i stalli.

#### Data pedendency Graph

La dipendenza è sul dato, non analizzo più l'operando, quindi non guardo più due o più istruzioni e vedere se un'istruzione scrive un registro e l'altra lo legge.

Esempio: capire se un loop può essere parallellizato.

Verificare che le iterazioni siano indipendenti di modo che posso far eseguire varie istruzioni vari processori.

- tipicamente c'è una dipendenza se faccio A[i] = A[i-1], c'è una dipendenza perché uno legge e uno scrive.

Il polyedral model è una sorta di DDG avanzato che tratta ogni iterazione del oop come un punto all'interno di un modello matematico che semplifica la trasfomazione dello spazio delle iterazioni.

#### Call graph

è una forma che ci serve quando la nostra analisi va a guardare il programma nella sua completezza.

![](/home/gerti/.var/app/com.github.marktext.marktext/config/marktext/images/2024-03-08-10-32-44-image.png)

Cosa succede se il mio programma è diviso in più file?

- interviene un framewark che fa la WPA(Whole Program Analysis) e gestisce la cose a livello Linker --> Link File Optimization (LTO).
