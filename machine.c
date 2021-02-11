#include <stdio.h>       // fopen, fclose
#include <stdlib.h>      // exit
#include <string.h>      // strcmp, memcpy, memset

#include "parser.tab.h"  // yyparse
#include "machine.h"
#include "debug/debug.h"

// Variables globales
int count; // Lineas de codigo
struct Instruction code[512]; // Arreglo de instrucciones
struct Machine m;
struct Machine* machine = &m;
extern FILE* yyin;

int origen(struct Operand o) {
  switch (o.type) {
    case IMM:
      return o.val;
    case REG:
      return machine->reg[o.val];
    case MEM:
      if (o.lab)
        return machine->memory[machine->reg[o.val]];
      else
        return machine->memory[o.val];
  }
}

int* destino(struct Operand o) {
  switch(o.type) {
    case REG:
      if (o.val == ZERO) {
        puts("Error. Intentando modificar el registro zero.");
        exit(3);
      } else {
        return &(machine->reg[o.val]);
      }
    case MEM:
      if (ISSET_SEGMENTATION && o.val < count)
        return NULL;
      else
        return &(machine->memory[o.val]);
  }
}

// Ejecuta una instruccion
void runIns(struct Instruction i) {
  int* d1 = destino(i.src);
  int* d2 = destino(i.dst);
  int o1 = origen(i.src);
  int o2 = origen(i.dst);

  switch (i.op) {
    case NOP:
      break;
    case MOV:
      *d2 = o1;
      break;
    case SW:
      machine->memory[o2] = o1;
      break;
    case LW:
      *d2 = machine->memory[o1];
      break;
    case PUSH:
      machine->reg[SP]--;
      machine->memory[machine->reg[SP]] = o1;
      break;
    case POP:
      *d1 = machine->memory[machine->reg[SP]];
      machine->reg[SP]++;
      break;
    case CALL:
      machine->reg[SP]--;
      machine->memory[machine->reg[SP]] = machine->reg[PC];
      machine->reg[PC] = o1 - 1;
      break;
    case RET:
      machine->reg[PC] = machine->memory[machine->reg[SP]];
      machine->reg[SP]++;
      break;
    case PRINT:
      printf("%d\n", o1);
      break;
    case READ:
      scanf("%d", d1);
      break;
    case ADD:
      *d2 = o1 + o2;
      if (*d2 == 0) SET_ZERO;
      else UNSET_ZERO;
      break;
    case SUB:
      *d2 = o1 - o2;
      if (*d2 == 0) SET_ZERO;
      else UNSET_ZERO;
      break;
    case MUL:
      *d2 = o1 * o2;
      if (*d2 == 0) SET_ZERO;
      else UNSET_ZERO;
      break;
    case DIV:
      *d2 = o1 / o2;
      if (*d2 == 0) SET_ZERO;
      else UNSET_ZERO;
      break;
    case AND:
      *d2 = o1 & o2;
      if (*d2 == 0) SET_ZERO;
      else UNSET_ZERO;
      break;
    case CMP:
      if (o1 == o2) {
          UNSET_LOWER;
          SET_EQUAL;
      } else {
          UNSET_EQUAL;
          if (o1 < o2) SET_LOWER;
      }
      break;
    case JMP:
      machine->reg[PC] = o1 - 1;
      break;
    case JMPE:
      if (ISSET_EQUAL) machine->reg[PC] = o1 - 1;
      break;
    case JMPL:
      if (ISSET_LOWER) machine->reg[PC] = o1 - 1;
      break;
    case DMP:
      dumpMachine(machine);
      dumpMemory(machine);
      break;
    case DBG:
      if (ISSET_DEBUG) UNSET_DEBUG;
      else SET_DEBUG;
      break;
    case SEG:
      if (ISSET_SEGMENTATION) UNSET_SEGMENTATION;
      else SET_SEGMENTATION;
      break;
    case HLT:
      machine->reg[PC]--;
      break;
    default:
      printf("Instruction not implemented\n");
      exit(4);
  }
}

