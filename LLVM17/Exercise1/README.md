Per generare codice IR:
```sh
clang -O2 -emit-llvm -S -c Loop.c -o Loop.ll
```
Oppure possiamo anche generare l'assembly:
```sh
clang -O2 -emit-llvm -c Loop.c -o Loop.bc
```
