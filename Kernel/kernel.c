#include <stdint.h>
#include <lib.h>
#include <moduleLoader.h>

#include <interrupts.h>
#include <sound_driver.h>
#include <video_driver.h>
#include <exceptions.h>
#include "dlc_list.h"
#include "list.h"
#include "memoryManager.h"
void *const heapAddress = (void *)0x600000;
#define MAX_HEAP_SIZE (1024 * 1024 * 128) // 128Mb

void *initializeKernelBinary()
{

	void *moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress};

	loadModules(&endOfKernelBinary, moduleAddresses);

	clearBSS(&bss, &endOfKernel - &bss);

	return getStackBase();
}

/* Primer hilo del Kernel */
int main()
{
	/* La mayoria de lo que esta aca se deberia reemplazar por initPCBTable() y similares */
	//-----------------------------------------Table Loading---------------------------------------------------

	load_idt();
	exceptionsBackupValues((uint64_t)sampleCodeModuleAddress, getSP());
	/* inicializo el MemManager, lo almaceno en una constante global  */
	/* puse 256Mb de memoria a mapear, en teoria se le da 1Gb pero ni idea */
	/* como direccion inicial puse el comienzo de la Userland segun la tabla del Pure64 */
	/* arrancar otro memory manager como el buffy o el bitmap */
	initMemoryManager(heapAddress, MAX_HEAP_SIZE);

	// memManager = createMemoryManager(/* cantidad de memoria */ (void *)268435456, /* comienzo de dicha memoria */ (void *)0x100000);

	//-----------------------------------------Process Management----------------------------------------------

	// Creo PCB Table
	PCBTable = newList(cmpInt);
	// Creo la Schedule List
	list_init(&route);
	// Inserto PCB del Kernel como Nodo Centinela
	PCB kernelPCB;
	int kernelFD[] = {0};
	buildPCB(&kernelPCB, 0, 0, 0, BLOCKED, 2, kernelFD, 1);
	insert(PCBTable, kernelPCB);
	// El output deberia ser la informacion del proceso Centinela del Kernel
	// printPCBTable(PCBTable);

	//-----------------------------------------Schedule Management----------------------------------------------

	/*
	point *pr1 = (point *)sys_allocMem(sizeof(point));
	pr1->PID = 7;
	point *pr2 = (point *)sys_allocMem(sizeof(point));
	pr2->PID = 15;
	point *pr3 = (point *)sys_allocMem(sizeof(point));
	pr3->PID = 51;

	list_push(&route, &pr1->link);
	list_push(&route, &pr2->link);
	list_push(&route, &pr3->link);
	*/

	//-----------------------------------------Process Creation-------------------------------------------------
	char *args[2] = {"1", "2"};
	// createProcess esta descactivada, una vez activada realmente no retorna, el kernel no deberia crear procesos
	// 0xDEADC0DE ->
	/* sys_createProcess(testProcess1, args, 2); */
	/* sys_createProcess(testProcess2, args, 2);
	sys_createProcess(testProcess3, args, 2);
	sys_createProcess(testProcess4, args, 2); */
	// El output deberia ser el Nodo Centinela mas el nodo del proceso creado
	printPCBTable(PCBTable);
	list_print(&route);
	/* creo que no es necesario un proceso idle, podria directamente siempre correr la shell */

	return ((EntryPoint)sampleCodeModuleAddress)(); // dirreccion del _start del userland
}

/*  Process Control Block Table */
/*
	PID : numero natural
	PPID : numero natural
	RSB : puntero de 64 bits
	state : READY, RUNNING o BLOCKED
	priority : 0, 1 o 2
	FD : numero natural
	FDSize : numero natural, mantener viva esta variable!
	memInfo.qPages : numero natural
	memInfo.baseAddress : puntero
	memInfo.limit : puntero (?)
*/
