#ifdef LOCK_SERVER_H
#else
#define LOCK_SERVER_H	
void lock_server();
int Acquire();
int Release();
#endif
