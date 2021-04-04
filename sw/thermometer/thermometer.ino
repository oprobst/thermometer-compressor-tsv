#include <OneWire.h>
#include <DallasTemperature.h>
#include <U8g2lib.h>
#include <Adafruit_MAX31855.h>

/* Ardunio Script for temperatur sensor.
 * 
 * I kept the whole logic in this single script to make it easily adjustable for beginners.
 * 
 
 * 
 */

//Configuration: Beep on startup 1= BEEP 0= OFF
#define BEEP_ON_STARTUP 0
// don't change, given by Dallas Instruments. Size of the sensor address.
#define BYTE_SIZE_ADDRESS 8
// size of the name for a sensor.
#define  BYTE_SIZE_NAME 16
// Pin serving das data port for the sensors.
#define ONE_WIRE_BUS A1
// Define the SPI Chip Select pin for LCD Display
#define CS_PIN_LCD 2
#define RST_PIN_LCD 3

#define CS_PIN_T1 6
#define CS_PIN_T2 5
#define CS_PIN_T3 4

#define BUZZER_PIN 8

/* Create an instance of the library for the 12864 LCD in SPI mode */
U8G2_ST7920_128X64_2_HW_SPI u8g2(U8G2_R0, CS_PIN_LCD, RST_PIN_LCD);

/*
 *  data struct for one sensor, name of maximum BYTE_SIZE_NAME and the unique address (given by producer)
 */
struct ds18b20_sensor {
  char name [BYTE_SIZE_NAME];
  byte address [BYTE_SIZE_ADDRESS];
};

/*
 *  Define all ds18b20_sensors here.
 *  First a human readable name and next the physical address of the sensor in hex.
 */
struct ds18b20_sensor ds18b20_sensors [] = {
  {"Sensor Innen" , {0x28, 0xFF, 0xA4, 0x20, 0x54, 0x17, 0x04, 0x12}},
  {"Sensor Außen", {0x28, 0xFF, 0x76, 0x20, 0x60, 0x17, 0x04, 0x8F}}
};

/*
 * Data set for one temperature measurement. Stores historical min/max value
 */
struct temperatur {
  float current;
  int min;
  int max;
};

struct temperatur temperatures [] = {
  {-128, 2048,-128},
  {-128, 2048,-128},
  {-128, 2048,-128},
  {-128, 2048,-128},
  {-128, 2048,-128}
};

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);
// Pass oneWire reference to Dallas Temperature.
DallasTemperature dallasSensors(&oneWire);

Adafruit_MAX31855 ts1(CS_PIN_T1);
Adafruit_MAX31855 ts2(CS_PIN_T2);
Adafruit_MAX31855 ts3(CS_PIN_T3);

//Number of ds18b20_sensors to be determined
const uint8_t amountKnownSensors =  sizeof(ds18b20_sensors) / sizeof(ds18b20_sensor);

void setup(void)
{
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite (BUZZER_PIN, HIGH);
  if (BEEP_ON_STARTUP) delay (800);
  digitalWrite (BUZZER_PIN, LOW);
  
  
  Serial.begin(9600);
  Serial.println("==================================================");
  Serial.print("Sensors in known ds18b20_sensor list: " );
  Serial.println( amountKnownSensors); 
  dallasSensors.begin();
  if (!ts1.begin()) {
    Serial.println("ERROR.");
    while (1) delay(10);
  }
   if (!ts2.begin()) {
    Serial.println("ERROR.");
    while (1) delay(10);
  }

  
  u8g2.begin();    
}

//Still test code here:
void loop(void)
{
  byte addr [BYTE_SIZE_ADDRESS];

  Serial.println("");
  // call dallasSensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  /********************************************************************/
  Serial.print(" Requesting ");
  Serial.print (dallasSensors.getDS18Count());
  Serial.print (" temperatures...");
  dallasSensors.requestTemperatures(); // Send the command to get temperature readings
  Serial.println("DONE");

  /********************************************************************/

  for (uint8_t i = 0; i < dallasSensors.getDS18Count(); i++) {
    Serial.print(i);
    Serial.print(". Temperature is: ");
    Serial.print(dallasSensors.getTempCByIndex(i));
    Serial.print(" from address ");
    dallasSensors.getAddress(addr, i);   
    char addressStringBuff [24];
    getStringForSensorAddress(addressStringBuff, addr );
    Serial.print(addressStringBuff);
    Serial.print(" at position ");
    
    uint8_t index;
    Serial.print(getNameForSensorAddress( addr, &index ));
    setNewMinMax(index, dallasSensors.getTempCByIndex(i)) ;

    
    Serial.println(".");
  
  }

 double c = ts1.readCelsius();
   if (isnan(c)) {
     Serial.println("Something wrong with thermocouple 1!");
   } else {
    setNewMinMax(2, c); 
   }
     c = ts2.readCelsius();
   if (isnan(c)) {
     Serial.println("Something wrong with thermocouple 2!");
   } else {
    setNewMinMax(3, c);
   }
    c = ts3.readCelsius();
   if (isnan(c)) {
     Serial.println("Something wrong with thermocouple 3!");
   } else {
    setNewMinMax(4, c);
   }
  
   
  updateDisplay();
  delay(1000);
}



