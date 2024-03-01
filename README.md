# Introduzione

Libri ti testo:

1. Engineering a Compiler Cooper, Torczon, Elsevier

2. Compilatori: principi, tecniche e strumenti- seconda edizione Aho, Lam, Sethi, Ullman edizione Pearson. **ce l'abbiamo**



Obiettivi del corso:

- conoscere i principi di base della costruzione di un compilatore moderno, concetto di ottimizzazione

- implementazione concreta dell'ottimizzazione in un reale compilatore LLVM.



Esame:

- svolgimento e la presentazione di varie attività progettuali da svolgere nel corso del semestre sottoforma di *assignments*. 

- una prova orale, durante essa si discuteranno gli assignements sia sulla parte teorica che quella pratica. 

---



I computer in qualsiasi forma sono **pervasivi** e onnipresenti. Lo studio di questi sistemi è fondamentale in un mondo e mercato dominato da essi .



Nel seguente schema di sotto è presentata l'astrazione dei calcolatori:

![](/home/gerti/.var/app/com.github.marktext.marktext/config/marktext/images/2024-03-01-09-50-49-image.png)

Lo schema si legge partendo dal basso verso l'alto. 

Nella ISA abbiamo lavorato con RISC-V. Ci aiuta a sapere com'è progettata la CPU.

**ISA**

E' un elenco di tutte le istruzioni che la CPU riesce ad eseguire. Ci vengono date indicazioni su come le istruzioni sono eseguite. 

- Esempio: Register file, un file dove vengono salvati i registri e la ISA dice quanti registri e come sono messi. 

La ISA ci indica come per esempio i 32bit devono essere strutturati/impacchettati di modo che possiamo avere delle istruzioni. 



I *programmi astraggono la ISA*. 



Nel corso di compilatori parliamo di qualcosa che avviene tra i Programmi e ISA.



***A cosa servono i compilatori***: obiettivo di trasformare il codice da un linguaggio all'altro. Abbassare quindi l'astrazione da un linguaggio di alto livello ad uno più basso.



#### Struttura del compilatore

![](/home/gerti/.var/app/com.github.marktext.marktext/config/marktext/images/2024-03-01-10-05-10-image.png)

Il compilatore è un pezzo della toolchain che ci porta da un linguaggio di alto livello a assembly.

**Assembler**

Come minimo deve avere un assembler che prende un codice sorgente e produce codice macchina, genera un file oggetto che dice in vari segmenti dove i dati del programma sono inseriti. 

Posso aver messo delle librerie e quindi aver spezzato il mio codice, quindi posso avere più file oggetto. 

**Linker**

risolve tutte le chiamate di funzione e dei simboli coinvolti. Potremmo avere dei file di input ulterirori come file di **startup** come il main che deve essere il primo programma. Si chiamano startup code che sono delle funzioni  che fa una serie di operazioni di base prima di invocare il main. Il linker quindi lo include. 



Include le librerie come file oggetto.



Posso dare ulteriori info come nel Linker script che mi permette di dire degli indirizzi di memoria si trovi il codice o vari tipi di dato. 



Il *linker genera* un file eseguibile Binario perché ogni volta che esegue un programma la cpu fa un fetch di 32bit. Possiamo fare anche passaggio inverso e avere un Disassembled code. 



Possiamo inserire nel nostro schema anche un pre-processore che estende le macro e quindi cose che vanno fatte prima della compilazione vera e propria. 



#### Ottimizzare il codice

Vogliamo migliorare la performance, quindi che il programma ci metta meno tempo ad eseguire. 

- oppure che occupi meno spazio

- migliore efficienza energetica

- perché sfrutti determinate caratteristiche architetturali (sfrutti la cache)





I sistemi di calcolo evolvono e con esso devono evolvere anche i compilatori. 

All'epoca non si riusciva a diminuire le dimensioni e migliorare performance senza surriscaldare.



**LEGGE DI MOORE**: Il numero di transistor in un circuito integrato (IC) raddoppia ogni 18 mesi. Ogni 18 mesi o mi trovavo col doppio di transistor o con circuito il doppio in meno.

Notiamo però che all'aumentare dei transistor aumenta la frequenza. Avendo però tanti transistor in un punto piccolo, molta energia si dissipa e quindi crea calore. 

Quindi ho molta densità di potenza che però mi dissipava tanto calore, questo si chiama il Power Wall. 



La *soluzione temporanea al problema* è tenere la frequenza in stallo, ma aggiungo più cpu dentro -> multicore cpu.

