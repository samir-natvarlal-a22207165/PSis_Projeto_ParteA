#include <zmq.h>

typedef char direction_t;

typedef struct {
    Sint32 x;
    Sint32 y;
} Position;

#define UP 'u'
#define DOWN 'd'
#define LEFT 'l'
#define RIGHT 'r'

#define FIFO_NAME "/tmp/fifo_snail"
void *create_client_channel(char *server_addr);
void read_message(void *fd, char *message_type, char *c, Position * position);
void send_response(void *fd, char *message_type, char *message);
void *create_server_channel();
void send_connection_message(void *fd, char ch);
void send_movement_message(void *fd, char ch, Position * position);
void receive_response(void *fd, char *message);
