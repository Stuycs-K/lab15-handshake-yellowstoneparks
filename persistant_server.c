#include "pipe_networking.h"
#include <signal.h>

void sigint_handler(int signum) {
	//delete the WKP befor ending the server

int main() {
  int to_client;
  int from_client;

	signal(SIGINT, sigint_handler);

	while (1) {
  	from_client = server_handshake( &to_client );

		//Close the old file descriptors before you end your loop
		if (close(from_client) == -1) {
			perror("error closing")
		}

		sleep(1);
	}
}
