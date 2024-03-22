# Guida

1. Prima di tutto dobbiamo registrare il nostro passo LocalOpts come
   abbiamo imparato a fare
   
   - Copiando dove opportuno LocalOpts.cpp
     `(SRC_ROOT/lib/Transforms/Utils)`
   
   - Aggiungendo LocalOpts.cpp al file `CMakeLists.txt` nella stessa
     cartella
   
   - Creando il file LocalOpts.h
     `(SRC_ROOT/include/llvm/Transforms/Utils)`
   
   - Editando `PassRegistry.def `e`PassBuilder.cpp`
     `(SRC_ROOT/lib/Passes)`

2. A questo punto possiamo ricompilare e installare opt
   
   - spostandoci in ROOT/BUILD
   
   - `make opt`
   
   - `make install`

3. Essendo un passo di trasformazione la IR verrà modificata, e quindi
   dobbiamo specificare l’output file
   `opt –p localopts Foo.ll -o Foo.optimized.bc`

4. L’output è in formato bytecode, quindi dobbiamo usare il disassembler
   per generare la forma .ll leggibile
   `llvm-dis Foo.optimized.bc -o Foo.optimized.ll`


