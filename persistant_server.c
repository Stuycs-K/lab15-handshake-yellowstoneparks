#include "pipe_networking.h"
#include <signal.h>

void sigint_handler(int signum) {
	//delete the WKP befor ending the server

int main() {
  int to_client;
  int from_client;

  from_client = server_handshake( &to_client );

	signal(SIGINT, sigint_handler);
}
