#include <stdio.h> 
#include <stdlib.h> 
#include <time.h> 
#include <unistd.h>
#include <string.h>  
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ctype.h> 

#define MESSAGE_COUNT 5 // olusturulacak mesaj sayisi
#define MESSAGE_SIZE 50 // olusturulacak mesaj boyu
#define PORT	 8080
#define CEASERINDEX 15

char *name ;

int sockfd;
struct sockaddr_in servaddr;

struct LocationData {
    char * name; 
    time_t currentTime; // zaman [0, +∞)
    int latitude;       // enlem  = X ekseni [-90, 90]
    int longitude;      // boylam = Y ekseni [-180, 180]
    int altitude;       // irtifa = Z ekseni [0, +∞)
} typedef LocationData;

const char *locationToMessage(const LocationData location) {
    // konum bilgileri mesaja donusturecek fonksiyon
    // Or “t180142x73y-97z179”
    const time_t data = location.currentTime;
    struct tm *timeinfo = localtime(&data);

    int hours = timeinfo ->tm_hour;
    int minutes = timeinfo ->tm_min;
    int seconds = timeinfo ->tm_sec;

    char *message = malloc(MESSAGE_SIZE); 

    sprintf(message,"%s |t:%d:%d:%d|x:%d|y:%d|z:%d",location.name,hours,minutes,seconds,location.latitude,location.longitude,location.altitude);
    //printf("%s",message);
    return message;
}

const char *encrypt(const char *message) {
    // mesaji sifreleyecek fonksiyon

    size_t length = strlen(message);
    char *encrypted = malloc(length +1);

    for(size_t i = 0; i < length; i++) {
        encrypted[i] = message[i] + CEASERINDEX;
    }
    encrypted[length] = '\0';
    return encrypted;  // encrypted 
}

LocationData getLocation() {
    time_t currentTime = time(NULL);
    int latitude = rand() % 180 - 90;
    int longitude = rand() % 360 - 180;
    int altitude = rand();
    LocationData location = {name, currentTime, latitude, longitude, altitude};
    return location;
}

void sendMessage(const char *encryptedMessage) {
    // sifrelenmis mesaji gonderecek fonksiyon
 	socklen_t len;
	len = sizeof(servaddr); //len is value/result
	sendto(sockfd, (char *)encryptedMessage, strlen(encryptedMessage),MSG_CONFIRM, (const struct sockaddr *) &servaddr,len);
	printf("Message %s sent.\n" ,encryptedMessage );
}

int main(int argc, char* argv[]) {

    name = argv[1];

	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
		
	memset(&servaddr, 0, sizeof(servaddr));
		
	// Filling server information
	servaddr.sin_family = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(PORT);
		
    for (int i = 0; i < MESSAGE_COUNT; i++) {
    // konum olusturma islemi
        srand(time(0));
        sleep(rand() % 5 + 1 ); 
        sendMessage(encrypt(locationToMessage(getLocation())));
    }
    close(sockfd);
    return 0;
}
