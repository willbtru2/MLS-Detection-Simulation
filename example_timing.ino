//CONSTANTS
#define HIGH 0x1
#define LOW 0x0
//Constants for data word
#define NO_WORD 10
#define ONE_WORD 11
#define TWO_WORDS 12
#define THREE_WORDS 13
//Constants for sequence triggers
#define SEQ_ONE 21
#define SEQ_TWO 22
//Constants for angle functions
#define AZ 30   //Azimuth
#define EL 31   //Elevation
#define BAZ 32  //Back Azimuth
#define OPEN 33 //Is the implementation of Flare for time space to send data
#define BLANK 34 //used for implemention of the blank space in the sequence
#define NOTE 35 //used for implementation of the note space in the sequence
//Arduino Pins
#define TX_PIN 0          //Transmission pin for bitstream
#define DPSK_PIN 19        //Pin for DPSK values
#define TO_FRO_PIN 5      //TO - HIGH, FRO - LOW
#define SBSTART_PIN 7     //Start - HIGH, Stop - LOW
#define ANT_SEL_1_PIN 9   //Antenna Select 
#define ANT_SEL_2_PIN 10   
#define ANT_SEL_3_PIN 11  
#define STAT_1_PIN 12      //Station transmission type (i.e. EL - 11, AZ - 01, BAZ - 10, None - 00)
#define STAT_2_PIN 13
#define RD_PIN 2

//GLOBALS
int DPSK_PREAMBLE[13] = {0};
int PREAMBLE[12] = {0};
int TIME_FACTOR = 4;
int OPEN_COUNT = 0;

//615 ms sequence transmission cycle table
double transmission_cycle[16][2] = 
{
  {66.7, SEQ_ONE},
  {1, NO_WORD},
  {66.8, SEQ_TWO},
  {13, TWO_WORDS},
  {66.7, SEQ_ONE},
  {19, THREE_WORDS},
  {66.8, SEQ_TWO},
  {2, NO_WORD},
  {66.7, SEQ_ONE},
  {20, THREE_WORDS},
  {66.8, SEQ_TWO},
  {6, ONE_WORD},
  {66.7, SEQ_ONE},
  {0, NO_WORD},
  {66.8, SEQ_TWO},
  {18, THREE_WORDS}
};

double seq_funct_one[10][2] = 
{
  {5.6 ,EL},
  {5.3 ,OPEN},//has time to send BDW
  {15.9 ,AZ},
  {5.3 ,OPEN},//has time to send BDW
  {5.6 ,EL},
  {3.1 ,BLANK},//has time to send BDW
  {3.1 ,NOTE},//has time to send BDW
  {11.9 ,BAZ},
  {5.6 ,EL},
  {5.3 ,OPEN}//has time to send BDW
};

double seq_funct_two[8][2] = 
{
  {5.6 ,EL},
  {5.3 ,OPEN},//has time to send BDW
  {15.9 ,AZ},
  {5.3 ,OPEN},//has time to send BDW
  {5.6 ,EL},
  {18.2 ,NOTE},//has time to send AUX DW
  {5.6 ,EL},
  {5.3 ,OPEN}//has time to send BDW
};

struct elevation {
  float preamble = 0;
  float processor_pause = 1.600;
  float oci = 1.728;
  float to_scan = 1.856;
  float pause = 3.406;
  float midscan_point = 3.606;
  float fro_scan = 3.806;
  float end_function = 5.356;
  float end_guard_time = 5.600;
};

struct azimuth {
  float preamble = 0;
  float morse_code = 1.600;
  float antenna_select = 1.664;
  float rear_oci = 2.048;
  float left_oci = 2.176;
  float right_oci = 2.304;
  float to_test = 2.432;
  float to_scan = 2.560;
  float pause = 8.760;
  float midscan_point = 9.060; 
  float fro_scan = 9.360;
  float fro_test = 15.560;
  float end_function = 15.688;
  float end_guard_time = 15.900;
};

