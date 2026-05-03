{2047, 2059, 2071, 2083, 2092, 2100, 2106, 2110, 
 2111, 2110, 2106, 2100, 2092, 2083, 2071, 2059, 
 2047, 2035, 2023, 2011, 2002, 1994, 1988, 1984,
 1983, 1984, 1988, 1994, 2002, 2011, 2023, 2035}
#include "stdio.h"

//OUTPUT PIN
#define OUTPUT_PIN A0
#define RX_PIN 7
#define DELAY 1

//Lengths
#define BITSTREAM_LEN 7
#define ANT_SWITCH_LEN 3
#define NUM_SAMPLES 32

//INDEXING
#define TX_ENABLE_OFFSET 0
#define PHASE_OFFSET 1
#define TO_FRO_OFFSET 2
#define SB_START 3
#define ANT_SWITCH_OFFSET 4

//ANTENNA SWITCH POSITIONS - 3 bits
#define IDENT_DATA 0
#define LEFT_OCI 1
#define REAR_OCI 2
#define RIGHT_OCI 3
#define SCAN_BEAM 4
#define ANT_OFF 5

//RELATIVE ANTENNA AMPLITUDES
#define IDENT_DATA_AMP 1
#define LEFT_OCI_AMP 0.5
#define REAR_OCI_AMP 0.25
#define RIGHT_OCI_AMP 0.125
#define SCAN_BEAM_AMP 10
#define ANT_OFF_AMP 0

//RECEIVER POSITIONS (Degrees)  
#define RCV_POS_EL 3
#define RCV_POS_AZ -10
#define RCV_POS_BAS -10

//dB BEAMWIDTH (Degrees)
#define BW_EL 1.5
#define BW_AZ 2
#define BW_BAS 2

//SCAN ANGLES (Degrees)
#define MAX_SCAN_ANGLE_EL 29.5
#define MAX_SCAN_ANGLE_AZ 62
#define MAX_SCAN_ANGLE_BAZ 42

enum STATION{EL, AZ, BAZ};

struct USSIM{

    int bitstream[BITSTREAM_LEN];   // array to store the incoming bitstream
    int *antenna;
    int index;                      // signal amplitude, changed by antenna switches
    int phase;                      // controls the phase of the generated wave (either 0 or pi)
    enum STATION station;           // the type of ground station
};

//FUNCTIONS

//initializeDevice: initalizes all member variables
void initializeDevice(struct USSIM *cmd);

//rcvMsg: recieves the incoming message from UTCU
void receiveMsg(struct USSIM *cmd);

//transmit: transmits the signal
void transmit(struct USSIM *cmd);

//plotSerial: plots the signal on the serial monitor
void plotSerial(struct USSIM *cmd);

//genCarrier: generates the carrier wave
void genCarrier(struct USSIM *cmd);

//shiftPhase: shifts the phase by pi
void shiftPhase(struct USSIM *cmd);

//ANTENNAS
int antennaIndent[NUM_SAMPLES] = {2047, 2147, 2243, 2331, 2409, 2473, 2520, 2549, 
                                   2559, 2549, 2520, 2473, 2409, 2331, 2243, 2147, 
                                   2047, 1947, 1851, 1763, 1685, 1621, 1574, 1545, 
                                   1535, 1545, 1574, 1621, 1685, 1763, 1851, 1947};
int antennaLeftOCI[NUM_SAMPLES] = {2047, 2097, 2145, 2189, 2228, 2260, 2283, 2298, 
                                   2303, 2298, 2283, 2260, 2228, 2189, 2145, 2097, 
                                   2047, 1997, 1949, 1905, 1866, 1834, 1811, 1796,
                                   1791, 1796, 1811, 1834, 1866, 1905, 1949, 1997};
int antennaUpperOCI[NUM_SAMPLES] = {2047, 2072, 2096, 2118, 2137, 2153, 2165, 2173,
                                    2175, 2173, 2165, 2153, 2137, 2118, 2096, 2072, 
                                    2047, 2022, 1998, 1976, 1957, 1941, 1929, 1921, 
                                    1919, 1921, 1929, 1941, 1957, 1976, 1998, 2022};