// Cehquea que una instruccion sea un salto
int checkJump(Opcode op) {
    return op == CALL || op == JMP || op == JMPE || op == JMPL;
}

// Chequea que todas las etiquetas existan
void checkLabels() {
  for (int j = 0; j < count; j++)
    if (checkJump(code[j].op))
      if (code[j].src.lab) {
        printf("Jump to unkown label %s\n", code[j].src.lab);
        exit(1);
      }
}

// Reemplaza las etiquetas por valores inmediatos
void processLabels() {
  for (int i = 0; i < count; i++) // Recorre el codigo...
    if (code[i].op == LABEL) { // ...en busca de una etiqueta
      for (int j = 0; j < count; j++) // Recorre el codigo...
        if (checkJump(code[j].op)) // ...buscando un salto
          if (code[j].src.lab && !strcmp(code[j].src.lab,code[i].src.lab)) {
            // Si el salto corresponde a la etiqueta
            code[j].src.type = IMM;
            code[j].src.val = i; // Agrega el numero de linea
            code[j].src.lab = NULL; // Borra la etiqueta de la instruccion
          }

      // Borra la etiqueta del codigo
      for (int j = i; j < count - 1; j++)
        code[j] = code[j + 1];

      count--;
    }

  checkLabels();
}

void run() {
  // Arquitectura de Von Neumann
  memcpy(&(machine->memory), &code, count * SIZE_INSTRUCTION);

  // Ciclo de ejecucion
  struct Instruction i;
  do {
    i = *getInstruction(machine->reg[PC], machine);

    // Unidad de depuramiento
    if (ISSET_DEBUG || (breakpoint && machine->reg[PC] == breakpoint) ) { breakpoint = 0; debug(machine); }
    if (next && i.op == RET) { SET_DEBUG; next = 0; }

    if (ISSET_SEGMENTATION && machine->reg[PC] >= count) {
      printf("Error: Intentando ejecutar codigo en el segmento de pila.\n");
      exit(2);
    } else {
      runIns(i); // Ejecuta la instruccion
    }

    machine->reg[PC]++; // If not a jump, continue with next instruction
  } while (i.op != HLT);
}

// Inicializa la maquina
void init(int argc, char** argv) {
  // Inicializa registros
  machine->reg[PC] = 0; // Start at first instruction
  machine->reg[FLAGS] = 1; // Segmentacion por defecto
  machine->reg[SP] = MEM_SIZE;
  machine->reg[ZERO] = 0;
  machine->reg[R0] = 0;
  machine->reg[R1] = 0;
  machine->reg[R2] = 0;
  machine->reg[R3] = 0;

  // Inicializa memoria
  memset(machine->memory, 0, MEM_SIZE * sizeof(int));

  // Argumentos por linea de comando
  if (argc > 2) machine->reg[R0] = atoi(argv[2]);
  if (argc > 3) machine->reg[R1] = atoi(argv[3]);
  if (argc > 4) machine->reg[R2] = atoi(argv[4]);
  if (argc > 5) machine->reg[R3] = atoi(argv[5]);
  for (int i = argc - 1; i > 5; i--) {
    machine->reg[SP]--;
    machine->memory[machine->reg[SP]] = atoi(argv[i]);
  }
}

int main(int argc, char** argv) {
  // Manejo de linea de comando
  if (argc < 2) {
    printf("Usage: %s file1.asm\n", argv[0]);
    exit(5);
  }

  // Manejo de archivo de entrada
  yyin = fopen(argv[1], "r"); // Lee el archivo
  yyparse(); // Analisis sintactico
  fclose (yyin);  // Cierra el archivo
  processLabels(); // Procesa las etiquetas

  // Bootea la maquina
  init(argc, argv); // Inicializa la maquina
  run(); // Ejecuta el codigo
  exit(0); // Finaliza correctamente
}
