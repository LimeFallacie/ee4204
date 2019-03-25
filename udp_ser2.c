//
// Created by Emil on 20/10/2018.
//

#include "headsock.h"

void str_ser1(int sockfd);                                                           // transmitting and receiving function

int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in my_addr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {			//create socket
        printf("error in socket");
        exit(1);
    }

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(MYUDP_PORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(my_addr.sin_zero), 8);
    if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr)) == -1) {           //bind socket
        printf("error in binding");
        exit(1);
    }
    printf("start receiving\n");
    while(1) {
        str_ser1(sockfd);                        // send and receive
    }
    close(sockfd);
    exit(0);
}

void str_ser1(int sockfd)
{
    char buffer[BUFSIZE];
    char recvs[2*DATALEN];
    struct ack_so ack;
    long filesize = 0;
    int n = 0;
    int end = 0;
    struct sockaddr_in addr;
    int len = sizeof(struct sockaddr_in);

    FILE *fp;

    printf("Ready to receive data\n");
    int acked = 1;
    ack.num = 1;
    ack.len = 0;

    while(!end){
        if(acked){
            if((n = recvfrom(sockfd, &recvs, 2*DATALEN, 0, (struct sockaddr *)&addr, &len)) == -1){
                printf("file receive error\n");
                exit(1);
            }

            //printf ("%s \n", recvs);
            if(recvs[n-1] == '\0'){
                end = 1; //end flag, break out of while loop here
                n--; //last char to be ignored \0
            }

            memcpy((buffer + filesize), recvs, n); // copies memory area, copies n byte from recvs and place it into area dest as buffer + filesize
            filesize += n; //next memcopy area
            acked = 0; //wait ack
        }

        if(!acked){
            if((n = sendto(sockfd, &ack, 2, 0,(struct sockaddr *)&addr, len)) == -1){
                printf("error sending file\n");
            }
            else {
                if(ack.num == 1 && ack.len == 0){
                    //printf("sent ack\n");
                    acked = 1; //succesful acknowledge
                }
            }

        }
    }

    if ((fp = fopen("myUDPreceive.txt", "wt")) == NULL){ //open file
        printf("file write error\n");
        exit(0);
    }

    fwrite(buffer, 1, filesize, fp); //write data to file
    fclose(fp); //close file

    printf("A file has been received\n total data received is %d bytes\n", (int)filesize);
}