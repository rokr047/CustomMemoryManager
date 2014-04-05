#include <new>

#define DEBUG 0
#define BUFFERSIZE 20480
template<typename T>
class MManager
{
private:

    char bufferStart[BUFFERSIZE];

    typedef struct tag_HeadBlock
    {
        unsigned m_nAllocated;
        unsigned m_nFreeBlocks;
        void* m_pLastBookmark;
    } Header;

    typedef struct tag_Bookmark
    {
        void* m_pDataLocation;
        bool m_bIsBlockEmpty;
    } Bookmark;

public:
	MManager();
	~MManager();

	T *	CreateObject();

	void RemoveObject(T *);

	unsigned GetCount() const;
	unsigned GetCapacity() const;

	bool IsEmpty() const;
	bool IsFull() const;

	T *	operator [] (unsigned nIndex);

    //Debug Funtions
    void Debug() const;
};

template<typename T>
MManager<T>::MManager()
{
    #if DEBUG
    printf("Saved Buffer Start...Creating the Header\n");
    #endif

    //Create and Initialize Header.
    Header* head = (Header*) bufferStart;

    head->m_nAllocated = 0;
    head->m_nFreeBlocks = 0;
    head->m_pLastBookmark = (void*)(bufferStart + sizeof(Header));

    #if DEBUG
    printf("Header has been created at %d size = %d\n",(size_t)head,sizeof(Header));
    #endif
}

template<typename T>
MManager<T>::~MManager()
{
   delete[] bufferStart;
}

template<typename T>
T* MManager<T>::CreateObject()
{
    void* result;
    #if DEBUG
    printf("-Started allocation of new block.\n");
    #endif

    Header* head = (Header*) bufferStart;

    if(head->m_nFreeBlocks <= 0)
    {
        #if DEBUG
        printf("--There are no empty blocks. So allocate from End.\n");
        #endif

        #if DEBUG
        printf("--Creating a new bookmark for the new block.\n");
        #endif
        Bookmark* f_Bookmark = (Bookmark*)head->m_pLastBookmark;
        f_Bookmark->m_bIsBlockEmpty = false;

        //The Data will be allocated from the end.
        f_Bookmark->m_pDataLocation = result = (void*)((char*)bufferStart + (BUFFERSIZE - ((head->m_nAllocated + 1) * sizeof(T))));

        //Update last bookmark pointer
        head->m_pLastBookmark = (void*)((char*)f_Bookmark + sizeof(Bookmark));

    }
    else
    {
        #if DEBUG
        printf("--free blocks available. so using one of them.\n");
        #endif
        //Search through the bookmark to find the nearest free block. (much faster & easier than searching the data.)
        Bookmark* f_Bookmark = (Bookmark*)((char*)bufferStart + sizeof(Header));
        for(; f_Bookmark < (Bookmark*)head->m_pLastBookmark; f_Bookmark++)
        {
            if(f_Bookmark->m_bIsBlockEmpty)
            {
                #if DEBUG
                printf("---found our empty block.\n");
                #endif
                break;
            }
        }

        f_Bookmark->m_bIsBlockEmpty = false;
        head->m_nFreeBlocks--;

        //now the data should be allocated where this bookmark points to.
        result = f_Bookmark->m_pDataLocation;
    }

    head->m_nAllocated++;

    #if DEBUG
    printf("-Block allocated at %d\n",(size_t)result);
    #endif

    return new(result) T();
}

template<typename T>
void MManager<T>::RemoveObject(T * elem)
{
    Header* head = (Header*) bufferStart;

    #if DEBUG
    printf("-Finding the bookmark this element belongs to...\n");
    #endif
    Bookmark* f_Bookmark = (Bookmark*)((char*)bufferStart + sizeof(Header));
    for(int l_count = 0; f_Bookmark < (Bookmark*)head->m_pLastBookmark; f_Bookmark++,l_count++)
    {
        if(f_Bookmark->m_pDataLocation == elem)
        {
            #if DEBUG
            printf("--found bookmark for the given element block at Index %d\n",l_count);
            #endif
            break;
        }
    }

    #if DEBUG
    printf("-Destructing the element.\n");
    #endif
    elem->~T();

    #if DEBUG
    printf("-Updating its bookmark.\n");
    #endif
    f_Bookmark->m_bIsBlockEmpty = true;

    head->m_nAllocated--;
    head->m_nFreeBlocks++;

    #if DEBUG
    printf("-The element has been removed.\n");
    #endif
}

template<typename T>
unsigned MManager<T>::GetCount() const
{
    Header* head = (Header*) bufferStart;

    return head->m_nAllocated;
}

template<typename T>
unsigned MManager<T>::GetCapacity() const
{
    return (BUFFERSIZE - sizeof(Header))/(sizeof(T) + sizeof(Bookmark)) ;
}

template<typename T>
bool MManager<T>::IsEmpty() const
{
    Header* head = (Header*) bufferStart;

    return !(head->m_nAllocated > 0);
}

template<typename T>
bool MManager<T>::IsFull() const
{
    Header* head = (Header*) bufferStart;

    return (MManager<T>::GetCapacity() - head->m_nAllocated <= 0);
}

/*
* Returns the (live) object that is nIndex position from BufferStart.
*/
template<typename T>
T*	 MManager<T>::operator [] (unsigned nIndex)
{
    Header* head = (Header*) bufferStart;

    #if DEBUG
    printf("--Searching for the index.\n");
    #endif

    //The search for the index should always start from first.
    //This is because there might be some blocks missing.
    Bookmark* f_Bookmark = (Bookmark*)((char*)bufferStart + sizeof(Header));
    for(int l_count = 0; f_Bookmark < (Bookmark*)head->m_pLastBookmark; f_Bookmark++)
    {
        if(!f_Bookmark->m_bIsBlockEmpty)
        {
            if(l_count == nIndex)
            {
                #if DEBUG
                printf("--found bookmark for the given element block at index %d\n", nIndex);
                #endif
                break;
            }
            l_count++;
        }
    }

    //We will have found the index by now.
    //Make sure it is not an empty block.
    assert(f_Bookmark->m_bIsBlockEmpty == false);

    return (T*)f_Bookmark->m_pDataLocation;
}

template<typename T>
void MManager<T>::Debug() const
{
    #if DEBUG
    printf("\nHEADER INFO :\n\tNumber of elements in Container = %d\n",CContainer<T>::Count());
	printf("\tManaged Container Capacity: %d, size of T : %d\n", CContainer<T>::Capacity(), sizeof(T));
	printf("\tIs Container Empty : %s\n", CContainer<T>::IsEmpty() ? "true" : "false");
	printf("\tIs Container Full : %s\n", CContainer<T>::IsFull() ? "true" : "false");
	printf("\tNumber of Free Blocks : %d\n",(size_t)((Header*) bufferStart)->m_nFreeBlocks);
	#endif // DEBUG
}