int antennaRightOCI[NUM_SAMPLES] = {2047, 2059, 2071, 2083, 2092, 2100, 2106, 2110, 
                                    2111, 2110, 2106, 2100, 2092, 2083, 2071, 2059, 
                                    2047, 2035, 2023, 2011, 2002, 1994, 1988, 1984, 
                                    1983, 1984, 1988, 1994, 2002, 2011, 2023, 2035};

int antennaAZ[NUM_SAMPLES];
int antennaEL[NUM_SAMPLES];
int antennaBAZ[NUM_SAMPLES];

//USSIM FUNCTIONS
void initializeDevice(struct USSIM *cmd){

    cmd->index = 0;
    cmd->phase = 0;
    cmd->station = EL;
    cmd->antenna = antenna1;
}

void receiveMsg(struct USSIM *cmd){

  for(int i = 0; i < BITSTREAM_LEN; i++){
    
    if(digitalRead(RX_PIN)){
      cmd->bitstream[i] = 1;
    }
    else{
      cmd->bitstream[i] = 0;
    }

    Serial.println(cmd->bitstream[i], DEC);
    delayMicroseconds(DELAY);
  }


}

//transmit: transmits the signal
void transmit(struct USSIM *cmd){

    while(1){
        analogWrite(OUTPUT_PIN, cmd->antenna[(cmd->index + cmd->phase) % NUM_SAMPLES]);
    }

}

//plotSerial: plots the signal on the serial monitor
void plotSerial(struct USSIM *cmd){

    Serial.begin(9600);
    while(1){
        Serial.println(cmd->antenna[(cmd->index + cmd->phase) % NUM_SAMPLES]);
    }
}

//genCarrier: generates the carrier wave
void genCarrier(struct USSIM *cmd){


}

//switchAntenna: functions to switch the antenna
void switchAntenna(struct USSIM *cmd){

    //parse the bit information from the array
    int antennaSwitch = 0;
    for(int i = 0; i < ANT_SWITCH_LEN; i++){
        antennaSwitch += (cmd->bitstream[ANT_SWITCH_OFFSET + i] & 1);

        //do all but on the last
        if(i != ANT_SWITCH_LEN-1){
            antennaSwitch <<= 1;
        }
    }

    //switch statement to select the relative amplitude
    switch(antennaSwitch){

        case IDENT_DATA:
            cmd->antenna = antenna1;
            break;

        case LEFT_OCI:
            cmd->antenna = antenna2;
            break;

        case REAR_OCI:
            
            break;

        case RIGHT_OCI:
            
            break;

        case SCAN_BEAM:
            
            break;

        case ANT_OFF:
            
            break;

        default:
            break;
    }
}

//shiftPhase: shifts the phase by pi
void shiftPhase(struct USSIM *cmd){

    if(cmd->bitstream[PHASE_OFFSET]){
        cmd->phase = NUM_SAMPLES/4;
    }
    else{
        cmd->phase = 0;
    }
}

//Program Execution--------------------------------------------------------------------
struct USSIM cmd;

void setup(){
  //REG_TCC0_CTRLA |= TCC_CTRLA_PRESCALER_DIV1;  // No prescaler
  //REG_TCC0_PER = (48000000 / SAMPLE_RATE) - 1;  // Adjust for desired frequency
  analogWriteResolution(12);
  pinMode(OUTPUT_PIN, OUTPUT);
  initializeDevice(&cmd);
  cmd.bitstream[ANT_SWITCH_OFFSET] = 0; cmd.bitstream[ANT_SWITCH_OFFSET+1] = 0; cmd.bitstream[ANT_SWITCH_OFFSET+2] = 1;
  switchAntenna(&cmd);
}

void loop() {

  int i = 0;
  for (i; i < NUM_SAMPLES; i++) {
    analogWrite(OUTPUT_PIN, cmd.antenna[i + cmd.phase]);
  }

}
