#include <stdio.h> 
#include <sys/time.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>


#include "demo_comm.h"
#include "opdevsdk_hikflow_dev_man.h"
#include "opdevsdk_mem.h"
#include "opdevsdk_hka_types.h"
#include "custom_callback.h"
#include "opdevsdk_hikflow_custom.h"

#include "plate_rec.h"

int argMax(float *input, int length)
{
    int maxIndex = 0;
    float max = input[0];
    for (int i = 1; i < length; i++)
    {
        if (input[i] > max)
        {
            max = input[i];
            maxIndex = i;
        }
    }
    return maxIndex;
}

int softMax(float *input, int length)
{
    // input取值可能是负数，所以要减去input的最大值避免上溢，加上eps避免下溢出
    
    float max = input[0];
    for (int i = 1; i < length; i++)
    {
        if (input[i] > max)
        {
            max = input[i];
        }
    }
    float eps = 0.000001;

    float sum = 0;
    for (int i = 0; i < length; i++)
    {
        sum += exp(input[i] - max);
    }
    for (int i = 0; i < length; i++)
    {
        input[i] = exp(input[i] - max) / (sum + eps);
    }
    return OPDEVSDK_S_OK;
}

char *parsePlateName(float **input, int length)
{
    int maxIndex = 0;
    int maxIndex_offset1 = 0;
    int maxIndex_offset2 = 0;
    int offset = 0;
    int flag = 1;
    char *pName = (char *)malloc(length * sizeof(char) * 3);
    // 全部初始化为0
    memset(pName, 0, length * sizeof(char) * 3);

    for (int i = 0; i < length; i++)
    {
        maxIndex = argMax((float*)input[i], 78) * 3;

        if (plateName[maxIndex] == '#') 
        {
            flag = 1;
            continue;
        }
        else if (flag == 0 && ((pName[offset - 1] == plateName[maxIndex]) || (pName[offset - 1] == plateName[maxIndex + 2] && pName[offset - 2] == plateName[maxIndex + 1] && pName[offset - 3] == plateName[maxIndex])))
            continue;

        flag = 0;

        pName[offset++] = plateName[maxIndex];

        maxIndex_offset1 = maxIndex + 1;
        if (plateName[maxIndex_offset1] == ' ')
            continue;
        pName[offset++] = plateName[maxIndex + 1];

        maxIndex_offset2 = maxIndex + 2;
        if (plateName[maxIndex_offset2] == ' ')
            continue;
        pName[offset++] = plateName[maxIndex + 2];
    }
    DEMOPRT((char*)"\n");
    return pName;
}

int parseRawData(float *input, int input_len, float ***output)
{
    for (int i=0; i<21; i++)
    {
        for (int j=0, offset=i; j<78; j++, offset+=21)
        {
            (*output)[i][j] = input[offset];
        }
    }
    return OPDEVSDK_S_OK;
}