/*
 * Set new min/max/current value for given index.
 */
void setNewMinMax (int index, float currentTemperature){
  if (temperatures[index].min > currentTemperature){
    temperatures[index].min = currentTemperature;
  }
  if (temperatures[index].max < currentTemperature){
    temperatures[index].max = currentTemperature;
  }
   temperatures[index].current = currentTemperature;
}

/*
 * Read the byte address and translate it into a human readable string containing
 * hex values. 
 * buf: buffer to write the created string into. Must be of size 24!
 * addr: The address to parse.
 */
void getStringForSensorAddress (char * buf , byte * addr) {
    unsigned char * pin = addr;
    const char * hex = "0123456789ABCDEF";
    char * pout = buf;
    for(; pin < addr+BYTE_SIZE_ADDRESS; pout+=3, pin++){
        pout[0] = hex[(*pin>>4) & 0xF];
        pout[1] = hex[ *pin     & 0xF];
        pout[2] = ':';
    }
    pout[-1] = 0;
}

/*
 * Iterate through the ds18b20_sensors array and compare hard coded sensor addresses with the one read from the sensor.
 * Return an const char * with the name stored in the array.
 * Set the index of the sensor to the *index var.
 */
const char * getNameForSensorAddress( byte * addr, uint8_t * index) {
  boolean found = true;
  for (uint8_t j = 0; j < amountKnownSensors; j++) {
    if (array_cmp(addr, ds18b20_sensors[j].address, BYTE_SIZE_ADDRESS) == true) {      
      * index = j;
      return ds18b20_sensors[j].name;
    }
    
  }
  if (!found) {
    * index = -1;
    return "unknown";
  }
}

/* Helper function:
 *  Compare content of two arrays with the length 'len'
 */
boolean array_cmp(byte *a, byte *b, int len) {
  for (uint8_t i = 0; i < len; i++) {
    if (a[i] != b[i]) return false;
  }
  return true;
}


// ************* Display 
// **
// **

//Layout:
#define COL1 2
#define COL2 54
#define COL3 94
#define ROW1 9
#define ROW2 19
#define ROW3 36
#define ROW4 49
#define ROW5 62

void updateDisplay(){
u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_6x10_tf  );
    printTemperature (COL1, ROW1, temperatures[0].current, 1);
    printTemperature (COL2, ROW1, temperatures[0].min);
    printTemperature (COL3, ROW1, temperatures[0].max);
   
    printTemperature (COL1, ROW2, temperatures[1].current,1);
    printTemperature (COL2, ROW2, temperatures[1].min);
    printTemperature (COL3, ROW2, temperatures[1].max);

    u8g2.setFont(u8g2_font_t0_15b_tf  );
    printTemperature (COL1, ROW3, temperatures[2].current);
    printTemperature (COL1, ROW4, temperatures[3].current);
    printTemperature (COL1, ROW5, temperatures[4].current);
     
    u8g2.setFont(u8g2_font_6x10_tf  );
    printTemperature (COL2, ROW3, temperatures[2].min);
    printTemperature (COL3, ROW3, temperatures[2].max);
    printTemperature (COL2, ROW4, temperatures[3].min);
    printTemperature (COL3, ROW4, temperatures[3].max);
    printTemperature (COL2, ROW5, temperatures[4].min);
    printTemperature (COL3, ROW5, temperatures[4].max);
          
    u8g2.drawFrame(50, 0, 90, 64);
    u8g2.drawFrame(90, 0, 117, 64);
        
  } while ( u8g2.nextPage() );

}

void printTemperature (int x, int y, float temp){
  printTemperature (x, y, temp, 0);
}
void printTemperature (int x, int y, float temp, int precision ){
 
   if (temp > -100 && temp < 100){
      x+=8;
   }   
   if (temp > -10 && temp < 10){
      x+=8;
   }
   u8g2.setCursor (x, y);
   if (temp > -120 && temp < 1000){
      u8g2.print(temp, precision);
      u8g2.print(char(176));
      u8g2.print("C");   
   } else {
      u8g2.print("n/a");   
   }

}
