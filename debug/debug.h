#ifndef __DEBUG_H_
#define __DEGUB_H_

#include "../machine.h"

// Banderas del depurador
extern char next;
extern char breakpoint;

struct Instruction* getInstruction(int linea, struct Machine* machine);

void printInstr(struct Instruction i);

// Volcado de registros
void dumpMachine(struct Machine* machine);

// Volcado de memoria
void dumpMemory(struct Machine* machine);

void debug(struct Machine* machine);

#endif
