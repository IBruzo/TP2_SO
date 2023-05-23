#include "list.h"
#include <assert.h>
#include "lib.h"
#include "memoryManager.h"

// Definimos una estructura auxiliar para los nodos
typedef struct node *TList;
typedef struct node
{
    elemType elem;
    struct node *tail;
} node;

struct listCDT
{
    TList first;
    unsigned int size;
    int (*compare)(elemType e1, elemType e2);
    TList iter; // para usar iteradores
};

listADT newList(int (*compare)(elemType e1, elemType e2))
{
    // listADT aux = allocMemory(memManager, sizeof(struct listCDT));
    listADT aux = sys_allocMem(sizeof(struct listCDT));
    memset(aux, 0, sizeof(struct listCDT));
    // listADT aux = calloc(1, sizeof(struct listCDT)); // traducirrrrrrrrrrrrrrrrrrrrrrrr
    aux->compare = compare;
    return aux;
}

static void freeRec(TList l)
{
    if (l == NULL)
    {
        return;
    }
    freeRec(l->tail);
    sys_free(l, sizeof(node));
    // Recursiva de cola
    // TList aux = l->tail;
    // free(l);
    // freeRec(aux);
}

void freeList(listADT list)
{
    freeRec(list->first);
    sys_free(list, sizeof(listADT));
}

int isEmpty(const listADT list)
{
    return list->first == NULL;
}

int size(const listADT list)
{
    return list->size;
}

static TList insertRec(TList first, elemType elem, int (*compare)(elemType e1, elemType e2), int *flag)
{
    int c;
    if (first == NULL || (c = compare(first->elem, elem)) > 0)
    {
        // TList aux = allocMemory(memManager, sizeof(struct node));
        TList aux = sys_allocMem(sizeof(struct node));
        aux->elem = elem;
        aux->tail = first;
        *flag = 1;
        return aux;
    }
    if (c < 0)
    {
        first->tail = insertRec(first->tail, elem, compare, flag);
    }
    return first;
}

// 1 si la inserta, 0 sinó
int insert(listADT list, elemType elem)
{
    int flag = 0; // va a valer 1 si se agregó el nuevo elemento
    list->first = insertRec(list->first, elem, list->compare, &flag);
    list->size += flag;
    return flag;
}

static TList deleteREC(TList l, elemType elem, int *flag, int (*compare)(elemType e1, elemType e2))
{
    int c;
    if (l == NULL || (c = (compare(l->elem, elem))) > 0)
    {
        return l;
    }
    if (c == 0)
    {
        TList aux = l->tail;
        sys_free(l, sizeof(node));
        *flag = 1;
        return aux;
    }
    if (c < 0)
    {
        l->tail = deleteREC(l->tail, elem, &flag, compare);
    }
    return l;
}
// 1 si estaba, 0 sinós
int delete(listADT list, elemType elem)
{
    int flag = 0;
    list->first = deleteREC(list->first, elem, &flag, list->compare);
    list->size -= flag;
    return flag;
}

static int belongsRec(const TList first, elemType elem, int (*cmp)(elemType e1, elemType e2))
{
    int c;
    if (first == NULL || (c = cmp(first->elem, elem)) > 0)
    {
        return 0;
    }
    if (c == 0)
    {
        return 1;
    }
    return belongsRec(first->tail, elem, cmp);
}

int belongs(const listADT list, elemType elem)
{
    return belongsRec(list->first, elem, list->compare);
}

int belongsIter(const listADT list, elemType elem)
{
    TList aux = list->first;
    while (aux != NULL)
    {
        int c = list->compare(aux->elem, elem);
        if (!c)
        {
            return 1;
        }
        if (c > 0)
        {
            return 0;
        }
        aux = aux->tail;
    }
    return 0;
}

static elemType getRec(TList first, unsigned int idx)
{
    if (idx == 0)
    {
        return first->elem;
    }
    return getRec(first->tail, idx - 1);
}

elemType get(const listADT list, unsigned int idx)
{
    if (list->size <= idx)
    {
        /* PCB dummy;
        int dummyFD[] = {-1};
        buildPCB(&dummy, -1, -1, -1, 3, 1, dummyFD, 1); */
        elemType dummy;
        return dummy;
    }
    // Ya sabemos que idx es válido
    return getRec(list->first, idx);
}

void toBegin(listADT list)
{
    list->iter = list->first;
}

int hasNext(const listADT list)
{
    return list->iter != NULL;
}
elemType next(listADT list)
{
    if (!hasNext(list))
    {
        elemType dummy;
        return dummy;
    }
    // guardo en aux lo que voy a devolver
    elemType aux = list->iter->elem;
    // avanzo el iterador
    list->iter = list->iter->tail;
    return aux;
}
void map(listADT list, elemType (*funcion)(elemType))
{
    TList aux = list->first;
    while (aux != NULL)
    {
        aux->elem = funcion(aux->elem);
        aux = aux->tail;
    }
}
