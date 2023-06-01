#include <irqDispatcher.h>

static void int_20();
static void int_21();
static uint64_t int_80(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);

typedef uint64_t (*syscall)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);

// static void int_22(uint64_t rdi, uint64_t rsi, uint64_t rdx ,uint64_t rcx, uint64_t r8, uint64_t r9);
void (*fun_inter[256])(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);
uint64_t (*fun_sys[256])(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);

/**
 * @brief inicializa los vectores de funciones q se usaran para las interrupciones
 *
 */
void initialize()
{
    (fun_inter[0]) = int_20;
    (fun_inter[1]) = int_21;
    (fun_inter[0x60]) = (void (*)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t))int_80;

    // agrego syscalls
    (fun_sys[SYS_GETCHAR_ID]) = (syscall)sys_getchar;
    (fun_sys[SYS_WRITE_ID]) = (syscall)sys_write;
    (fun_sys[SYS_TICK_ID]) = (syscall)sys_tick;
    (fun_sys[SYS_HALT_ID]) = (syscall)sys_halt;
    (fun_sys[SYS_PUTSQUARE_ID]) = (syscall)sys_putSquare;
    (fun_sys[SYS_GETTIME_ID]) = (syscall)sys_getTime;
    (fun_sys[SYS_GETLASTKEY_ID]) = (syscall)sys_getLastKey;
    (fun_sys[SYS_GETREGISTERS_ID]) = (syscall)sys_getRegisters;
    (fun_sys[SYS_BEEP_ID]) = (syscall)sys_beep;
    (fun_sys[SYS_SLEEP_ID]) = (syscall)sys_sleep;
    (fun_sys[SYS_CLEARKEYBUFFER_ID]) = (syscall)sys_clearkeybuffer;
    (fun_sys[SYS_CHANGELANGUAGE_ID]) = (syscall)sys_changeLanguage;
    (fun_sys[SYS_STOREREGISTERS_ID]) = (syscall)sys_storeRegisters;
    (fun_sys[SYS_MEMACCESS_ID]) = (syscall)sys_memAccess;
    (fun_sys[SYS_ALLOCMEM_ID]) = (syscall)sys_allocMem;
    (fun_sys[SYS_FREE_ID]) = (syscall)sys_free;
    (fun_sys[SYS_CREATEPROCESS_ID]) = (syscall)sys_createProcess;
    (fun_sys[SYS_SCROLL_UP_ONCE_ID]) = (syscall)scroll_up_once;
    (fun_sys[SYS_GETPID_ID]) = (syscall)sys_getPid;
    (fun_sys[SYS_INCREASEPRIORITY_ID]) = (syscall)sys_increasePriority;
    (fun_sys[SYS_DECREASEPRIORITY_ID]) = (syscall)sys_decreasePriority;
    (fun_sys[SYS_YIELD_ID]) = (syscall)sys_yield;
    (fun_sys[SYS_KILL_ID]) = (syscall)sys_kill;
    (fun_sys[SYS_EXIT_ID]) = (syscall)sys_exit;
    (fun_sys[SYS_SEMCREATE_ID]) = (syscall)sys_semCreate;
    (fun_sys[SYS_SEMOPEN_ID]) = (syscall)sys_semOpen;
    (fun_sys[SYS_SEMCLOSE_ID]) = (syscall)sys_semClose;
    (fun_sys[SYS_SEMWAIT_ID]) = (syscall)sys_semWait;
    (fun_sys[SYS_SEMPOST_ID]) = (syscall)sys_semPost;
    (fun_sys[SYS_MEM_ID]) = (syscall)sys_mem;
    (fun_sys[SYS_WAITPID_ID]) = (syscall)sys_waitPid;
}

void irqDispatcher(uint64_t irq, uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9)
{
    (*fun_inter[irq])(rdi, rsi, rdx, rcx, r8, r9);
    return;
}

void int_20()
{
    timer_handler();
}

void int_21()
{
    storeKey();
    // unblock();
}
/**
 * @brief selecciona la syscall q se va a usar
 *
 */
uint64_t int_80(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9)
{
    return (*fun_sys[rdi])(rsi, rdx, rcx, r8, r9);
}
