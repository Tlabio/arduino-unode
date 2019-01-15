/*******************************************************************************
   Copyright (c) 2018 Ioannis Charalampidis - TLab.gr

   This is a private, preview release of the uNode hardware abstraction library.
   The holder of a copy of this software and associated documentation files
   (the "Software") is allowed to use the Software without any obligation to
   create private and/or commercial projects. The Software can be obtained
   through the official channels of the author, including but not limited to
   Github and the official TLab.gr website. It is FORBIDDEN however to modify,
   reverse-engineer, publish, distribute, sublicense, and/or sell copies of the
   Software itself.

   The license for this file might change in a future release. The author is not
   obliged to announce this change through any channel but it should be included
   in the release notes.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
   FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
   COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
   IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 *******************************************************************************/

/******************************************************************************
   This scetch will verify GPIO operation:
    - LoRa is switched off
    - Under voltage protection is switched on
    - Async port initialised to 115200 b/s with INFO logging on

    The board set up should be:
       Generic ESP8266 module
       Flash Mode = DIO
       Flash Size = Select a 4 MB option.

    This Test/Demo is for uNode noards with the MCP23S08 I/O expander chip fitted.
    The GPIO pins have the logical names D2 to D9 and can be used with:
       uNode.pinMode( pin, mode );  Mode can be set to INPUT, INPUT_PULLUP and OUTPUT
       uNode.digitalRead( pin );
       uNode.digitalWrite( pin );
    Theoretically you can also control the ESP8266 I/O pins D0 and D1 with these functions, but remember
    that D0 is the same as UPIN_GPIO, which drives the chip select of the MCP23S08 GPIO and D1 is UPIN_CS_RF
    which drives the chip select of the LoRa radio. So in practice: don't touch them...

    This Test/Demo verifies correct operation of the MCP23S08. Pin 10 on the chip is D2, pin 11 is D3, ... and pin 17 is D9.
    The Test needs pins D2(=10) connected to D3(=11), D4 TO D5, D6 to D7 and D8 to D9.

    Contributed 13/01/2019, Gijs Mos, Sensemakers Amsterdam, www.sensemakersams.org
*/
#include <uNodeOpen.hpp>
#include <SPI.h>

/**
   We are using the ADC to measure the battery voltage. If you are using the ADC
   in your project, comment-out the following line.
*/
ADC_MODE(ADC_VCC);

/**
   uNode library configuration
*/
uNodeConfig unode_config = {
  .lora = {
    .mode = LORA_DISABLED           // Disable LoRa. Consult TTN examples to switch LoRa on.
  },
  .logging = LOG_DEFAULT            // Or use LOG_DISABLED or LOG_INFO
};

const uint8_t evenPins[] = { D2, D4, D6, D8 };
const uint8_t oddPins[] = { D3, D5, D7, D9 };
const uint8_t nPins = 4;

// Forward declarations
unsigned functionalTest( const uint8_t a[], const uint8_t b[] );
unsigned stressTest( const uint8_t a[], const uint8_t b[] );

/**
   Sketch setup
*/
void setup() {
  unsigned functionalErrors, stressErrors;

  uNode.setup();                    // Initialize the uNode library
  Serial.println("GPIO (MCP23S08) test. Pin D2 should be connected to D3, D4 to D5, D6 to D7, D8 to D9.");

  functionalErrors = functionalTest( evenPins, oddPins );
  functionalErrors += functionalTest( oddPins, evenPins );
  if (functionalErrors) {
    Serial.print("** ");
    Serial.print( functionalErrors );
    Serial.println( " functional errors found." );
  } else
    Serial.println( "Functional tests passed." );

  stressErrors = stressTest( evenPins, oddPins );
  stressErrors += stressTest( oddPins, evenPins );
  if (stressErrors) {
    Serial.print("** ");
    Serial.print( stressErrors );
    Serial.println( " stress errors found." );
  } else
    Serial.println( "Stress tests passed." );

  Serial.println("\nReset uNode for another pass...");
}


/**
   Sketch loop
*/
void loop() {
  uNode.step();
}

