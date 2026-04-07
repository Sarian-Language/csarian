// label.h
#ifndef LABEL_H
#define LABEL_H

#include "stdlib.h"

#include "../definitions.h"

extern Label *labels;

void InitLabels();

void TerminateLabels();

void CreateLabel(char *name, size_t start);

ssize_t GetLabel(char *name);

#endif