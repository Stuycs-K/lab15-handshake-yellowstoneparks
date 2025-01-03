#include "pipe_networking.h"
//UPSTREAM = to the server / from the client
//DOWNSTREAM = to the client / from the server
/*=========================
  server_setup

  creates the WKP and opens it, waiting for a  connection.
  removes the WKP once a connection has been made

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_setup() {
	int from_client = 0;

	// read and write for everyone 
	if (mkfifo(WKP, 0666) == -1) {
		perror("mkdififo error");
		exit(1);
	}
 
	printf("(ss) WKP created, waiting waiting waiting\n");
 
	// blocking until it is opened in write only
	from_client = open(WKP, O_RDONLY);
	if (from_client == -1) {
		perror("open err");
		exit(1);
	}
 
	printf("(ss) ah a connection\n");	

  return from_client;
}

/*=========================
  server_handshake 
  args: int * to_client

  Performs the server side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe (Client's private pipe).

  returns the file descriptor for the upstream pipe (see server setup).
  =========================*/
int server_handshake(int *to_client) {
  int from_client;

	// create wkp
	from_client = server_setup();

	// private pipe name
	char pp_name[BUFFER_SIZE];
	if (read(from_client, pp_name, BUFFER_SIZE) == -1) {
 		perror("cannot read ppname");
 		exit(1);
 	}
  
 	printf("(sh) client's pp name %s\n", pp_name);	
	
	// dont want other clients connecting while connected
	unlink(WKP);
	
	// opening the privatre pipe
	*to_client = open(pp_name, O_WRONLY);

	// sending a random int (syn ack)
	srand(time(NULL));
	int rand_int = rand() % 1000;
	write(*to_client, &rand_int, sizeof(rand_int));
	printf("(sh) sent random int %d\n", rand_int);

	// get the ack
	int ack;
	read(from_client, &ack, sizeof(ack));
	printf("(sh) Value sent: %d; value recieved (should be value sent + 1): %d\n", rand_int, ack);

  return from_client;
}


/*=========================
  client_handshake
  args: int * to_server

  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.

  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
  int from_server;

	// fifo name creating it using pid
	char pp_name[BUFFER_SIZE];
	snprintf(pp_name, sizeof(pp_name), "client_%d", getpid());

	if (mkfifo(pp_name, 0666) == -1) {
		exit(1);
	}

	printf("(ch) clients pp created %s\n", pp_name);

	// set *to_server to the fd, connect to server
	*to_server = open(WKP, O_WRONLY);
	
	// send name to server
	write(*to_server, pp_name, strlen(pp_name) + 1);
	
	// get the random int from the server
	int from_server = open(pp_name, O_RDONLY);

	int rand_int;
	read(from_server, &rand_int, sizeof(rand_int));

	printf("(ch) client got %d as the random int\n", rand_int);

	// unlinking cause we are done with the pipe here
	unlink(pp_name);

	// sending the number plus one, this is the ack
	int ack = rand_int + 1;
	write(*to_server, &ack, sizeof(ack));

  return from_server;
}


/*=========================
  server_connect
  args: int from_client

  handles the subserver portion of the 3 way handshake

  returns the file descriptor for the downstream pipe.
  =========================*/
// same thing as the other function right??
int server_connect(int from_client) {
  int to_client  = 0;

	char pp_name[BUFFER_SIZE];
  
  if (read(from_client, pp_name, BUFFER_SIZE) == -1) {
    perror("cannot read ppname");
    exit(1);
  }
  
  printf("client's pp name %s\n", pp_name); 

  unlink(WKP);
  
  to_client = open(pp_name, O_WRONLY);

  // sending a random int
  srand(time(NULL));
  int rand_int = rand() % 1000;
  write(*to_client, &rand_int, sizeof(rand_int));

  int ack;
  read(from_client, &ack, sizeof(ack));

  printf("Value sent: %d; value recieved (should be value sent + 1): %d\n", rand_int, ack);

  return to_client;
}


