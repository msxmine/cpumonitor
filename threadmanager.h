void create_thread(void (*inner_function)(void), int loopdelay, char* name);
void watchdog(void);
void exitThreads(void);
void joinThreads(void);
void destroyThreadManager(void);
