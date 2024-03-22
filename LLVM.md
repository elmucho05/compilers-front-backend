# LLVM

Creare struttura della slide 1, il file zip lo spacchetti in SRC.
Ti sposti in build e fai `cmake -g`.

Poi usi make

Poi `make install`

## Passi LLVM

Abbiamo quasi una corrispondenza 1 a 1 con un linugaggio Assembly.

Andremo a manipolare il concetto di

- **Modulo**: concetto di file, per ogni file c'è un modulo LLVM. All'interno di un modulo posso ispezionare tutte le funzioni. Dentro le funzioni posso ispezionare il CFG e quindi i BB e dentro i BB le istruzioni, dentro le istruzioni i operandi

- **Iteratori** : come attravverso il modulo, la descrizione sopra

- **Downcasting**: i vari nodi usano l'ereditarietà. 

### Modulo LLVM

Il nostro programma ha :

- **Files**: Module, lista di Function e variabili globali

- **Funzioni** : la lista di BB e argomenti, mi permette di scorrere la lista di argomenti

- **BasicBlock**: l'iteratore mi permette di scorrere le istruzioni

- **Istructions**: per controllare optcode(add,sub,mul) e gli operandi.

#### Iteratori

```cpp
Module &M
for(auto iter = M.begin(); iter!= M.end(); ++iter){
    Funtion &f = *iter;
}
```

Risciamo ad attraversare il modulo tramite le double linked lists.

PArto dalla istruzione e fare **Downcasting** sulla classe.

```cpp
Instruction *inst = ...
if (CallInst *call:usnt = dyn_cast<CallInst>(inst))
```

Scendi giù fino a trovare l'istruzione.

**Interfacce dei passi LLVM (pass manager)**:

Le tipologie di passi osno varie di cui le principali sono le :

- Module passes: come passo ho l'intero modulo

- FunctionPAsss : itera sulla lista delle funzioni del modulo

- LoopPass : itera sui loops, in ordine inverso di nesting

- RegionPass: itersa sulle Single Entry Single Exit regions, in ordine inverso di nesting. Da cui possono costruire un grafo di regions fatte di BB.

Il nuovo Pass Manager 

Normalmente noi usiamo `clang` che mi permette di chiamare le varie parti della toolchain.

se facciamo `opt` senza comandi, applica i passi in modo default. possiamo quindi dire la lista dei passi :

`opt -pass1 -pass2 -pass3 x.bc -o y.bc`

L'opzione -O0 ti dice di non ottimizzare. Aggiungendo il flag `-Rpass=.*` dicendo in modalità verbose quali passi sono stati invocati con -O2 e con -O0.

Se analizzi il codice della funzione loop, avendo dato il flag -O2, abbiamo una IR ottimizzata. Notiamo che da nessuna parte compare la chiamta della funzione `g_incr()`

`clang -O2 -Rpass=.* Loop.c `. Essendo **clang** un driver, esegue tutta la toolchain.

Abbiamo un errore di *linking* il che ci dice che manca il main. Se ci fossimo perfati alla fase di generazione di codice non avremmo avuto problemi.

`-S` : si ferma alla compilazione --> da in output un file assembly

`-c` : genera un file oggetto.

**Passi applicati**

Notiamo dalla modalità verbose che il passo che ottimizza il codice, ha rimosso la chiamata e l'ha espansa col suo codice. E' questo il motivo per cui non vediamo più la chiamata a funzione.

*LoopInvariantCodemotion*

Cerca stmt che sono invarianti dal loop e sposta i statement fupri dal loop, cosiché non eseguiamo codice inutile tante volte.

Se confrontiamo il codice prodotto da -O2 e -O0, vediamo che ci sono più ottimizzazioni e che in O0 appare la chiamata a funzione.

#### Scrivere un Passo LLVM

A partire dalla versione 17.0.6, tutti i passi ereditano da PassInfoMix

Per ogni passo dobbiamo creare un file header e un file c++.

