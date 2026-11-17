// Read color sensor and identify ball color
// TODO: Output binary representation of which colors detected

#include <Arduino.h>
#include <Adafruit_AS7341.h>

Adafruit_AS7341 as7341;
uint16_t readings[12];
uint8_t errors = 0;
uint8_t identified_balls = 0;

#define PRINT_DEBUG false // Define as true to print all the readings
#define WIRE Wire1
#define COUNT_VARIANCE_PERCENTAGE 0.2 // If count varies less than this percent, same color
uint16_t yellow_average[12] =  {6400, 14000, 18000, 51000, 65500, 20000, 65500, 65500, 65500, 65500, 65500, 20000};
uint16_t blue_average[12] = {2800, 43600, 47700, 28700, 65500, 4700, 18100, 14500, 15400, 9800, 65500, 4700};
uint16_t green_average[12] = {2850, 12000, 21000, 54000, 65500, 7800, 50000, 31000, 28000, 19200, 65500, 7800};
uint16_t orange_average[12] = {5000, 13500, 15800, 19000, 65500, 16000, 42000, 65500, 65500, 65500, 65500, 16100};
uint16_t aqua_average[12] = {2800, 26000, 29000, 28100, 65500, 5300, 28880, 24000, 24200, 14500, 65500, 5300};
uint16_t mgreen_average[12] = {6880, 37000, 50400, 65500, 65500, 15880, 65500, 65500, 63300, 46860, 65500, 15290};
uint16_t silver_average[12] = {5230, 43690, 46140, 52800, 65500, 10000, 54840, 65400, 59000, 38680, 65500, 10010};

char* color_names[7] = {"Yellow", "Green", "Orange", "Blue", "Aqua Blue", "Macaroon Green", "Silver"};
uint16_t* color_averages[7] = {yellow_average, green_average, orange_average, blue_average, aqua_average, mgreen_average, silver_average};
uint8_t output_pins[7] = {A0, A0, A0, A0, A1, A2, A3};


bool isColor(uint16_t* averages) {
  // as7341.readAllChannels(readings);
  errors = 0;
  for (int i=0; i<12; i++) {
    if (PRINT_DEBUG) {
      Serial.print(readings[i]); // TODO: if debug
      Serial.print(", ");
      Serial.print(averages[i]);
    }
    if (!(abs(readings[i] - averages[i]) < int(averages[i] * COUNT_VARIANCE_PERCENTAGE))) {
      errors++;
      if (PRINT_DEBUG) {
        Serial.print("   NOT");
      }
    }
    if (PRINT_DEBUG) {
      Serial.println();
    }
  }
  if (errors > 0) {
    return false;
  }
  return true;
}

void printReadings() {  
  // Print out the stored values for each channel
  Serial.print("F1 415nm Violet: ");
  Serial.println(readings[0]);
  Serial.print("F2 445nm Indigo: ");
  Serial.println(readings[1]);
  Serial.print("F3 480nm   Blue: ");
  Serial.println(readings[2]);
  Serial.print("F4 515nm   Cyan: ");
  Serial.println(readings[3]);
  Serial.println(readings[4]);
  Serial.println(readings[5]);
  Serial.print("F5 555nm  Green: ");
  Serial.println(readings[6]);
  Serial.print("F6 590nm Yellow: ");
  Serial.println(readings[7]);
  Serial.print("F7 630nm Orange: ");
  Serial.println(readings[8]);
  Serial.print("F8 680nm    Red: ");
  Serial.println(readings[9]);
  Serial.print("          Clear: ");
  Serial.println(readings[10]);
  Serial.print("        Near IR: ");
  Serial.println(readings[11]);
  Serial.println("");
}

void setup() {
  Serial.begin(115200);

  // Wait for communication with the host computer serial monitor
  // while (!Serial) {
  //   delay(1);
  // }
  
  for(int i=0; i<7; i++) {
    pinMode(output_pins[i], OUTPUT);
    digitalWrite(output_pins[i], LOW);
  }

  if (!as7341.begin(0x39, &Wire1)){  // Had to add address and QT Py S2 second TwoWire interface
    Serial.println("Could not find AS7341");
    while (1) { delay(10); }
  }
  as7341.setLEDCurrent(4); // 4mA
  as7341.setATIME(200); // was 100
  as7341.setASTEP(999);
  as7341.setGain(AS7341_GAIN_256X);
  Serial.println("Setup Complete.......................");
}

void loop() {
  // Read all channels at the same time and store in as7341 object
  as7341.enableLED(true);
  // delay(50);
  if (!as7341.readAllChannels(readings)){
    Serial.println("Error reading all channels!");
    return;
  }
  as7341.enableLED(false);

  identified_balls = 0;
  for(int i=0; i<7; i++) {
    Serial.print("Checking color: ");
    Serial.println(color_names[i]);
    if (isColor(color_averages[i])) {
      Serial.print("Color identified as ");
      Serial.print(color_names[i]); // 
      Serial.print(" with errors: ");
      Serial.println(errors);
      digitalWrite(output_pins[i], HIGH);
      identified_balls++;
    } else {
      digitalWrite(output_pins[i], LOW);
    }
  }
  Serial.print(identified_balls);
  Serial.println(" balls identified.");

  printReadings();

  delay(3000);
}