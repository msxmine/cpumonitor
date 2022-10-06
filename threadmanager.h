void create_thread(void (*inner_function)(void), int loopdelay, char* name);
void watchdog();
void exitThreads();
void joinThreads();
