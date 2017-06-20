#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagData
{
	int value;
}Data;

Data* AllocateData();
void DoSomethingWithData(Data *data, int x);
void DeallocateData(Data* data);

#ifdef __cplusplus
}
#endif

