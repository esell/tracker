#include <string.h>
#include <sched.h>
#include <sys/mman.h>

#include <bcm2835.h>
#include <gps.h>

// Blinks pin 17 which is pin bcm 11 RPI_V2_GPIO_P1_11
#define PIN RPI_V2_GPIO_P1_11


// radio stuff 
char datastring[80];

// gps 
static struct gps_data_t gpsdata;
static unsigned int flags = WATCH_ENABLE;
static float altfactor = METERS_TO_FEET;

void rtty_txstring (char * string);
void rtty_txbyte (char c);
void rtty_txbit (int bit);
uint16_t crc_xmodem_update (uint16_t crc, uint8_t data);
uint16_t gps_CRC16_checksum (char *string);

/* Simple function to sent a char at a time to
 * rtty_txbyte function.
 */
void rtty_txstring (char * string) {
    char c;
    c = *string++;

    while ( c != '\0') {
        rtty_txbyte (c);
        c = *string++;
    }
}

/* Simple function to sent each bit of a char to
 * rtty_txbit function.
 * NB The bits are sent Least Significant Bit first
 *
 * All chars should be preceded with a 0 and
 * proceded with a 1. 0 = Start bit; 1 = Stop bit
 *
 */
void rtty_txbyte (char c) {
    int i;
    rtty_txbit (0); // Start bit

    // Send bits for char LSB first
    // Change this here 7 or 8 for ASCII-7 / ASCII-8
    for (i=0;i<7;i++) {
        if (c & 1) rtty_txbit(1);

        else rtty_txbit(0);

        c = c >> 1;

    }

    rtty_txbit (1); // Stop bit
    rtty_txbit (1); // Stop bit
}

void rtty_txbit (int bit) {
    if (bit) {
        bcm2835_gpio_write(PIN, HIGH);
    }
    else {
        bcm2835_gpio_write(PIN, LOW);

    }
    nanosleep((const struct timespec[]){{0, 20150000L}}, NULL);
}

uint16_t crc_xmodem_update (uint16_t crc, uint8_t data) {
    int i;

    crc = crc ^ ((uint16_t)data << 8);
    for (i=0; i<8; i++) {
        if (crc & 0x8000)
            crc = (crc << 1) ^ 0x1021;
        else
            crc <<= 1;
    }

    return crc;
}

uint16_t gps_CRC16_checksum (char *string) {
    size_t i;
    uint16_t crc;
    uint8_t c;

    crc = 0xFFFF;

    // Calculate checksum ignoring the first two $s
    for (i = 2; i < strlen(string); i++) {
        c = string[i];
        crc = crc_xmodem_update (crc, c);
    }

    return crc;
}

int main() {
    if (gps_open((char *)"localhost", (char *)DEFAULT_GPSD_PORT, &gpsdata) != 0) {
        perror("unable to connect to gpsd");
        return 1;
    }

    // set scheduler as an attempt to make things
    // more "real time"
    struct sched_param param;
    memset(&param, 0, sizeof(param));
    param.sched_priority = sched_get_priority_max(SCHED_FIFO);
    if (sched_setscheduler(0, SCHED_FIFO, &param) != 0) {
        perror("sched_setscheduler");
        return 1;
    } else {
        mlockall(MCL_CURRENT | MCL_FUTURE);
    }

    if (!bcm2835_init())
        return 1;

    (void)gps_stream(&gpsdata, flags, NULL);
    // Set the pin to be an output
    bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_OUTP);

    while(1){
        if (gps_read(&gpsdata) == -1) {
            // no data
        } else {
            // got data
            if (gpsdata.set & PACKET_SET) {
                printf("%d %d %f %f %9.3f %8.2f\n", gpsdata.status, gpsdata.fix.mode, gpsdata.fix.latitude, gpsdata.fix.longitude, gpsdata.fix.altitude * altfactor, gpsdata.fix.speed);
            }
        }

        // lat,long,altitude,speed
        sprintf(datastring,"%f,%f,%.3f,%.2f\n", gpsdata.fix.latitude, gpsdata.fix.longitude,gpsdata.fix.altitude * altfactor, gpsdata.fix.speed); // Puts the text in the datastring
        unsigned int CHECKSUM = gps_CRC16_checksum(datastring);  // Calculates the checksum for this datastring
        char checksum_str[6];
        sprintf(checksum_str, "*%04X\n", CHECKSUM);
        //strcat(datastring,checksum_str);

        rtty_txstring(datastring);
        bcm2835_delay(1500);
    }

    (void)gps_close(&gpsdata);
    bcm2835_close();

    return 0;
}
