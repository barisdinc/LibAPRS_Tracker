#include "Arduino.h"
#include "AFSK.h"
#include "AX25.h"

Afsk modem;
AX25Ctx AX25;
#define countof(a) sizeof(a)/sizeof(a[0])

unsigned long custom_preamble = 350UL;
unsigned long custom_tail = 50UL;

AX25Call src;
AX25Call dst;
AX25Call path1;
AX25Call path2;

char CALL[7] = "NOCALL";
int CALL_SSID = 0;
char DST[7] = "HYMTR";
int DST_SSID = 0;
char PATH1[7] = "WIDE1";
int PATH1_SSID = 1;
char PATH2[7] = "WIDE2";
int PATH2_SSID = 2;

AX25Call path[4];

// Location packet assembly fields
char latitude[9];
char longtitude[10];
char symbolTable = '/';
char symbol = '>';

uint8_t power = 10;
uint8_t height = 10;
uint8_t gain = 10;
uint8_t directivity = 9;

char speed[3];
char course[3] ;
char direction[3];

void APRS_init() {
    AFSK_init(&modem);
    ax25_init(&AX25);
}

void APRS_setCallsign(char *call, int ssid) {
    memset(CALL, 0, 7);
    int i = 0;
    while (i < 6 && call[i] != 0) {
        CALL[i] = call[i];
        i++;
    }
    CALL_SSID = ssid;
}

void APRS_setDestination(char *call, int ssid) {
    memset(DST, 0, 7);
    int i = 0;
    while (i < 6 && call[i] != 0) {
        DST[i] = call[i];
        i++;
    }
    DST_SSID = ssid;
}

void APRS_setPath1(char *call, int ssid) {
    memset(PATH1, 0, 7);
    int i = 0;
    while (i < 6 && call[i] != 0) {
        PATH1[i] = call[i];
        i++;
    }
    PATH1_SSID = ssid;
}

void APRS_setPath2(char *call, int ssid) {
    memset(PATH2, 0, 7);
    int i = 0;
    while (i < 6 && call[i] != 0) {
        PATH2[i] = call[i];
        i++;
    }
    PATH2_SSID = ssid;
}

void APRS_setPreamble(unsigned long pre) {
    custom_preamble = pre;
}

void APRS_setTail(unsigned long tail) {
    custom_tail = tail;
}

void APRS_useAlternateSymbolTable(bool use) {
    if (use) {
        symbolTable = '\\';
    } else {
        symbolTable = '/';
    }
}

void APRS_setSymbol(char sym) {
    symbol = sym;
}

void APRS_setLat(char *lat) {
    memset(latitude, 0, 9);
    int i = 0;
    while (i < 8 && lat[i] != 0) {
        latitude[i] = lat[i];
        i++;
    }
}

void APRS_setLon(char *lon) {
    memset(longtitude, 0, 10);
    int i = 0;
    while (i < 9 && lon[i] != 0) {
        longtitude[i] = lon[i];
        i++;
    }
}

void APRS_setPower(int s) {
    if (s >= 0 && s < 10) {
        power = s;
    }
}

void APRS_setHeight(int s) {
    if (s >= 0 && s < 10) {
        height = s;
    }
}

void APRS_setGain(int s) {
    if (s >= 0 && s < 10) {
        gain = s;
    }
}

void APRS_setDirectivity(int s) {
    if (s >= 0 && s < 10) {
        directivity = s;
    }
}

void APRS_setSpeed(int s) {
    if (s >= 0 && s < 1000) {
        snprintf(speed,3,"%03d",s) ;
    }
}

void APRS_setCourse(int s) {
    if (s >= 0 && s < 360) {
        snprintf(course,3,"%03d",s) ;
    }
}

void APRS_setDirection(int s) {
    if (s >= 0 && s < 360) {
        snprintf(direction,3,"%03d",s) ;
    }
}

void APRS_sendPkt(void *_buffer, size_t length) {
    uint8_t *buffer = (uint8_t *)_buffer;
    memcpy(dst.call, DST, 6);
    dst.ssid = DST_SSID;
    memcpy(src.call, CALL, 6);
    src.ssid = CALL_SSID;
    memcpy(path1.call, PATH1, 6);
    path1.ssid = PATH1_SSID;
    memcpy(path2.call, PATH2, 6);
    path2.ssid = PATH2_SSID;

    path[0] = dst;
    path[1] = src;
    path[2] = path1;
    path[3] = path2;

    ax25_sendVia(&AX25, path, countof(path), buffer, length);
}

// Dynamic RAM usage of this function is 30 bytes
void APRS_sendLoc(void *_buffer, size_t length, char packetType) {
    size_t payloadLength = 20+length;
    bool usePHG = false;
    bool useCSE = false;
    bool useDIR = false;
    switch (packetType)
    {
    case ' ': //No extension to packet
        break;
    case 'p': //PHG
        usePHG = true;
        //break;
    case 'c': // CSE/SPD
        useCSE = true;
    case 'd': // DIR/SPD
        useDIR = true;
    default:
        payloadLength += 7;
        break;
    } 
    /*
    if (power < 10 && height < 10 && gain < 10 && directivity < 9) {
        usePHG = true;
        payloadLength += 7;
    }
    if (course > 0 && course < 1000 && speed > 0 && speed < 1000) {
        useCSE = true;
        payloadLength += 7;
    }
    if (direction > 0 && direction < 360 && speed > 0 && speed < 1000) {
        useDIR = true;
        payloadLength += 7;
    }
    */
    uint8_t *packet = (uint8_t*)malloc(payloadLength);
    uint8_t *ptr = packet;
    packet[0] = '=';
    packet[9] = symbolTable;
    packet[19] = symbol;
    ptr++;
    memcpy(ptr, latitude, 8);
    ptr += 9;
    memcpy(ptr, longtitude, 9);
    ptr += 10;
    if (usePHG) {
        packet[20] = 'P';
        packet[21] = 'H';
        packet[22] = 'G';
        packet[23] = power+48;
        packet[24] = height+48;
        packet[25] = gain+48;
        packet[26] = directivity+48;
        ptr+=7;
    }
    if (useCSE) {
        packet[20] = course[0];
        packet[21] = course[1];
        packet[22] = course[2];
        packet[23] = '/';
        packet[24] = speed[0];
        packet[25] = speed[1];
        packet[26] = speed[2];
        ptr+=7;
    }
    if (useDIR) {
        packet[20] = direction[0];
        packet[21] = direction[1];
        packet[22] = direction[2];
        packet[23] = '/';
        packet[24] = speed[0];
        packet[25] = speed[1];
        packet[26] = speed[2];
        ptr+=7;
    }
    if (length > 0) {
        uint8_t *buffer = (uint8_t *)_buffer;
        memcpy(ptr, buffer, length);
    }

    APRS_sendPkt(packet, payloadLength);
    free(packet);
}
