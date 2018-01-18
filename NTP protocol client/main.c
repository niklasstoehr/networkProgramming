#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <sys/time.h>
//https://www.meinbergglobal.com/english/info/ntp-packet.htm

//###################################################
//Evaluation

void evaluate (int servers, int *stratumValues, double *t1Array, double *t2Array, double *t3Array, double *t4Array){
    
    // counter
    int count;
    
    //Chose server with most accurate time
    //
    // 1.) Consider Stratum
    
    // Search for lowest Stratum
    int bestStratumServer = 0;
    
    for (count = 0; count < servers; count++) {
        if (stratumValues[count] < stratumValues[bestStratumServer]){
            bestStratumServer = count;
        }
    }
    
    // Which servers have lowes stratum
    int goodServers [servers];
    
    // See if lowest more than one lowest stratum exist
    for (count = 0; count < servers; count++) {
        if (stratumValues[count] == stratumValues[bestStratumServer]){
            goodServers[count] = 1; // 1 for best servers
            //printf("server number %i has lowest stratum of %i\n", count+1, stratumValues[count]);
        } else {
            goodServers[count] = -1; // -1 for bad servers
        }
    }
    
    // 2.) Consider Delay
    float delay [servers];
    int bestServer = 0;
    
    for (count = 0; count < servers; count++) {
        if (goodServers[count] == 1){
            // delay = (t4-t1)-(t3-t2)
            delay[count] = ((t4Array[count] - t1Array[count]) - (t3Array[count] - t2Array[count]));
            bestServer = count;
            printf("server number %i has lowest stratum of %i and delay of %f seconds\n", count+1, stratumValues[count] , delay[count]);
        }
    }
    
    // Find best Server
    for (count = 0; count < servers; count++) {
        if (goodServers[count] == 1){
            if (delay[count] < delay[bestServer]) {
                bestServer = count;
            }
        }
    }
    
    printf("-> server number %i is best server with stratum of %i and delay of %f seconds\n\n", bestServer+1, stratumValues[bestServer], delay[bestServer]);
    
    
    //Calculate Time
    //
    // Current local time
    printf("current local clock time: %f\n", t4Array[bestServer]);
    
    //Offset = 0.5 * ((t2-t1)+(t3-t4))
    double offset = (double) (0.5 * ((t2Array[bestServer]-t1Array[bestServer])+(t3Array[bestServer]-t4Array[bestServer])));
    printf("The offset is %f\n", offset);
    
    //Adjust time
    double newTime;
    // new time = t4 + offset
    newTime = t4Array[bestServer] + offset;
    printf("adjust local clock time to: %f\n\n", newTime);
    
}

double calculateFraction(double time, unsigned fraction){
    
    long double j = 4294967296;
    
    for( int i = 32; i > 0; i-- ){
        
        if (fraction&1){
            time = time + (1/j);
        }
        
        j = j/2;
        
        fraction = fraction >> 1;
        
    }
    
    return time;
    
}


