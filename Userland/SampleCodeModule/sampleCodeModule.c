/* sampleCodeModule.c */

#include <shell.h>

void testProcess1()
{
    while (1)
    {
        print("Chris Arrived");
    }
    return;
}
void testProcess2()
{
    while (1)
    {
        print("Inaki Arrived");
    }
    return;
}

int main()
{
    /*
    Estado actual: se va a la syscall create Process ( se puede verificar con gdb )
    arma el dummy stack pero parece que hay un error en el armado que hace que a la vuelta te mande a trololandia en vez de a la shell, porque el scheduling en si esta funcionando, elige correctamente a la shell como proximo proceso pero el RSP simplemente esta desalineado
    Nota para joaco: probar mover el 20 a 21 y cercanos
    */
    createProcess(initialize_shell);
    /* char *args[2] = {"1", "2"}; */
    /* createProcess(testProcess1);
    createProcess(testProcess2); */
    /* sys_createProcess(initializeShell) */
    /* initialize_shell(); */
    return 0;
}
