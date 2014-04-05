#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

#include "MManager.h"

using namespace std;

class myTestClass
{
public:
    int m_id;
    int m_random;
    int m_random2;
};

int main()
{
    int numAlloc = 2000000;
    int currentAllocCount = 0;

    printf("-Started the Memory Manager.\n");

    MManager<myTestClass> * pManager = new MManager<myTestClass>();

    printf("--Max. number of Objects that we can save : %d \n",pManager->GetCapacity());

    clock_t startTime = clock();

    printf("Started PreFill\n");
    for (unsigned i = 0; i < 0.5 * pManager->GetCapacity(); ++i)
        {
            myTestClass * pObj = pManager->CreateObject();
            pObj->m_id = i;
            pObj->m_random = (pManager->GetCapacity() - i * 20 ) << 4;
            pObj->m_random2 = pObj->m_random * 0.333;
            currentAllocCount++;
        }

    printf("Started random alloc dealloc\n");
    int i = 0;
    while(currentAllocCount < numAlloc)
    {
        ++i;
        bool alloc = (rand() % 2 == 0)  ? true : false;

        if(alloc && pManager->IsFull())
            alloc = false;

        if(!alloc && pManager->IsEmpty())
            alloc = true;

        if(alloc)
        {
            myTestClass * pObj = pManager->CreateObject();
            pObj->m_id = i;
            pObj->m_random = (pManager->GetCapacity() - i * 20 ) << 4;
            pObj->m_random2 = pObj->m_random * 0.333;
            currentAllocCount++;
        }
        else
        {
            int nIndex = (pManager->GetCount() > 1) ? rand() % (pManager->GetCount() - 1) : 0;

            myTestClass * pObj = (*pManager)[nIndex];
            pManager->RemoveObject(pObj);
        }
    }


    printf("Started RemoveAll\n");
    while(!pManager->IsEmpty())
    {
        myTestClass * pObj = (*pManager)[0];
        pManager->RemoveObject(pObj);
    }
    assert(pManager->GetCount() == 0);

	clock_t endTime = clock();

	double elapsedTime = (double)(endTime - startTime) / CLOCKS_PER_SEC;

	printf("\nTime taken for test to complete: %f seconds\n", elapsedTime);

    return 0;
}