/*
   Functional test suite
*/
unsigned functionalTest( const uint8_t a[], const uint8_t b[] ) {
  // a[] pins to INPUT_PULLUP. This should pull themselfs and the connected b[] pin high.
  unsigned errors = 0;

  for ( int i = 0; i < nPins; i++ ) {

    // ------
    // b[] pins pull the a[] pins LOW through exteral wires.
    uNode.pinMode( a[i], INPUT );
    uNode.pinMode( b[i], OUTPUT );
    uNode.digitalWrite( b[i], LOW );
    delay(1);  // After stabilisation both should be LOW
    if ( uNode.digitalRead( a[i] ) != LOW ) {
      Serial.print(a[i]); Serial.println( " expected to be LOW (E001)." );
      errors++;
    }
    if ( uNode.digitalRead( b[i] ) != LOW ) {
      Serial.print(b[i]); Serial.println( " expected to be LOW (E002)." );
      errors++;
    }

    // ------
    // b[] pins pull the a[] pins HIGH through exteral wires using built in pull_ups.
    uNode.pinMode( b[i], INPUT_PULLUP );   // Now the pullup on b[] should pull both up
    delay(1);
    if ( uNode.digitalRead( a[i] ) != HIGH ) {
      Serial.print( a[i] );
      Serial.println( " expected to be HIGH (E003)." );
      errors++;
    }
    if ( uNode.digitalRead( b[i] ) != HIGH ) {
      Serial.print( b[i] );
      Serial.println( " expected to be HIGH (E004)." );
      errors++;
    }

    // ------
    // b[] pins pull the a[] pins HIGH through exteral wires using built in pull_ups.
    // but they will be pulled down through the a[] pins
    uNode.pinMode( a[i], OUTPUT );   // Now the pullup on b[] should pull both up
    uNode.digitalWrite( a[i], LOW );
    delay(1);
    if ( uNode.digitalRead( a[i] ) != LOW ) {
      Serial.print( a[i] );
      Serial.println( " expected to be HIGH (E005)." );
      errors++;
    }
    if ( uNode.digitalRead( b[i] ) != LOW ) {
      Serial.print( b[i] ); Serial.println( " expected to be HIGH (E006)." );
      errors++;
    }


    // ------
    // b[] pins normal inputs agan and pulled HIGH by the a[] pins.

    uNode.pinMode( b[i], INPUT );
    uNode.pinMode( a[i], OUTPUT );   // Now the pullup on b[] should pull both up
    uNode.digitalWrite( a[i], HIGH );
    delay(1);
    if ( uNode.digitalRead( a[i] ) != HIGH ) {
      Serial.print( a[i] );
      Serial.println( " expected to be HIGH (E007)." );
      errors++;
    }
    if ( uNode.digitalRead( b[i] ) != HIGH ) {
      Serial.print( b[i] );
      Serial.println( " expected to be HIGH (E008)." );
      errors++;
    }
  }
  return errors;
}

/*
   Stress test suite
*/
unsigned stressTest( const uint8_t a[], const uint8_t b[] ) {

  unsigned errors = 0;

  for ( int i = 0; i < nPins; i++ ) {
    // Start as a[] pins as INPUT and b[] as OUTPUT LOW.
    uNode.pinMode( a[i], INPUT );
    uNode.pinMode( b[i], OUTPUT );
    uNode.digitalWrite( b[i], LOW );
  }

  uint8_t newState;

  for ( int loops = 0; loops < 10000; loops++ ) {
    if ( !(loops % 1000) ) yield();   // Keep WDT happy.
    for ( int i = 0; i < nPins; i++ ) {
      newState = ( uNode.digitalRead( b[i] ) == LOW ? HIGH : LOW );
      uNode.digitalWrite( b[i], newState );
      if ( uNode.digitalRead( b[i] ) != newState ) {
        errors++;
        if ( errors < 20 ) {
          Serial.print(" Loop ");
          Serial.print(loops);
          Serial.print(", Output ");
          Serial.print(b[i]);
          Serial.print(", expected ");
          Serial.print(newState);
          Serial.println(" (E010)");
        }
        while (1) yield();
      }
      if ( uNode.digitalRead( a[i] ) != newState ) {
        errors++;
        if ( errors < 20 ) {
          Serial.print(" Loop ");
          Serial.print(loops);
          Serial.print(", Input ");
          Serial.print(a[i]);
          Serial.print(", expected ");
          Serial.print(newState);
          Serial.println(" (E011)");
        }
      }
    }
  }

  return errors;
}