//###################################################
//
// Main
int main(int argc, char *argv[])
{
    
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    struct sockaddr_storage their_addr;
    socklen_t addr_len = sizeof(their_addr);
    bzero(&their_addr, sizeof(their_addr));
    
    //###################################################
    // loop servers and fill arrays
    //
    //Number of Servers contacted
    int servers = 3;
    // Servers which will be contacted
    char** servernames = malloc(100);
    servernames[0] = malloc(10000);
    servernames[1] = malloc(10000);
    servernames[2] = malloc(10000);
    
    strcpy(servernames[0], "stratum2-4.NTP.TechFak.Uni-Bielefeld.DE");
    strcpy(servernames[1], "time1.uni-paderborn.de");
    strcpy(servernames[2], "ntp0.rrze.uni-erlangen.de");
    
    //Client Timestamp Examples
    double t1Array [3];
    double t4Array [3];
    //Server Timestamp Examples
    double t2Array [3];
    double t3Array [3];
    // extracted Stratum Value Examples
    int stratumValues [3];
    
    //###################################################
    
    for(int i = 0; i < 3; i++){
        
        //###################################################
        //
        // NTP Port is 123
        // 3 Servers to get NTP Time
        //if ((rv = getaddrinfo("stratum2-4.NTP.TechFak.Uni-Bielefeld.DE", "123", &hints, &servinfo)) != 0) {
        //if ((rv = getaddrinfo("time1.uni-paderborn.de", "123", &hints, &servinfo)) != 0) {
        //if ((rv = getaddrinfo("ntp0.rrze.uni-erlangen.de", "123", &hints, &servinfo)) != 0) {
        
        // Check with local server
        //    if ((rv = getaddrinfo("localhost", "3000", &hints, &servinfo)) != 0) {
        //if ((rv = getaddrinfo("stratum2-4.NTP.TechFak.Uni-Bielefeld.DE", "123", &hints, &servinfo)) != 0) {
        if ((rv = getaddrinfo(servernames[i], "123", &hints, &servinfo)) != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            return 1;
        }
        //###################################################
        
        
        // loop through all the results and make a socket
        for(p = servinfo; p != NULL; p = p->ai_next) {
            if ((sockfd = socket(p->ai_family, p->ai_socktype,
                                 p->ai_protocol)) == -1) {
                perror("talker: socket");
                continue;
            }
            
            break;
        }
        
        if (p == NULL) {
            fprintf(stderr, "failed to create socket\n");
            return 2;
        }
        
        // Get Time T1
        struct timeval tv;
        gettimeofday(&tv, NULL);
        long unsigned unixTimeT1Whole = 1000000 * tv.tv_sec + tv.tv_usec; // Unix Time (1.1.1970)
        //printf("T1 Unix Time: %llu\n", unixTimeT1);
        long unsigned ntpTimeT1Whole = unixTimeT1Whole + 2208988800000000; // NTP Time 1.1.1900
        double ntpTimeT1 = (double)ntpTimeT1Whole/1000000;
        //printf("ntpTimeT1: %lf\n", ntpTimeT1);
        double currentUnixTime = (double)unixTimeT1Whole/1000000;
        
        
        // NTP Protocol 384 Bit
        char* clientRequest = calloc(48, sizeof(int));
        
        // Set VN = 4 and MODE = 3
        // | 0 0 | 1 0 0 | 0 1 1| 0 0 0 ...
        //clientRequest[0] |= 587202560;
        clientRequest[0] |= 35;
        
        // Set Origin Timestamp T1 = ntpTime 1
        printf("Current Unix Time: %lf\n", currentUnixTime);
        clientRequest[6] = ntpTimeT1;
        
        if (sendto(sockfd, clientRequest, 48, 0, p->ai_addr, p->ai_addrlen) == -1) {
            perror("talker: sendto");
            exit(1);
        }
        
        // NTP Protocol 384 Bit
        int* serverReply = calloc(48, sizeof(int));
        
        
        // Receive NTP reply
        if (recvfrom( sockfd, serverReply, 48, 0, (struct sockaddr *)&their_addr, &addr_len ) == -1 ) {
            perror("recv");
            exit(1);
        }
        
        // Get Time T4
        gettimeofday(&tv, NULL);
        unsigned long unixTimeT4 = 1000000 * tv.tv_sec + tv.tv_usec; // Unix Time (1.1.1970)
        //printf("T4 Unix Time: %llu\n", unixTimeT4);
        long unsigned ntpTimeT4Whole = unixTimeT4 + 2208988800000000; // NTP Time 1.1.1900
        double ntpTimeT4 = (double)ntpTimeT4Whole/1000000;
        //printf("T4 Time: %lf\n", ntpTimeT4);
        
        
        unsigned t2;
        unsigned t3;
        
        if(serverReply[8] < 0){
            t2 = serverReply[8] + 2*2147483648;
        }
        else{
            t2 = serverReply[8];
        }
        
        double ntpTimeT2 = ((t2>>24)&0xff) | // move byte 3 to byte 0
        ((t2<<8)&0xff0000) | // move byte 1 to byte 2
        ((t2>>8)&0xff00) | // move byte 2 to byte 1
        ((t2<<24)&0xff000000); // byte 0 to byte 3
        
        unsigned t2fractionReversed = serverReply[9];
        
        unsigned t2fraction = ((t2fractionReversed>>24)&0xff) | // move byte 3 to byte 0
        ((t2fractionReversed<<8)&0xff0000) | // move byte 1 to byte 2
        ((t2fractionReversed>>8)&0xff00) | // move byte 2 to byte 1
        ((t2fractionReversed<<24)&0xff000000); // byte 0 to byte 3
        
        ntpTimeT2 = calculateFraction(ntpTimeT2, t2fraction);
        
        
        if(serverReply[10] < 0){
            t3 = serverReply[10] + 2*2147483648;
        }
        else{
            t3 = serverReply[10];
        }
        
        double ntpTimeT3 = ((t3>>24)&0xff) | // move byte 3 to byte 0
        ((t3<<8)&0xff0000) | // move byte 1 to byte 2
        ((t3>>8)&0xff00) | // move byte 2 to byte 1
        ((t3<<24)&0xff000000); // byte 0 to byte 3
        
        unsigned t3fractionReversed = serverReply[11];
        
        unsigned t3fraction = ((t3fractionReversed>>24)&0xff) | // move byte 3 to byte 0
        ((t3fractionReversed<<8)&0xff0000) | // move byte 1 to byte 2
        ((t3fractionReversed>>8)&0xff00) | // move byte 2 to byte 1
        ((t3fractionReversed<<24)&0xff000000); // byte 0 to byte 3
        
        ntpTimeT3 = calculateFraction(ntpTimeT3, t3fraction);
        
        
        printf("Server: %s\n", servernames[i]);
        printf ("T1: %f\n", ntpTimeT1);
        printf ("T2: %f\n", ntpTimeT2);
        printf ("T3: %f\n", ntpTimeT3);
        printf ("T4: %f\n", ntpTimeT4);
        
        t1Array[i] = ntpTimeT1;
        t2Array[i] = ntpTimeT2;
        t3Array[i] = ntpTimeT3;
        t4Array[i] = ntpTimeT4;
        
        
        int stratum = serverReply[0] << 8;
        stratum = stratum >> 24;
        
        if (stratum < 0){
            stratum += 256;
        }
        
        stratumValues[i] = stratum;
        printf ("Stratum Value is %i\n\n", stratum);
        
        /*
         // Loop ServerReply
         for(int i = 0; i < 12; i++) {
         printf ("%d:  %d\n", i, serverReply[i]);
         }
         */
        
        freeaddrinfo(servinfo);
        
        close(sockfd);
        
        sleep(1);
        
    }
    
    // evaluate measured values to set clock
    //
    // evaluate(int, int array, int array, int array, int array, int array)
    evaluate(servers, stratumValues, t1Array, t2Array, t3Array, t4Array);
    
    return 0;
}

