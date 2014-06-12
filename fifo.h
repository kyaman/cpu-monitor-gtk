/*
 * fifo.h
 *
 *  Created on: 2014/03/03
 *      Author: kyama
 */

#ifndef FIFO_H_
#define FIFO_H_

void fifo_init(int count, int size);
void fifo_finalize();
void fifo_add(int fifoId, double new_data);
void fifo_get(int fifoId, double **fifo, int *size);
double fifo_getByIndex(int fifoId, int index);

#endif /* FIFO_H_ */
