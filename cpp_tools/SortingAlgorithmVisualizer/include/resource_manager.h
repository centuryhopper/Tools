#include "utils.h"
#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

void initState(SortType type, void **sortState);
void cleanUpState(SortType type, void **sortState);

#endif