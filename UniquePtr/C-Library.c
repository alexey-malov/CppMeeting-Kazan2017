#include "C-Library.h"
#include <stdlib.h>
#include <stdio.h>


Data* AllocateData()
{
	Data* data = (Data*)malloc(sizeof(Data));
	data->value = 42;
	return data;
}

void DoSomethingWithData(Data *data, int x)
{
	data->value += x;
}

void DeallocateData(Data* data)
{
	printf("Deallocating data\n");
	free(data);
}
