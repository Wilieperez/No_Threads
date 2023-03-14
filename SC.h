#define PORT 5002
#define BACKLOG 2
#define PREAMB 170
#define SENSOR_FAIL 254
#define SIZE_FAIL 0
#define CRC_FAIL 255

typedef struct
{
    uint8_t preamb;
    char source[15];
    char destination[15];
    char payload[50];
}frame_msg;