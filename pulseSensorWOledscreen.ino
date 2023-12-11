//Library
#include <U8glib.h> //oled screen library
#include <Wire.h>
#define USE_ARDUINO_INTERRUPTS true // Needed for pulse to be read correctly
#include <PulseSensorPlayground.h>
PulseSensorPlayground pulseSensor; // PulseSensor Playground functions

U8GLIB_SSD1306_128X64 oled(U8G_I2C_OPT_NO_ACK); //tells wich screen is in use

//  Variables

int counter;     // used to slow the updating of the oledscreen since it takes to long and interferes with the pulsesensors intervals

const int PULSE_INPUT = A0;        // Pulse Sensor PURPLE WIRE connected to ANALOG PIN 0

const int THRESHOLD = 600;       // Determine which Signal to "count as a beat", and which to ingore.

const int OUTPUT_TYPE = SERIAL_PLOTTER; // used to communicate with serial plotter

const int BUZZER = 9; //buzzer pin


void setup() {
  Serial.begin(115200);         // Set's up Serial Communication at certain speed.

//Pulseplayground managing to make using the variables easier to use
  pulseSensor.analogInput(PULSE_INPUT); 

  pulseSensor.setSerial(Serial);

  pulseSensor.setThreshold(THRESHOLD);
  pulseSensor.setOutputType(OUTPUT_TYPE);

  pinMode(BUZZER, OUTPUT); // buzzer pin sends output signals

  pulseSensor.begin(); //starts the pulsesensor reading 
  
  Wire.begin();
  oled.setFont(u8g_font_helvB10); //font for the oled display

}

// The Main Loop Function
void loop() {
  delay(10);

  pulseSensor.outputSample(); // pulsesenor value sent to serialmonitor

  // BPM value sent serialmonitor
  if (pulseSensor.sawStartOfBeat()) {
    pulseSensor.outputBeat();

  }

  //Playing sounds if the pulsesensor value is above the threshold
  if (pulseSensor.getLatestSample() > THRESHOLD) {

    // calls for piezo sound pitch and plays it
    tone(BUZZER , piezoTone(pulseSensor.getLatestSample())); 

  }

  else {
    noTone(BUZZER);
  }


  //if function used to make it run only once a second because pulsesensor needs precise amount of streaming info to run properly
  
  if (counter == 50) {

    updateOled(String(pulseSensor.getBeatsPerMinute())); //calls pulsesensor function to get BPM and then calls for the function displaying it's value on the oled screen
    counter = 0; // reseting counter
    
  }
  counter = counter + 1; //a 20ms delay is used, making this count to 50 resulting in 1 run per second
    
}

// functions outside of library

//maping the sesnor values to fit the piezo values 
int piezoTone(int x) {
  int piezoValue;
  piezoValue = map(x, 450 , 650 , 300 , 700);
  return piezoValue;
}


  //updating the information that's sent to the oled screen
void updateOled(String text) {
  oled.firstPage();
  do {
    oled.drawStr(0,11, text.c_str());
   }
   while(oled.nextPage()); 
}
