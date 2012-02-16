#ifndef EXPERIMENTS_H
#define EXPERIMENTS_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <strstream>
#include <list>
#include <vector>
#include <algorithm>
#include <math.h>
#include "neat.h"
#include "networks.h"
#include "genetics.h"

using namespace std;

//The XOR evolution routines *****************************************
Population *xor_test(int gens);
bool xor_evaluate(Organism *org);
int xor_epoch(Population *pop,int generation,char *filename, int &winnernum, int &winnergenes,int &winnernodes);



#endif






