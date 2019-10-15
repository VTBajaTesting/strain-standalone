/**
 * This program is responsible for reading in data from the analog inputs
 * and printing that data to an onboard sd card. 
 * NOTE: This program blinks out errors through an error led pin. The errors 
 * as follows in terms of blinks:
 * 
 * 1 blink   - Failed to initialize the SD card.
 * 2 blinks  - Failed to open file.
 * @author Brennan Hurst
 * @Version 15.10.2019
 */
#include <SD.h>
#include <SPI.h>

File file; 

#define BUFFER_SIZE 1000 //The size of the buffer. (MAX size prior to dump, be careful may take up lots of memory).
#define CARD_SELECT 4 //The pin used to communicate with the card
#define ERROR_LED 13 //The LED which corrosponds to the led we wish to blink

String fileName = "testFile.txt"; //the name of the file we wish to save to
int dumpCount = 0;//the current dumpcount
short dumpBuffer[BUFFER_SIZE]; //the buffer to be dumped

/**
 * The setup() method is called first at runtime and is responsible
 * for initializing the SD card.
  */
void setup() {
  //set pinMode
  pinMode(A0, INPUT);
  pinMode(ERROR_LED, OUTPUT); //the error pin
  //Begin Serial
  Serial.begin(2000000);
  while (!Serial);

  Serial.println("Beginning SD card Init. . .");

  //initialize the sd card
  if (!SD.begin(CARD_SELECT))
  {
    Serial.println("Init failed!");
    error(1);
  }
  else
  {
    Serial.println("Init completed successfully");
  }
}

/**
 * The loop() method loops while the program is running. 
 * This implementation of the loop() metod writes data from the 
 * strain guage to a buffer and dumps that data to a file when required.
 */
void loop() {
  // put your main code here, to run repeatedly:
  if (dumpCount == BUFFER_SIZE)
  {
    writeFile(); //dump the data if the dump count is equal to the buffer size
  }
  else
  {
    writeToBuffer(analogRead(A0)); //write to the buffer the read signal
  }
  
}

/**
 * This method is responsible for writing the strain data 
 * to the data buffer. 
 */
void writeToBuffer(short strainMeasured)
{
  dumpBuffer[dumpCount] = strainMeasured;
  dumpCount++;
}

/**
 * This method is responsible for dumping the data in the buffer
 * to the file and resetting the dumpCount to 0. 
 */
void writeFile()
{
   //open the file
    file = SD.open("testFile.txt", FILE_WRITE);
    //check to see if file opened correctly
    if (file)
    {
      Serial.println("beginning data dump into file");
      for (int i = 0; i < BUFFER_SIZE; i++)
      {
        //write each row to the file
        file.println(dumpBuffer[i]);
      }
      //reset dumpCount to zero
      dumpCount = 0;
      file.close();
      Serial.println("Data dump complete");
    }
    else
    {
      Serial.println("Failed to open the file");
      error(2);
    }
}
/**
 * This method is responsible for holding the program 
 * hostage if an error occurs and blinking out an error 
 * code to the associated ERROR_LED pin.
 */
void error(byte errorNum)
{
  while (1) {
    byte i;
    for (i = 0; i < errorNum; i++)
    {
      digitalWrite(ERROR_LED, HIGH);
      delay(100);
      digitalWrite(ERROR_LED, LOW);
      delay(100);
    }

    delay(1000);
  }
  
}
