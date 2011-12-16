#ifdef NAME_SERVER_H
#else
#define NAME_SERVER_H

#define     LOOKUP_TABLE_SIZE   1024
#define     SIGNUP              "signup:"
#define     LOOKUP              "lookup:"

#define     MSG_TOO_SHORT       "too short"
#define     MSG_INVALID         "bad msg  "
#define     MSG_SUCCEED         "succeeded"
#define     REPLY_LENGTH        9

#define     MSG_MIN_LENGTH      7
#define     MSG_MAX_LENGTH      13

int string_cmp(char *src, char *dest, int n);
void name_server();

void initiate_name_server(int tid);
int RegisterAs(char *name);
int WhoIs(char *name);
#endif
