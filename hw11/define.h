typedef struct message_{
    char opcode;
    short len;
    char payload[1024]; 
}message;