Dal 2004 comincia l'era del multicore. Meglio sfruttare tanti core con frequenze più basse che solo 1 con frequenza alta. I consumi di potenza sono legati alla frequenza, quindi le teniamo uguali. 

![](/home/gerti/.var/app/com.github.marktext.marktext/config/marktext/images/2024-03-01-10-19-37-image.png) 

Guardare la performance. Rimane fissa. Questo perché l'era multicore è legata al singlecore e i programmi sono scritti in singlecore. L'archietetture è rimasta con la misura che è legata al single core. 



Il problema rimane che abbiamo tutto sto codice legacy e non possiamo buttarlo. Si studiano i compilatori per capire come prendere un programma che esegue su un core  e eseguirlo su multicore. I compilatori moderni stanno introducendo tecniche *auto-parallelizzanti*. Purtroppo però non si è arrivato a produrre dei tool che fanno bene sta cosa. Inoltre è difficile fare il parallelismo e sopratutto creare un tool che ti dica dove sta la parallelizzazione nel programma.



Possibile soluzione: supporto da parte dei compilatori ai *parallel programming models*. 

Cerco di rendere meno traumatico il passaggio nel quale io so scrivere un programma e scriverlo per il parallelismo. Rendo quindi disponibile un'interfaccia semplice verso il **parallelismo**. Il compilatore traduce i semplici costrutti dell'interfaccia in codice parallelo eseguibile.



Programmi come Cuda di NVIDIA, oppure *OpenMP* per il c/c++ e fortran. Scritto il programma C, si aggiungono delle notazioni come `#pragma omp`.



Esempio:

```c
#pragma omp parallel for
for  (i=0; i<n; i++){
    ...
}
```

Il programmatore quindi non deve concentrarsi  su come il compilatore esegue in parallelo. Quindi richiede magari usare un libreria che usa i *pthread* e poi fare una p_thread create che ha bisogno di un puntatore ad una funzione parallela dove le dici com'è fatta una funzione. 



**RUNTIME ENVIRONMETS** : è una libreria che linkiamo che contiene una sorta di funzioni che hanno info sullo stato della nostra programmazione parallela.

Vogliamo quindi aiutare a fare la transizione in modo facile da un concetto di programmazione singlecore a uno multicore.



Introduciamo un nuovo concetto.

L'altro aspetto che ci ha consentito di incrementare la performance: **eterogeneità architetturale**. Siamo passati da uno *omogeneo* per esempio di un multicore da x86, ho 4 processori x86 che sono uguali.

Possiamo usare una CPU general purpose, possiamo integrarla con cpu che sono brave a fare una cosa in particolare : GPU. è molto brava a fare calcoli su triangoli e altro.

Nel corso degli anni le GPU sono diventate GPGPU, gpu moderne che fanno general purporse parallel programming gpu --> gpu integrata.



Possono avere anche acceleratore per le neural networks TPU (Tensor Processing Units). 



### Ottimizzazione

Come i compilatori migliorano le performance.

Migliorare significa ridurre il tempo di esecuzione

$$
Performance = \frac{1}{ExcecutionTime}
$$

$$
ExecutionTime= \frac {InstructionCount \times CPI}{Frequency}
$$

CPI: Clock Cycles per Instruction, misura media di quanti cicli ci metto per un'istruzione.



Nelle Load e Store devi mettere Hit rate in cache. 

Migliorabile da programmatore è solo l'Istruction Count minimizzando il numero di istruzioni, operazioni aritmetiche, acessi in memoria.

Oppure Rimpiazzando *mul* con shift left logical immediate.



#### esempi di ottimizzazione

Compile time : tempo di compilazione

Excecution Time: mentre sto eseguendo.



Semplificazioni algerbiche:

`b or true ---> true`



Constant folding, eseguo a compile time `c = 1+3 ---> c=4`



**Strenght reduction**

Per la moltiplicazione faccio la somma `y=x*2 --> y=x+x`.

`y = x*17 --> y = (x << 16) +x`, equivale a sommare 17 volte, le 16 le trasformo in uno shift. 

La moltiplicazione è tipicamente un'operazione multi-ciclo. Cerchiamo di farne il meno possibile. 

- dobbiamo vedere se la trasfomazione è valida quindi non altera la semantica

- oppure se ne vale la pena



Nell'esempio dell'array , è un intero, quindi offset 4.



![](/home/gerti/.var/app/com.github.marktext.marktext/config/marktext/images/2024-03-01-10-58-23-image.png)

Ho tirato via la mul sostituendola col +, ho tolto a[i] col puntatore. 




