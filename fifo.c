/*
 * fifo.c
 *
 *  Created on: 2014/03/03
 *      Author: kyama
 */

#include <stdlib.h>

static double **_fifo = NULL;
static int _fifo_count = -1;
static int _fifo_size = -1;

void fifo_init(int count, int size) {
  int i, j;

  _fifo = (double **)malloc(sizeof(double) * count);
  _fifo_count = count;

  for (i = 0; i < count; i++)
    _fifo[i] = (double *)malloc(sizeof(double) * size);
  _fifo_size = size;

  for (i = 0; i < count; i++) {
    for (j = 0; j < size; j++)
      _fifo[i][j] = -1;
  }
}

void fifo_finalize() {
  int i;

  if (_fifo == NULL)
    return;

  for (i = 0; i < _fifo_count; i++) {
    if (_fifo[i] != NULL)
      free(_fifo[i]);
  }

  free(_fifo);
  _fifo = NULL;
  _fifo_size = -1;
  _fifo_count = -1;
}

void fifo_add(int fifoId, double new_data) {
  int i;

  if (_fifo_size < 0)
    return;

  for (i = 0; i < _fifo_size - 1; i++)
    _fifo[fifoId][i] = _fifo[fifoId][i + 1];
  _fifo[fifoId][_fifo_size - 1] = new_data;

#if 0
	for(i=0; i<_fifo_size; i++)
		printf("%-3.1lf ", _fifo[fifoId][i]);
	printf("\n");
#endif
}

void fifo_get(int fifoId, double **fifo, int *size) {
  if (_fifo_size < 0)
    return;

  *fifo = _fifo[fifoId];
  *size = _fifo_size;
}

double fifo_getByIndex(int fifoId, int index) { return _fifo[fifoId][index]; }
