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

La carpeta *examples* contiene programas de prueba.

## Descripción de la arquitectura ##

La arquitectura *LCC_32* es una arquitectura de tamaño *int* (muy probablemente
32 bits). Posee una unidad principal, una unidad de depuración, un sistema de
segmentación, una memoria de 1024 *ints* y 8 registros de tamaño *int*: cuatro
de ellos de propósito general y los restantes de uso específico.

### Instrucciones ###

* nop: No realiza ninguna operación.
* mov: Mueve un valor de/hacia registro/memoria/constante.
* sw: Guarda el valor de *src* en la dirección de memoria *dst*.
* lw: Carga en *dst* el valor de la memoria apuntada por *src*.
* push: Escribe el argumento *src* en el tope de la pila, decrementando *SP* en 4.
* pop: Escribe en el argumento *dst* el tope de la pila, incrementando *SP* en 4.
* call: Guarda en la pila el valor del registro *PC* y salta a la etiqueta indicada.
* ret: Escribe en el registro *PC* lo que saca del tope de la pila.
* print: Imprime por pantalla el entero descripto en el argumento *src*.
* read: Lee un entero y lo guarda en el argumento destino.
* add: Suma los argumentos *src* y *dst* y lo guarda en *dst*.
* sub: Resta los argumentos *src* y *dst* y lo guarda en *dst*.
* mul: Multiplica los argumentos *src* y *dst* y lo guarda en *dst*.
* div: Divide los argumentos *src* y *dst* y lo guarda en *dst*.
* and: Efectura una conjunción bit a bit entre los argumentos *src* y *dst* y lo guarda en *dst*.
* cmp: Compara *src* y *dst* y setea los bits correspondientes en el registro *FLAGS*.
* jmp: Salta a la instrucción número *dst* de la lista de instrucciones.
* jmpe: Salta a la instrucción número *dst* si la bandera de igualdad esta seteada.
* jmpl: Salta a la instrucción número *dst* si la bandera de menor esta seteada.
* dmp: Imprime por pantalla un volcado de registros y memoria.
* dbg: Invierte el estado de la bandera de depuración.
* seg: Invierte el estado de la bandera de segmentación.
* hlt: Termina el programa.

Por ejemplo, el siguiente programa lee un numero por telcado e imprime su
valor absotulo:

```asm
read %r0
cmp $0, %r0
jmpl end
mul $-1, %r0
end:
print %r0
hlt
```

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

Luego de ser inicializados todos los registros y memoria en 0, se procede a
cargar el programa en memoria. A continuacion se le entrega el control a la
unidad princial que dará comienzo al ciclo de ejecución propiamente dicho:

1. Si la bandera de depuración esta encendida se le da control a la unidad de
   depuramiento.
2. Si la bandera de segmentación está encendida se verifica que la instrucción
   indicada por el *PC* pertenezca al segmento de código.
3. Cuando todo lo anterior termina, se procede a ejecutar la instrucción
   correspondiente y luego se incremente el registro *PC*.

La ejecución de la máquina termina cuando ocurre algún error, o se ejecuta la
instrucción HTL.

### Sistema de segmentacion ###

### Unidad de depuración ###
