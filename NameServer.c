#ifdef TEST
#include <stdio.h>
#endif

#include <ts7200.h>
#include <TaskDescriptor.h>
#include <ContextSwitch.h>
#include <NameServer.h>
int volatile NAME_SERVER_TID = -1;
int lookup_table[LOOKUP_TABLE_SIZE];

int string_cmp(char *src, char *dest, int n) {
        int i = 0;
	while (src[i] == dest[i]) {
            i++;
            if (i==n||src[i]==0)return 1;
        }
	
	return 0;
}

int hash(char *token, int n) {
    int num = 0;
    int i = 0;
    int digit;
    while(i<n) {
        digit = (token[i] - '0') % 10;
        num = num * 10 + digit;
        i++;
    }
    
    return num % LOOKUP_TABLE_SIZE;
}

void name_server() {
    int i = 0;
    while (i < LOOKUP_TABLE_SIZE) {
        lookup_table[i] = -1;
        i++;
    }
    //char reply[REPLY_LENGTH];
    char msg[MSG_MAX_LENGTH+1];
    msg[MSG_MAX_LENGTH] = 0;
#ifdef TEST
        printf("Start NameServer\n");
#endif
    int tid;
    int msglen;
    while(1) {
        msglen = Receive(&tid, msg, MSG_MAX_LENGTH);
        
        if (msglen<MSG_MIN_LENGTH) {
            Reply(tid, MSG_TOO_SHORT, REPLY_LENGTH);
        } else {
            switch(*msg) {
                case 's':
                    if (string_cmp(SIGNUP, msg, MSG_MIN_LENGTH)) {
                        //Maximum 3 char
                        //Hash result at most 999
                        int hash_id = hash(msg + MSG_MIN_LENGTH, msglen - MSG_MIN_LENGTH);
                        lookup_table[hash_id] = tid;
                        Reply(tid, MSG_SUCCEED, REPLY_LENGTH);
                    }
                    break;
                
                case 'l':
                    if (string_cmp(LOOKUP, msg, MSG_MIN_LENGTH)) {
                        int hash_id = hash(msg + MSG_MIN_LENGTH, msglen - MSG_MIN_LENGTH);
                        int id = lookup_table[hash_id];
                        Reply(tid, (char *)&id, 4);
                    }
                    break;
                
                default:
                    Reply(tid, MSG_INVALID, REPLY_LENGTH);
            }
        }
        
    }
}

void initiate_name_server(int tid) {
        NAME_SERVER_TID = tid;
}
int RegisterAs(char *name) {
   if (NAME_SERVER_TID<0||NAME_SERVER_TID>TASK_POOL_SIZE) {
    return 0;
   }
#ifdef TEST
        printf("Register to NameServer\n");
#endif
   char msg[MSG_MAX_LENGTH+1];
   msg[MSG_MAX_LENGTH] = 0;
   msg[0] = 's';
   msg[1] = 'i';
   msg[2] = 'g';
   msg[3] = 'n';
   msg[4] = 'u';
   msg[5] = 'p';
   msg[6] = ':';
   
   int i = 7;
   while (name[i-MSG_MIN_LENGTH]) {
    msg[i] = name[i-MSG_MIN_LENGTH];
    i++;
    if (i==MSG_MAX_LENGTH)break;
   }

   char reply[REPLY_LENGTH+1];
   reply[REPLY_LENGTH] = 0;
   Send(NAME_SERVER_TID, msg, i, reply, REPLY_LENGTH);
   
   if (string_cmp(reply, MSG_SUCCEED, REPLY_LENGTH)) {
    //bwprintf(TERM_PORT, "Reply with success information: %s", msg);
    return 1;
   }
   
   return 0;
}

int WhoIs(char *name) {
   if (NAME_SERVER_TID==-1) {
    //bwprintf(TERM_PORT, "Missing NameServer\n");
    return -1;
   }
    
   char msg[MSG_MAX_LENGTH+1];
   msg[MSG_MAX_LENGTH] = 0;
   msg[0] = 'l';
   msg[1] = 'o';
   msg[2] = 'o';
   msg[3] = 'k';
   msg[4] = 'u';
   msg[5] = 'p';
   msg[6] = ':';
   
   int i = 7;
   while (name[i-MSG_MIN_LENGTH]) {
    msg[i] = name[i-MSG_MIN_LENGTH];
    i++;
    if (i==MSG_MAX_LENGTH)break;
   }

   char reply[REPLY_LENGTH+1];
   reply[REPLY_LENGTH] = 0;
   Send(NAME_SERVER_TID, msg, i, reply, REPLY_LENGTH);
   
   int *tid = (int *)reply;
   if (*tid == -1) {
    //bwprintf(TERM_PORT, "name not exist: %s\n", name);
   }
   return *tid;
}
