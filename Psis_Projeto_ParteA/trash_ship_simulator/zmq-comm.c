#include "zmq-comm.h"
#include "letter-movements.pb-c.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <SDL2/SDL.h>
#include <unistd.h>

#include <zmq.h>



void *create_server_channel() {
  void *context = zmq_ctx_new();
  void *responder = zmq_socket(context, ZMQ_REP);
  int response = zmq_bind(responder, "tcp://*:5555");
  (void)response;
  return responder;
}

void read_message(void *fd, char *message_type, char *c, direction_t *direction) {
  uint8_t buffer[1024];
  int size = zmq_recv(fd, buffer, sizeof(buffer), 0);

  if (size < 0) {
    strcpy(message_type, "ERROR");
    return;
  }

  MovementRequest *move_req = movement_request__unpack(NULL, size, buffer);
  if (move_req != NULL && move_req->direction.len > 0) { 
    strcpy(message_type, "MOVE");
    *c = (move_req->letter.len > 0) ? move_req->letter.data[0] : '\0';
    *direction = (direction_t)move_req->direction.data[0];
    movement_request__free_unpacked(move_req, NULL);
    return;
  }
  if (move_req != NULL) {
    movement_request__free_unpacked(move_req, NULL);
  }

  ConnectRequest *conn_req = connect_request__unpack(NULL, size, buffer);
  if (conn_req != NULL) {
    strcpy(message_type, "CONNECT");
    *c = (conn_req->letter.len > 0) ? conn_req->letter.data[0] : '\0'; 
    connect_request__free_unpacked(conn_req, NULL);
    return;
  }

  strcpy(message_type, "UNKNOWN");
}

void send_response(void *fd, char *message_type, char *message) {
  uint8_t buffer[1024];
  size_t packed_size;

  ServerResponse resp = SERVER_RESPONSE__INIT;

  if (strcmp(message_type, "CONNECT") == 0) {
    resp.type = SERVER_RESPONSE__RESPONSE_TYPE__CONNECT;
    resp.success = (strcmp(message, "OK") == 0) ? 1 : 0;
  } else if (strcmp(message_type, "MOVE") == 0) {
    resp.type = SERVER_RESPONSE__RESPONSE_TYPE__MOVEMENT;
    resp.success = (strcmp(message, "WALL") == 0) ? 0 : 1;
  } else {
    resp.type = SERVER_RESPONSE__RESPONSE_TYPE__CONNECT;
    resp.success = 1;
  }

  packed_size = server_response__pack(&resp, buffer);
  zmq_send(fd, buffer, packed_size, 0);
}

void *create_client_channel(char *server_ip_addr) {
  void *context = zmq_ctx_new();
  void *requester = zmq_socket(context, ZMQ_REQ);

  char server_zmq_addr[100];
  sprintf(server_zmq_addr, "tcp://%s:5555", server_ip_addr);

  zmq_connect(requester, "tcp://localhost:5555");

  return requester;
}

void send_connection_message(void *fd, char ch) {
  ConnectRequest req = CONNECT_REQUEST__INIT;
  req.letter.data = (uint8_t *)&ch;
  req.letter.len = 1;
  uint8_t buffer[1024];
  size_t packed_size = connect_request__pack(&req, buffer);
  zmq_send(fd, buffer, packed_size, 0);
}

void send_movement_message(void *fd, char ch, direction_t direction) {
  MovementRequest req = MOVEMENT_REQUEST__INIT;
  req.letter.data = (uint8_t *)&ch;
  req.letter.len = 1;
  req.direction.data = (uint8_t *)&direction;
  req.direction.len = 1;

  uint8_t buffer[1024];
  size_t packed_size = movement_request__pack(&req, buffer);
  zmq_send(fd, buffer, packed_size, 0);
}

void receive_response(void *fd, char *message) {
  uint8_t buffer[1024];
  int size = zmq_recv(fd, buffer, sizeof(buffer), 0);

  if (size < 0) {
    strcpy(message, "ERROR");
    return;
  }

  ServerResponse *resp = server_response__unpack(NULL, size, buffer);
  if (resp != NULL) {
    if (resp->type == SERVER_RESPONSE__RESPONSE_TYPE__CONNECT) {
      strcpy(message, resp->success ? "OK" : "NOT OK");
    }else {
      strcpy(message, "UNKNOWN");
    }
    server_response__free_unpacked(resp, NULL);
    return;
  }

  strcpy(message, "UNKNOWN");
}