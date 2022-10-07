#pragma once

void initLogger(char path[]);
void closeLogger(void);
void destroyLogger(void);
void processLogger(void);
void dlog(const char* format, ...);