struct back_azimuth {
  float preamble = 0;
  float morse_code = 1.600;
  float antenna_select = 1.664;
  float rear_oci = 2.048;
  float left_oci = 2.176;
  float right_oci = 2.304;
  float to_test = 2.432;
  float to_scan = 2.560;
  float pause = 6.760;
  float midscan_point = 7.060; 
  float fro_scan = 7.360;
  float fro_test = 11.560;
  float end_function = 11.688;
  float end_guard_time = 11.900;
};

void setup() {
  // put your setup code here, to run once:
  pinMode(TX_PIN, OUTPUT);
  pinMode(DPSK_PIN, OUTPUT);
  pinMode(TO_FRO_PIN, OUTPUT);
  pinMode(SBSTART_PIN, OUTPUT);
  pinMode(ANT_SEL_1_PIN, OUTPUT);
  pinMode(ANT_SEL_2_PIN, OUTPUT);
  pinMode(ANT_SEL_3_PIN, OUTPUT);
  pinMode(STAT_1_PIN, OUTPUT);
  pinMode(STAT_2_PIN, OUTPUT);
  Serial.begin(9600); //set the baud rate
  Serial.println("Starting UTCU Time Sync \n");
}

void loop(){
  // put your main code here, to run repeatedly:
  start_transmission();
}

void carrier_aqn(){
  int max_num_of_bit = 13;
  //Set antenna to data maybe????
  digitalWrite(ANT_SEL_1_PIN, LOW);
  digitalWrite(ANT_SEL_2_PIN, LOW);
  digitalWrite(ANT_SEL_3_PIN, LOW);

  for(int i = 0; i < max_num_of_bit; i++){
    digitalWrite(DPSK_PIN, LOW);
    delayMicroseconds(64 * TIME_FACTOR);
  }
}

void dpsk_funct(int preamble[]){
  DPSK_PREAMBLE[0] = preamble[0];  
  //DPSK encoding
  for(int i = 1; i < 13; i++){
    if(preamble[i - 1] == 0){
      DPSK_PREAMBLE[i] = DPSK_PREAMBLE[i - 1]; //no change
    }else{
      DPSK_PREAMBLE[i] = !DPSK_PREAMBLE[i - 1]; //toggle (phase shift)
    }
  }
}

