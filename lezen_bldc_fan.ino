/*  LEZEN BLDC FAN remote control by sendust   2022/7/14
 *  
 *  Build with IRRemote library 3.7.1
 *  Skylife Remote Cursor Key
 *  
 *  Protocol == 7
 *  address == 165
 *  UP == 26
 *  DOwn == 27
 *  Left == 29
 *  Right == 28
 *  
 *  2022/7/14 For accurate IrSender timing, disable irReceiver before send !!!! (IrReceiver.stop())
 *            Flush serial print
 *  To do : Instead of raw IRData, Try raw IRData tick (uint16_t -> uint8_t)
 *  
 */
#include <Arduino.h>

/*
 * Define macros for input and output pin etc.
 */

#define MARK_EXCESS_MICROS    20 // recommended for the cheap VS1838 modules
#include "PinDefinitionsAndMore.h" //Define macros for input and output pin etc.
#include <IRremote.hpp>

enum windpia
{
  ONOFF,
  SPEED,
  ROTATE,
  TIMER,
  MODE
};

const unsigned char irsignal[5][67] = {
  {91, 48, 4, 8, 4, 8, 4, 8, 4, 8, 4, 8, 8, 4, 8, 4, 8, 4, 4, 8, 8, 4, 4, 8, 8, 4, 8, 4, 8, 4, 8, 4, 8, 4, 4, 8, 4, 8, 4, 8, 4, 8, 4, 8, 4, 8, 8, 4, 4, 8, 8, 4, 8, 4, 8, 4, 8, 4, 8, 4, 8, 4, 4, 8, 8, 4, 12},
  {91, 48, 4, 8, 4, 8, 4, 8, 4, 8, 4, 8, 8, 4, 8, 4, 8, 4, 4, 8, 8, 4, 4, 8, 8, 4, 8, 4, 8, 4, 8, 4, 8, 4, 4, 8, 4, 8, 4, 8, 4, 8, 4, 8, 8, 4, 4, 8, 4, 8, 8, 4, 8, 4, 8, 4, 8, 4, 8, 4, 4, 8, 8, 4, 8, 4, 12},
  {91, 48, 4, 8, 4, 8, 4, 8, 4, 8, 4, 8, 8, 4, 8, 4, 8, 4, 4, 8, 8, 4, 4, 8, 8, 4, 8, 4, 8, 4, 8, 4, 8, 4, 4, 8, 4, 8, 4, 8, 4, 8, 8, 4, 4, 8, 4, 8, 4, 8, 8, 4, 8, 4, 8, 4, 8, 4, 4, 8, 8, 4, 8, 4, 8, 4, 12},
  {91, 48, 4, 8, 4, 8, 4, 8, 4, 8, 4, 8, 8, 4, 8, 4, 8, 4, 4, 8, 8, 4, 4, 8, 8, 4, 8, 4, 8, 4, 8, 4, 8, 4, 4, 8, 8, 4, 4, 8, 8, 4, 8, 4, 8, 4, 8, 4, 8, 4, 8, 4, 4, 8, 8, 4, 4, 8, 4, 8, 4, 8, 4, 8, 4, 8, 12},
  {91, 48, 4, 8, 4, 8, 4, 8, 4, 8, 4, 8, 8, 4, 8, 4, 8, 4, 4, 8, 8, 4, 4, 8, 8, 4, 8, 4, 8, 4, 8, 4, 8, 4, 4, 8, 4, 8, 4, 8, 4, 8, 8, 4, 8, 4, 4, 8, 4, 8, 8, 4, 8, 4, 8, 4, 8, 4, 4, 8, 4, 8, 8, 4, 8, 4, 12}   
};


//const uint8_t NEC_KHZ = 38; // 38kHz carrier frequency for the NEC protocol  // Remove this line (Version 3.7.1)


void setup() {
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(115200);
#if defined(__AVR_ATmega32U4__) || defined(SERIAL_USB) || defined(SERIAL_PORT_USBVIRTUAL)  || defined(ARDUINO_attiny3217)
    delay(4000); // To be able to connect Serial monitor after reset or power up and before first print out. Do not wait for an attached Serial Monitor!
#endif
    // Just to know which program is running on my Arduino
    Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));

    IrSender.begin(IR_SEND_PIN, ENABLE_LED_FEEDBACK); // Specify send pin and enable feedback LED at default feedback LED pin

    Serial.print(F("Ready to send IR signals at pin "));
#if defined(ARDUINO_ARCH_STM32) || defined(ESP8266)
    Serial.println(IR_SEND_PIN_STRING);
#else
    Serial.print(IR_SEND_PIN);
#endif
    Serial.println();
    
// In case the interrupt driver crashes on setup, give a clue
// to the user what's going on.
    Serial.println(F("Enabling IRin..."));
    
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

    Serial.print(F("Ready to receive IR signals at pin "));
    
#if defined(ARDUINO_ARCH_STM32) || defined(ESP8266)
    Serial.println(IR_RECEIVE_PIN_STRING);
#else
    Serial.println(IR_RECEIVE_PIN);
#endif
  
    // infos for receive
    Serial.print(RECORD_GAP_MICROS);
    Serial.println(F(" us is the (minimum) gap, after which the start of a new IR packet is assumed"));
    Serial.print(MARK_EXCESS_MICROS);
    Serial.println(F(" us are subtracted from all marks and added to all spaces for decoding"));
      
}

void loop() {
  // put your main code here, to run repeatedly:
  uint16_t irbuffer[67] = {0};        // Actual buffer for ir send

    if (IrReceiver.decode())
    {
  
    IrReceiver.printIRResultShort(&Serial);
    Serial.println();
    Serial.print("Signal Detected   with protocol ");
    Serial.println(IrReceiver.decodedIRData.protocol);
    Serial.print("address is ");
    Serial.println(IrReceiver.decodedIRData.address);
    Serial.print("command is ");
    Serial.println(IrReceiver.decodedIRData.command);

    IrReceiver.stop();        // Disable interrupt before irsend.. Important !!!!
    
    delay(70);        // Guard band time between two ir remote signal
    if (IrReceiver.decodedIRData.protocol == 8)
     {
      if (IrReceiver.decodedIRData.address == 165) 
      {
          if (IrReceiver.decodedIRData.command == 26)   // button up
          {
            for(unsigned char i=0;i<67;i++)
            {
              irbuffer[i] = irsignal[SPEED][i] * 100;
            }
            Serial.println("Send ir signal FAN SPEED");
            Serial.flush();
            IrSender.sendRaw(irbuffer, sizeof(irbuffer) / sizeof(irbuffer[0]), NEC_KHZ);
          } else if (IrReceiver.decodedIRData.command == 27)   // button down
          {
            for(unsigned char i=0;i<67;i++)
            {
              irbuffer[i] = irsignal[ONOFF][i] * 100;
            }
            Serial.println("Send ir signal FAN ON/OFF");
            Serial.flush();
            IrSender.sendRaw(irbuffer, sizeof(irbuffer) / sizeof(irbuffer[0]), NEC_KHZ);
          } else if ((IrReceiver.decodedIRData.command == 28) || (IrReceiver.decodedIRData.command == 29))  // button left, right
          {
            for(unsigned char i=0;i<67;i++)
            {
              irbuffer[i] = irsignal[ROTATE][i] * 100;
            }
            Serial.println("Send ir signal FAN rotate");
            Serial.flush();
            IrSender.sendRaw(irbuffer, sizeof(irbuffer) / sizeof(irbuffer[0]), NEC_KHZ);
          }
      }
     }
    IrReceiver.start();
  }

}
