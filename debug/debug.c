#include <stdio.h>
#include <string.h>

#include "debug.h"

char next = 0, breakpoint = 0;

const char* regname[REGS] = { "\%zero", "\%pc", "\%sp", "\%r0",
                              "\%r1","\%r2","\%r3", "\%flags" };

struct Instruction* getInstruction(int linea, struct Machine* machine) {
  int* i = &(machine->memory[linea * SIZE_INSTRUCTION / sizeof(int)]);
  return (struct Instruction*) i;
}

void printOperand(struct Operand s) {
  switch (s.type) {
    case IMM:
      printf("$%d", s.val);
      break;
    case REG:
      printf("%s", regname[s.val]);
      break;
    case MEM:
      if (s.lab) printf("(%s)", regname[s.val]);
      else printf("%d", s.val);
      break;
  }
}

void printInstr(struct Instruction i) {
  switch (i.op) {
    case NOP:
      printf("NOP");
      break;
    case HLT:
      printf("HLT");
      break;
    case MOV:
      printf("MOV ");
      printOperand(i.src);
      printf(",");
      printOperand(i.dst);
      break;
    case LW:
      printf("LW ");
      printOperand(i.src);
      printf(",");
      printOperand(i.dst);
      break;
    case ADD:
      printf("ADD ");
      printOperand(i.src);
      printf(",");
      printOperand(i.dst);
      break;
    case MUL:
      printf("MUL ");
      printOperand(i.src);
      printf(",");
      printOperand(i.dst);
      break;
    case SUB:
      printf("SUB ");
      printOperand(i.src);
      printf(",");
      printOperand(i.dst);
      break;
    case DIV:
      printf("DIV ");
      printOperand(i.src);
      printf(",");
      printOperand(i.dst);
      break;
    case AND:
      printf("AND ");
      printOperand(i.src);
      printf(",");
      printOperand(i.dst);
      break;
    case JMPL:
      printf("JMPL ");
      printOperand(i.src);
      if (i.src.lab) printf("%s",i.src.lab);
      break;
    case JMPE:
      printf("JMPE ");
      printOperand(i.src);
      if (i.src.lab) printf("%s",i.src.lab);
      break;
    case JMP:
      printf("JMP ");
      printOperand(i.src);
      if (i.src.lab) printf("%s",i.src.lab);
      break;
    case LABEL:
      printf("LABEL %s",i.src.lab);
      break;
    case CMP:
      printf("CMP ");
      printOperand(i.src);
      printf(",");
      printOperand(i.dst);
      break;
    case PRINT:
      printf("PRINT ");
      printOperand(i.src);
      break;
    case READ:
      printf("READ ");
      printOperand(i.src);
      break;
    case PUSH:
      printf("PUSH ");
      printOperand(i.src);
      break;
    case POP:
      printf("POP ");
      printOperand(i.src);
      break;
    case CALL:
      printf("CALL ");
      printOperand(i.src);
      if (i.src.lab) printf("%s",i.src.lab);
      break;
    case RET:
      printf("RET ");
      break;
    case DMP:
      printf("DMP ");
    case DBG:
      printf("DBG ");
      break;
    default:
      printf("Instrucction not printed.\n");
  }
}

// Volcado de registros
void dumpMachine(struct Machine* machine) {
  printf("\n******* ");
  printf("Machine state at PC = %d", machine->reg[PC]);
  printf(" ********\n");

  for (int i = 0; i < REGS; i++)
    if (strlen(regname[i])) {
      printf("%d\t%s\t\t= ", i, regname[i]);
      printf("%d\t(0x%x)\n", machine->reg[i], machine->reg[i]);
    }

  puts("***************************************\n");
}

// Volcado de memoria
void dumpMemory(struct Machine* machine) {
  printf("*************** ");
  printf("Memory state at PC = %d", machine->reg[PC]);
  printf(" ****************\n");

  for (int i = 0; i < MEM_SIZE; i++) {
    printf("%d=%d\t", i, machine->memory[i]);
    if (i && (i + 1) % 7 == 0) puts("");
  }

  puts("\n*******************************************************\n");
}

void printCode(struct Machine* machine) {
  printf("\n*****Codigo******\n");

  for (int j = 0; j < count; j++)  {
    if (j == machine->reg[PC]) printf(">> ");
    printf("%d: ",j);
    printInstr(*getInstruction(j, machine));
    if (breakpoint && breakpoint == j) printf(" (*)");
    puts("");
  }

  printf("*****************\n");
}

void debug(struct Machine* machine) {
  char comando[100];
  int direccion, valor;

  while (1) {
    printf("Ingrese comando (h para ayuda): ");
    scanf("%s", comando);

    if (*comando == 'r') dumpMachine(machine);
    if (*comando == 'm') dumpMemory(machine);
    if (*comando == 'l') printCode(machine);
    if (*comando == 'w') {
      printf("%d: ", machine->reg[PC]);
      printInstr(*getInstruction(machine->reg[PC], machine));
      puts("");
    }
    if (*comando == 'j') {
      printf("Ingrese una instruccion: ");
      scanf("%d", &valor);
      machine->reg[PC] = valor;
    }
    if (*comando == 'x') {
      printf("Ingrese una direccion de memoria: ");
      scanf("%d", &direccion);
      printf("Valor: %d\n", machine->memory[direccion]);
    }
    if (*comando == 'c') {
      printf("Indique (m)emoria o (r)egistro: ");
      scanf("%s", comando);
      if (*comando == 'm') {
        printf("Ingrese una direccion de memoria: ");
        scanf("%d", &direccion);
        printf("Ingrese el nuevo valor: ");
        scanf("%d", &valor);
        machine->memory[direccion] = valor;
      } else {
        printf("Ingrese un numero de registro: ");
        scanf("%d", &direccion);
        printf("Ingrese el nuevo valor: ");
        scanf("%d", &valor);
        machine->reg[direccion] = valor;
      }
    }
    if (*comando == 'h') {
      printf("******Help******\n");
      printf("(s)tep\n");
      printf("(n)ext\n");
      printf("(j)ump\n");
      printf("(b)reakpoint\n");
      printf("(c)hange\n");
      printf("(r)egisters\n");
      printf("(m)emory\n");
      printf("(l)ist\n");
      printf("(w)here\n");
      printf("e(x)amine\n");
      printf("(q)uit\n");
      printf("****************\n");
    }
    if (*comando == 'b') {
      printf("Ingrese una instruccion: ");
      scanf("%d", &valor);
      breakpoint = valor;
    }
    if (*comando == 's') return;
    if (*comando == 'n') { UNSET_DEBUG; next = 1; return; }
    if (*comando == 'q') { UNSET_DEBUG; return; }
    puts("");
  }
}
