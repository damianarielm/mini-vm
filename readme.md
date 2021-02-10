# Mini Maquina Virtual #

## Dependencias y compilacion ##

Para poder compiar el programa necesitará el compilador de C *gcc*, el
generador de parsers *bison* y el generador de lexers *flex*; además de la
herramienta de compilación *make*.

Si utiliza *Ubuntu*, puede instalar todas estas dependencias con la
instrucción:

```bash
sudo apt install gcc bison flex make
```

Luego bastará con ejecutar:

```bash
make
```

## Instrucciones de uso ##

Si dispone del programa ya compilado puede utilizar la siguiente instrucción
para correr el programa:

```bash
./machine programa.asm
```

## Descripción de la arquitectura ##

La arquitectura *LCC_32* es una arquitectura de tamaño *int* (muy probablemente
32 bits). Posee una unidad principal, una unidad de depuración, un sistema de
segmentación, una memoria de 1024 *ints* y 8 registros de tamaño *int*: cuatro
de ellos de propósito general y los restantes de uso específico.

### Instrucciones ###

* NOP: No realiza ninguna operación.
* MOV: Mueve un valor de/hacia registro/memoria/constante.
* SW: Guarda el valor de *src* en la dirección de memoria *dst*.
* LW: Carga en *dst* el valor de la memoria apuntada por *src*.
* PUSH: Escribe el argumento *src* en el tope de la pila, decrementando *SP* en 4.
* POP: Escribe en el argumento *dst* el tope de la pila, incrementando *SP* en 4.
* CALL: Guarda en la pila el valor del registro *PC* y salta a la etiqueta indicada.
* RET: Escribe en el registro *PC* lo que saca del tope de la pila.
* PRINT: Imprime por pantalla el entero descripto en el argumento *src*.
* READ: Lee un entero y lo guarda en el argumento destino.
* ADD: Suma los argumentos *src* y *dst* y lo guarda en *dst*.
* SUB: Resta los argumentos *src* y *dst* y lo guarda en *dst*.
* MUL: Multiplica los argumentos *src* y *dst* y lo guarda en *dst*.
* DIV: Divide los argumentos *src* y *dst* y lo guarda en *dst*.
* AND: Efectura una conjunción bit a bit entre los argumentos *src* y *dst* y lo guarda en *dst*.
* CMP: Compara *src* y *dst* y setea los bits correspondientes en el registro *FLAGS*.
* JMP: Salta a la instrucción número *dst* de la lista de instrucciones.
* JMPE: Salta a la instrucción número *dst* si la bandera de igualdad esta seteada.
* JMPL: Salta a la instrucción número *dst* si la bandera de menor esta seteada.
* DMP: Imprime por pantalla un volcado de registros y memoria.
* DBG: Invierte el estado de la bandera de depuración.
* SEG: Invierte el estado de la bandera de segmentación.
* HLT: Termina el programa.

### Registros ###

* Registros de propósito general:
  * R0.
  * R1.
  * R2.
  * R3.

* Registros de uso especifico:
  * ZERO (siempre vale cero).
  * PC (contador del programa).
  * SP (puntero al tope de la pila).
  * FLAGS (banderas de estado).

### Convención de llamada ###

La convención de llamada para la arquitectura *LCC_32* establece que todos los
registros son caller saved. Además los primeros cuatro argumentos son pasados
en los registros *R0*, *R1*, *R2* y *R3*; y los restantes en la pila, dejando
en el tope el quinto argumento.

### Ciclo de ejecución ###

### Sistema de segmentacion ###

### Unidad de depuración ###
