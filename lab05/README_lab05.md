# Laborator: Gestiunea memoriei

### Linux
Afiseaza tabela de simboluri. (l - variabile locale; g - variabile globale)
```
	objdump -t adr_space
```

Afiseaza spectiunea de date specificata de <sectiune>:
```
	objdump -s -j <sectiune> exefile
	readelf -x <sectiune> file.o
```
* should prefer readelf when possible, objdump don't show some sections like .symtabl.
More at:
https://stackoverflow.com/questions/22160621/why-does-objdump-not-show-bss-shstratab-symtab-and-strtab-sections

**static** var
* var este initializat o singura data

**const** var
* var nu vrei sa fie modificat in program
* var este alocat in .rodata daca e var e declarat gloabl/static
* daca var e declarat in functie(aici intra si main) atunci e alocata pe stack, vrei primi eroare de compilare cand vrei o sa modifici (asta nu mereu, depinde de caz)

#### char *var vs char var[]
* char *var = "Memory" va fi alocat in .rodata, daca il modifici va da SegFault
* char var[] = "Memory" e vector de char, va fi alocat in .rodata, poti sa il modifici

#### Sectiunea de date
Impartita in 4 subsectiuni:

**.data**
* conține variabilele globale și variabilele statice inițializate

**.bss**
* conține variabilele globale și variabilele statice neinițializate sau initializate cu 0

**.rodata**
* conține informație care poate fi doar citită, nu și modificată

**.text**
* sectiune de cod cu primisiunile r-x
