### Laborator: Gestiunea memoriei

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
<static> var
	* var este initializata o singura data

<const> 

##### Sectiunea de date
* Impartita in 4 subsectiuni

<.data>

<.bss>

<.rodata>

<.text>