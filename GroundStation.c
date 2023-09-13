#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MESSAGE_COUNT 5 // olusturulacak mesaj sayisi
#define PORT	 8080
#define MAXLINE 1024
#define CEASERINDEX 15

    int sockfd;
	char buffer[MAXLINE];
	struct sockaddr_in	 servaddr, cliaddr;
    int highestAltitude = 0;

const char* receiveMessage() {
	socklen_t len;
	int n = recvfrom(sockfd, (char *)buffer, MAXLINE,MSG_WAITALL, (struct sockaddr *) &cliaddr,&len);
	buffer[n] = '\0';
    return buffer;
}

int getAltitude(const char *decryptedMessage){
    int altitude ;
    sscanf(decryptedMessage,"%*s |t:%*d:%*d:%*d|x:%*d|y:%*d|z:%d",&altitude);
    return altitude;
}

const char* decrypt(const char* encryptedMessage) {
    // mesajin sifresini cozecek fonksiyon
    size_t length = strlen(encryptedMessage);
    char *decrypted = malloc(length +1);

    for(size_t i = 0; i < length; i++) {
        decrypted[i] = encryptedMessage[i] - CEASERINDEX;
   }
    decrypted[length] = '\0';
    int altitude = getAltitude(decrypted);
    if (altitude > highestAltitude){
        highestAltitude = altitude;
    }    
    return decrypted;  // decrypted 
}

void displayMessage(const char* message) {
    // mesaji yazdiracak fonksiyon
	printf("%s\n", message);
}

int main(int argc, char* argv[]) {
	
	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
	
	memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
		
	// Filling server information
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(PORT);

    // Bind the socket with the server address
	if ( bind(sockfd, (const struct sockaddr *)&servaddr,sizeof(servaddr)) < 0 )
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

    for (int i = 0; i < atoi(argv[1]); i++) {
        //system("/bin/bash -c /home/han/IHA IHA1");
        //system("./IHA IHA"+i);
        //system("/bin/sh ls");
        //execl("ls","-al",NULL);
        //execl("./IHA","IHA");
    }

    for (int i = 0; i < MESSAGE_COUNT * atoi(argv[1]); i++) {
    // konum alma islemi
        displayMessage(decrypt(receiveMessage()));
    }		
	close(sockfd);
    printf("Highest Altitude : %d\n",highestAltitude);
    return 0; 
} 