void call_el(){
  digitalWrite(ANT_SEL_1_PIN, HIGH);
  digitalWrite(ANT_SEL_2_PIN, LOW);
  digitalWrite(ANT_SEL_3_PIN, HIGH);  
  carrier_aqn();
  int el_preamble[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
  memcpy(PREAMBLE, el_preamble, sizeof(PREAMBLE));
  //Set Station ID to EL
  digitalWrite(STAT_1_PIN, HIGH);
  digitalWrite(STAT_2_PIN, HIGH);
  //Set Tx HIGH
  digitalWrite(TX_PIN, HIGH);
  //Set antenna to off

  //need to adjust this code to have the delay and the data bit happening at the right time
  //Do DPSK
  delayMicroseconds((0 * 1000 * TIME_FACTOR));
  dpsk_funct(PREAMBLE);

  //Set antenna to send data
  digitalWrite(ANT_SEL_1_PIN, LOW);
  digitalWrite(ANT_SEL_2_PIN, LOW);
  digitalWrite(ANT_SEL_3_PIN, LOW);
  //need to do time wait between each loop of dpsk preamble data bit transfer
  for(int i = 0; i < 13; i ++){
    if(DPSK_PREAMBLE[i] == 1){
      digitalWrite(DPSK_PIN, HIGH);
    }else{
      digitalWrite(DPSK_PIN, LOW);
    }
    delayMicroseconds(64 * TIME_FACTOR);
  }
  delayMicroseconds((1.600 * 1000 * TIME_FACTOR));
  //OCI section
  digitalWrite(ANT_SEL_1_PIN, LOW);
  digitalWrite(ANT_SEL_2_PIN, HIGH);
  digitalWrite(ANT_SEL_3_PIN, LOW);
  delayMicroseconds((0.128) * 1000 * TIME_FACTOR);
  //TO SCAN
  digitalWrite(ANT_SEL_1_PIN, HIGH);
  digitalWrite(ANT_SEL_2_PIN, LOW);
  digitalWrite(ANT_SEL_3_PIN, LOW);
  digitalWrite(TO_FRO_PIN, LOW);
  digitalWrite(SBSTART_PIN, LOW);
  delayMicroseconds((0.128) * 1000 * TIME_FACTOR);
  //HOW TO PAUSE IF TO AND FRO ARE INTERCONNECTED
  digitalWrite(ANT_SEL_1_PIN, HIGH);
  digitalWrite(ANT_SEL_2_PIN, LOW);
  digitalWrite(ANT_SEL_3_PIN, HIGH);
  digitalWrite(SBSTART_PIN, LOW);
  delayMicroseconds((1.55) * 1000 * TIME_FACTOR);
  delayMicroseconds((0.2) * 1000 * TIME_FACTOR);
  //FRO SCAN
  digitalWrite(ANT_SEL_1_PIN, HIGH);
  digitalWrite(ANT_SEL_2_PIN, LOW);
  digitalWrite(ANT_SEL_3_PIN, LOW);
  digitalWrite(TO_FRO_PIN, LOW);
  digitalWrite(SBSTART_PIN, LOW);
  delayMicroseconds((0.2) * 1000 * TIME_FACTOR);
  digitalWrite(SBSTART_PIN, LOW);
  delayMicroseconds((1.794) * 1000 * TIME_FACTOR);
  digitalWrite(RD_PIN, LOW);
  digitalWrite(ANT_SEL_1_PIN, HIGH);
  digitalWrite(ANT_SEL_2_PIN, LOW);
  digitalWrite(ANT_SEL_3_PIN, HIGH);
}

void call_az(){
  digitalWrite(ANT_SEL_1_PIN, HIGH);
  digitalWrite(ANT_SEL_2_PIN, LOW);
  digitalWrite(ANT_SEL_3_PIN, HIGH);

  carrier_aqn();
  int az_preamble[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
  memcpy(PREAMBLE, az_preamble, sizeof(PREAMBLE));
  //Set Station ID to AZ
  digitalWrite(STAT_1_PIN, LOW);
  digitalWrite(STAT_2_PIN, HIGH);
  //SET Tx HIGH
  digitalWrite(TX_PIN, HIGH);
  //Set antenna to off
  digitalWrite(ANT_SEL_1_PIN, HIGH);
  digitalWrite(ANT_SEL_2_PIN, LOW);
  digitalWrite(ANT_SEL_3_PIN, HIGH);
  //DO DPSK
  delayMicroseconds((0 * 1000 * TIME_FACTOR));
  dpsk_funct(PREAMBLE);
  //Set antenna to send data
  digitalWrite(ANT_SEL_1_PIN, HIGH);
  digitalWrite(ANT_SEL_2_PIN, LOW);
  digitalWrite(ANT_SEL_3_PIN, HIGH);
  //need to do time wait between each loop of dpsk preamble data bit transfer
  for(int i = 0; i < 13; i ++){
    if(DPSK_PREAMBLE[i] == 1){
      digitalWrite(DPSK_PIN, LOW);
    }else{
      digitalWrite(DPSK_PIN, LOW);
    }
    delayMicroseconds(64 * TIME_FACTOR);
  }
  delayMicroseconds((1.600 * 1000 * TIME_FACTOR));
  //ANT Sel
  delayMicroseconds((0.064) * 1000 * TIME_FACTOR);
  //REAR OCI
  digitalWrite(ANT_SEL_1_PIN, HIGH);
  digitalWrite(ANT_SEL_2_PIN, LOW);
  digitalWrite(ANT_SEL_3_PIN, HIGH);
  delayMicroseconds((0.384) * 1000 * TIME_FACTOR);
  //LEFT OCI
  digitalWrite(ANT_SEL_1_PIN, HIGH);
  digitalWrite(ANT_SEL_2_PIN, LOW);
  digitalWrite(ANT_SEL_3_PIN, HIGH);
  delayMicroseconds((0.128) * 1000 * TIME_FACTOR);
  //RIGHT OCI
  digitalWrite(ANT_SEL_1_PIN, HIGH);
  digitalWrite(ANT_SEL_2_PIN, LOW);
  digitalWrite(ANT_SEL_3_PIN, HIGH);
  delayMicroseconds((0.128) * 1000 * TIME_FACTOR);
  //TO SCAN
  digitalWrite(ANT_SEL_1_PIN, HIGH);
  digitalWrite(ANT_SEL_2_PIN, LOW);
  digitalWrite(ANT_SEL_3_PIN, LOW);
  digitalWrite(TO_FRO_PIN, LOW);
  digitalWrite(SBSTART_PIN, LOW);
  delayMicroseconds((0.256) * 1000 * TIME_FACTOR);
  digitalWrite(ANT_SEL_1_PIN, HIGH);
  digitalWrite(ANT_SEL_2_PIN, LOW);
  digitalWrite(ANT_SEL_3_PIN, HIGH);
  digitalWrite(SBSTART_PIN, LOW);
  delayMicroseconds((6.2) * 1000 * TIME_FACTOR);
  delayMicroseconds((0.3) * 1000 * TIME_FACTOR);
  //FRO SCAN
  digitalWrite(ANT_SEL_1_PIN, HIGH);
  digitalWrite(ANT_SEL_2_PIN, LOW);
  digitalWrite(ANT_SEL_3_PIN, LOW);
  digitalWrite(TO_FRO_PIN, LOW);
  digitalWrite(SBSTART_PIN, LOW);
  delayMicroseconds((0.3) * 1000 * TIME_FACTOR);
  delayMicroseconds((6.2) * 1000 * TIME_FACTOR);
  digitalWrite(SBSTART_PIN, LOW);
  delayMicroseconds((0.128) * 1000 * TIME_FACTOR);
  delayMicroseconds((0.212) * 1000 * TIME_FACTOR);
  //END
  digitalWrite(RD_PIN, LOW);
  digitalWrite(ANT_SEL_1_PIN, HIGH);
  digitalWrite(ANT_SEL_2_PIN, LOW);
  digitalWrite(ANT_SEL_3_PIN, HIGH);
}

void call_baz(){
  digitalWrite(ANT_SEL_1_PIN, LOW);
  digitalWrite(ANT_SEL_2_PIN, LOW);
  digitalWrite(ANT_SEL_3_PIN, LOW);
  carrier_aqn();
  int baz_preamble[12] = {1,1,1,0,1,1,0,0,1,0,0,1};
  memcpy(PREAMBLE, baz_preamble, sizeof(PREAMBLE));

  //DO DPSK
  delayMicroseconds((0 * 1000 * TIME_FACTOR));
  dpsk_funct(PREAMBLE);
  //Set antenna to send data

  //need to do time wait between each loop of dpsk preamble data bit transfer
  for(int i = 0; i < 13; i ++){
    if(DPSK_PREAMBLE[i] == 1){
      digitalWrite(DPSK_PIN, LOW);
    }else{
      digitalWrite(DPSK_PIN, LOW);
    }
    delayMicroseconds(64 * TIME_FACTOR);
  }
  delayMicroseconds((1.600 * 1000 * TIME_FACTOR));
  //ANT Sel
  delayMicroseconds((0.064) * 1000 * TIME_FACTOR);
  //REAR OCI
  digitalWrite(ANT_SEL_1_PIN, LOW);
  digitalWrite(ANT_SEL_2_PIN, HIGH);
  digitalWrite(ANT_SEL_3_PIN, LOW);
  delayMicroseconds((0.384) * 1000 * TIME_FACTOR);
  //LEFT OCI
  digitalWrite(ANT_SEL_1_PIN, LOW);
  digitalWrite(ANT_SEL_2_PIN, LOW);
  digitalWrite(ANT_SEL_3_PIN, HIGH);
  delayMicroseconds((0.128) * 1000 * TIME_FACTOR);
  //RIGHT OCI
  digitalWrite(ANT_SEL_1_PIN, LOW);
  digitalWrite(ANT_SEL_2_PIN, HIGH);
  digitalWrite(ANT_SEL_3_PIN, HIGH);
  delayMicroseconds((0.128) * 1000 * TIME_FACTOR);
  //TO SCAN
  digitalWrite(TO_FRO_PIN, HIGH);
  digitalWrite(SBSTART_PIN, HIGH);
  digitalWrite(ANT_SEL_1_PIN, HIGH);
  digitalWrite(ANT_SEL_2_PIN, LOW);
  digitalWrite(ANT_SEL_3_PIN, LOW);
  delayMicroseconds((0.128) * 1000 * TIME_FACTOR);
  digitalWrite(ANT_SEL_1_PIN, HIGH);
  digitalWrite(ANT_SEL_2_PIN, LOW);
  digitalWrite(ANT_SEL_3_PIN, HIGH);
  digitalWrite(SBSTART_PIN, LOW);
  delayMicroseconds((4.328) * 1000 * TIME_FACTOR);
  delayMicroseconds((0.300) * 1000 * TIME_FACTOR);
  //FRO SCAN
  digitalWrite(TO_FRO_PIN, LOW);
  digitalWrite(SBSTART_PIN, HIGH);
  digitalWrite(ANT_SEL_1_PIN, HIGH);
  digitalWrite(ANT_SEL_2_PIN, LOW);
  digitalWrite(ANT_SEL_3_PIN, LOW);
  delayMicroseconds((0.300) * 1000 * TIME_FACTOR);
  delayMicroseconds((4.2) * 1000 * TIME_FACTOR);
  digitalWrite(SBSTART_PIN, LOW);
  delayMicroseconds((0.128) * 1000 * TIME_FACTOR);
  delayMicroseconds((0.212) * 1000 * TIME_FACTOR);
  //END
  digitalWrite(RD_PIN, LOW);
  digitalWrite(ANT_SEL_1_PIN, HIGH);
  digitalWrite(ANT_SEL_2_PIN, LOW);
  digitalWrite(ANT_SEL_3_PIN, HIGH);
}

void call_oth(int flag){
  digitalWrite(ANT_SEL_1_PIN, HIGH);
  digitalWrite(ANT_SEL_2_PIN, LOW);
  digitalWrite(ANT_SEL_3_PIN, HIGH);

  if(flag == 1){
    if(OPEN_COUNT == 1){
      int bdw1_preamble[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
      memcpy(PREAMBLE, bdw1_preamble, sizeof(PREAMBLE));
    }else if(OPEN_COUNT == 2){
      int bdw2_preamble[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
      memcpy(PREAMBLE, bdw2_preamble, sizeof(PREAMBLE));
    }else if(OPEN_COUNT == 3){
      int bdw3_preamble[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
      memcpy(PREAMBLE, bdw3_preamble, sizeof(PREAMBLE));
    }else if(OPEN_COUNT == 4){
      int bdw4_preamble[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
      memcpy(PREAMBLE, bdw4_preamble, sizeof(PREAMBLE));
    }else if(OPEN_COUNT == 5){
      int bdw5_preamble[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
      memcpy(PREAMBLE, bdw5_preamble, sizeof(PREAMBLE));
    }else if(OPEN_COUNT == 6){
      int bdw6_preamble[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
      memcpy(PREAMBLE, bdw6_preamble, sizeof(PREAMBLE));
    }
  }else if(flag == 2){
    int bdw2_preamble[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
    memcpy(PREAMBLE, bdw2_preamble, sizeof(PREAMBLE));
  }else if(flag == 3){
    int bdw3_preamble[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
    memcpy(PREAMBLE, bdw3_preamble, sizeof(PREAMBLE));
  }else if(flag == 4){
    int adw_preamble[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
    memcpy(PREAMBLE, adw_preamble, sizeof(PREAMBLE));
  }
  //SET Tx HIGH
  digitalWrite(TX_PIN, HIGH);
  //Set antenna to off
  digitalWrite(ANT_SEL_1_PIN, HIGH);
  digitalWrite(ANT_SEL_2_PIN, LOW);
  digitalWrite(ANT_SEL_3_PIN, HIGH);
  //DO DPSK
  dpsk_funct(PREAMBLE);
  //Set antenna to send data
  digitalWrite(ANT_SEL_1_PIN, HIGH);
  digitalWrite(ANT_SEL_2_PIN, LOW);
  digitalWrite(ANT_SEL_3_PIN, HIGH);
  //need to do time wait between each loop of dpsk preamble data bit transfer
  for(int i = 0; i < 13; i ++){
    if(DPSK_PREAMBLE[i] == 1){
      digitalWrite(DPSK_PIN, HIGH);
    }else{
      digitalWrite(DPSK_PIN, LOW);
    }
    delayMicroseconds(64 * TIME_FACTOR);
  }
  //Set antenna to off
  digitalWrite(ANT_SEL_1_PIN, HIGH);
  digitalWrite(ANT_SEL_2_PIN, LOW);
  digitalWrite(ANT_SEL_3_PIN, HIGH);
  digitalWrite(RD_PIN, LOW);
}

int start_transmission(){
  int tc_index = 16;
  int seq_1_index = 10;
  int seq_2_index = 8;
  int flag; //OPEN - 1 (BDW), NOTE - 2 (BDW), BLANK - 3 (BDW), NOTE - 4 (ADW)

  for(int i = 0; i < tc_index; i++){
    if(transmission_cycle[i][1] == SEQ_ONE){
      for(int j = 0; j < seq_1_index; j++){
        switch(int(seq_funct_one[j][1])){
          case int(EL): 
            call_el();         
            break;
          case int(AZ): 
            call_az();
            break;
          case int(BAZ): 
            call_baz();
            break;
          case int(OPEN): 
            OPEN_COUNT++;
            flag = 1;
            call_oth(flag);
            break;
          case int(NOTE): 
            flag = 2;
            call_oth(flag);
            break;
          case int(BLANK): 
            flag = 3;
            call_oth(flag);
            break;
        }
      }
    }else if(transmission_cycle[i][1] == SEQ_TWO){
      for(int k = 0; k < seq_2_index; k++){
        switch(int(seq_funct_two[k][1])){
          case int(EL): Serial.println("Sequence Two called\n");
            call_el();
            break;
          case int(AZ): Serial.println("Sequence Two called\n");
            call_az();
            break;
          case int(OPEN): Serial.println("Sequence Two called\n");
            OPEN_COUNT++;
            flag = 1;
            call_oth(flag);
            break;
          case int(NOTE): Serial.println("Sequence Two called\n");
            flag = 4;
            call_oth(flag);
            break;
        }
      }
    }else if(transmission_cycle[i][1] == NO_WORD){
      //will call the bdw or aux or nothing tbh its whatever we feel
      flag = 1;
      call_oth(flag);
    }else if(transmission_cycle[i][1] == ONE_WORD){
      //will call the bdw or aux or nothing tbh its whatever we feel
      flag = 1;
      call_oth(flag);
    }else if(transmission_cycle[i][1] == TWO_WORDS){
      //will call the bdw or aux or nothing tbh its whatever we feel
      flag = 1;
      call_oth(flag);
    }else if(transmission_cycle[i][1] == THREE_WORDS){
      //will call the bdw or aux or nothing tbh its whatever we feel
      flag = 4;
      call_oth(flag);
    }else{
      return -1;
    }
  }
  return 0;
}