Il file c++ dobbiamo inserirlo in `$ROOT/src/llvm/include/lib/Transforms/utils/`

Creare il file TEstPASS.cpp

```c
#ifndef LLVM_TRANSFORMS_TESTPASS_H
#define LLVM_TRANSFORMS_TESTPASS_H
#include "llvm/IR/PassManager.h"
namespace llvm {
class TestPass : public PassInfoMixin<TestPass> {
public:
PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
};
} // namespace llvm
#endif // LLVM_TRANSFORMS_TESTPASS _H
```

```cpp
#include "llvm/Transforms/Utils/TestPass.h"
using namespace llvm;
PreservedAnalyses TestPass::run(Function &F,
FunctionAnalysisManager &AM) {
errs() << Questa funzione si chiama << F.getName() << "\n";
return PreservedAnalyses::all();
}
```

Ricordandoci la struttura, abbiamo Modules --> Functions --> CFG (BB) --> Instructions.

Noi abbiamo creato un passo che lavora su Functions, il pass manager invoca il metodo run per la funzione Loop e g_incr.

Sto invocando il metodo getname sull'oggetto getfuncions

Dobbiamo modificare il file CMakeLists.cpp, aaggiungendo il file TestPass.cpp

Il MODULE_PASS, tratta i tipi di parametri di funzione, se come argomento ci hai passato una function, non andrà mai, perché lui capisce che si aspetta una funzione.

#### I passi di LLVM

L'ottimizzatore del middle-end è l'opt. 

La IR usa la forma SSA, Static Single Assignment, per la quale una var,non può essere definita più di una volta. Crei una nova versione della variabile una volta che incontro una nuova *definizione* di essa.

- *posso rimuoverla perché non ha **usi**.*  

La notazione SSA è molto importante per capire quale parte di codice è DEAD CODE, e quindi la posso eliminare. 

*Come llvm implementa la forma SSA*?

La gerarchia di classi, sfrutta una rappresentazione del tipo *User - Use - Value*

```assembly
%2 = add %1, 0
%3 = mul %2, 2
```

SE tolgo la prima riga, la seconda riga non trova %2. Devo quindi aggiornare gli USI che coinvolgono le istruzioni nel Basic Block.

`%3 = mul %1, 2` --> aggiorno il puntatore alla variabile che devo usare. 

Le *Instruction* LLVM ereditano dalla classe Value.

Ma ereditano anche dalla classe *User*. Quindi esiste una legame tra instruction e i suoi usi.

![](/home/gerti/.var/app/com.github.marktext.marktext/config/marktext/images/2024-03-22-10-04-42-image.png)



Per vedere gli usi delle istruzioni

```assembly
User &Inst = …
for (auto Iter = Inst.op_begin(); Iter != Inst.op_end(); ++Iter)
{ Value *Operand = *Iter; }
Se eseguo questo codice per analizzare l’istruzione
%2 = add %1, 0
verranno estratti gli operandi
%1, 0
```

La parte diversa è quella degli USEE, usanti.

Una cosa diversa da come interpretiamo noi, una Intruction è anche uno Usee.

- la riposta sta nel fatto che noi %2 = add %1, 0

- la macchina la intepresta come : %2 è la rappresentaizone Value dell'istruzione add %1, 0



Se la fun usa qualcosa:

```assembly
for (auto Iter = Inst.op_begin(); Iter != Inst.op_end(); ++Iter)
{ Value *Operand = *Iter; }
```



Se invece sono usato da qualucuno, scorro la lista dei miei USER, chi mi usa

```assembly
for (auto Iter = Inst.user_begin(); Iter != Inst.user_end(); ++Iter)
{ User *InstUser = *Iter; }
→ Instruction mul %2, 2 (oppure Value %3)
```

Se la stampo come Value, ti esce la %3. 

```assembly
y = p + 1;
y = q * 2;
z = y + 3;
```

LLVM fa la forma SSA, quindi quando ridefinisci y, avrai y1 e y2.
