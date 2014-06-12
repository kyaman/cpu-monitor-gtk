/*
 * fifo.h
 *
 *  Created on: 2014/03/03
 *      Author: kyama
 */

#ifndef FIFOEX_H_
#define FIFOEX_H_

void fifoEx_init(int count, int size);
void fifoEx_finalize();
void fifoEx_add(int fifoId, double new_data);
void fifoEx_get(int fifoId, double **fifo, int *size);
double fifoEx_getByIndex(int fifoId, int index);

#endif /* FIFOEX_H_ */
