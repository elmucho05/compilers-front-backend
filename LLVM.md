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



:
