// label.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../definitions.h"

Label *labels;
size_t labels_count;
size_t labels_size;

void InitLabels()
{
  labels_size = 16;
  labels_count = 0;
  labels = calloc(labels_size, sizeof(Label));
}

void TerminateLabels()
{
  free(labels);
  labels = NULL;
}

void CreateLabel(char *name, size_t start)
{
  if (labels_count > labels_size)
  {
    size_t new_size = labels_size * 2;

    Label *tmp = realloc(labels, sizeof(Variable) * new_size);
    labels = tmp;

    labels_size *= 2;
  }

  labels[labels_count].key = name;
  labels[labels_count].start = start;
  labels_count++;
}

ssize_t GetLabel(char *name)
{
  ssize_t result;

  for (size_t i = 0; i < labels_count; i++)
  {
    if (strcmp(labels[i].key, name) == 0)
    {
      result = i;

      return result;
    }
  }

  // Label is not found
  result = -1;
  return result;
}