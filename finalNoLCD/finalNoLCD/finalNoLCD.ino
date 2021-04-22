#include <LiquidCrystal.h> //for the screen
#include <FPS_GT511C3.h> //the fps (fingerprint scanner) library
#include <SoftwareSerial.h> //used by fps library




//Setting up the pins for the LCD and the fps
SoftwareSerial lcd(2, 8); //pinouts for LCD
FPS_GT511C3 fps(10, 11); //RX, TX

boolean isFinger = false; //true if the fps detects a finger on the scanner

//output pins
const int buzzerPin = 13;
const int doorOpener = 6;
const String idNames[] = 
{
  "Basit", "Basit", "Hafsah", "Hafsah", "Hafsah", "Mom", "Mom", "Ami G", "Ami G"};


void setup(){
  //set outputs
  pinMode(buzzerPin, OUTPUT);

  pinMode(doorOpener, OUTPUT);
  analogWrite(doorOpener, 0);
  
  //for debugging
  Serial.begin(300);
  fps.UseSerialDebug = false; //set to true for fps debugging through serial
  
  //initializing the libraries
  Serial.println("Hello!");
  delay(1000);

  fps.Open();
  fps.SetLED(true); //the fps LED

  /*
  fps.DeleteID(1);
  fps.DeleteID(2);
  fps.DeleteID(3);
  fps.DeleteID(4);
  fps.DeleteID(5);
  fps.DeleteID(6);
  fps.DeleteID(7);
  fps.DeleteID(8);
  //*/
  
  //boot up sound

  for(int i=0; i<30; i++){
    tone(buzzerPin, 50+10*i, 30);
    delay(30);
  }
  tone(buzzerPin, 350);

  //print starting message

  Serial.println("Put your finger "); //the command to print to the LCD

  Serial.println(" on the scanner ");
  delay(150);
  
  noTone(buzzerPin); //stops the startup sound
  
}

void loop(){

  //scan and identify the finger when one is put on it
  
  waitForFinger();

  fps.CaptureFinger(false); //captures the finger for identification
  int id = fps.Identify1_N(); //identifies print and stores the id

  if(id <= 10){
    Serial.println("Access granted "); //success message

    //prints name when the garage is opening 
    String message = "Hey " + idNames[id] + "!";
    Serial.println(message);

    tone(buzzerPin, 262, 1000);
    delay(1500);

    //sends a signal to open the garage door

    analogWrite(doorOpener, 169);
    delay(1000);
    analogWrite(doorOpener, 0);
    
    for(int i = 0; i<5; i++)
    {
      digitalWrite(2, HIGH);
      delay(250);
      digitalWrite(2, LOW);
      delay(250);
    }

    Serial.println("Don't forget to ");

    Serial.println("shut me off!");
    delay(2000);

    waitForFinger(); //tap to continue to enroll

    while(true){ //save a new fingerprint
      //prints message to lcd

      Serial.println("So you want to");

      Serial.println("scan a new one?");
      delay(2000);

      //Copied and slightly modified from the enroll example:
      int enrollid = 11;

      //choosing which id to overwrite/create
      //release your finger when you want to write to the id/name printed on the screen

      waitForFinger(); //waits for the fps to be pressed

      while(enrollid==11){
        for (int i = 1; i<=10; i++){
          if((fps.IsPressFinger() == true)){
            
            String str = "ID " + String(i) + ": " + idNames[i]; //concats a string w/the id
            Serial.println(str);
            delay(2000);
          }
          else if(i>1){
            Serial.println(i);
            enrollid = i-1;
            break;
          }
        }
      }

      //warning if there is already data in this id slot
      if(fps.CheckEnrolled(enrollid)){ 

        Serial.println("Warning! ID #");
        Serial.println(enrollid);

        Serial.println("has data. OK?");
        delay(2500);

        waitForFinger(); //waits for the fps to be pressed

        fps.DeleteID(enrollid); //delete data
        delay(100);
      }


      //Enroll
      fps.EnrollStart(enrollid);

      Serial.println("Place finger to ");

      Serial.println("enroll #");
      Serial.println(enrollid); //prints id that is being enrolled
      waitForFinger(); //waits for the fps to be pressed

      //captures the finger and saves to memory three times for accurate data
      bool bret = fps.CaptureFinger(true); //high quality pic for enrollment
      int iret = 0; //error stuff

      if (bret != false){ //first enroll

        Serial.println("Remove finger");
        fps.Enroll1();
        while(fps.IsPressFinger() == true) delay(100); //waits until no finger

        Serial.println("Press again");
        waitForFinger(); //waits for the fps to be pressed
        bret = fps.CaptureFinger(true);

        if (bret != false){ //second enroll

          Serial.println("Remove finger  ");
          fps.Enroll2();
          while(fps.IsPressFinger() == true) delay(100);

          Serial.println("Press yet again ");
          waitForFinger(); 
          bret = fps.CaptureFinger(true);

          if (bret != false){ //third enroll
            iret = fps.Enroll3();
            if (iret == 0){ //checks to see if there are any errors

              Serial.println("Success!");
              delay(2000);
              beep(); //shuts arduino off
            }
            else{ //if the enrollment fails in any way

              Serial.println("Fail. Try again ");
              delay(1000);
            }
          }
          
          Serial.println("Failed 3rd"); //error on 3rd
          delay(1000);
        }
        
        Serial.println("Failed 2nd"); //error on 2nd
        delay(1000);
      }
      
      Serial.println("Failed 1st"); //error on 1st
      delay(1000);
    }
  }

  else{
    Serial.println("Fingerprint is"); //if print isn't recognized
    
    Serial.println("unverified");
    delay(2000);
    
    Serial.println("Please try again");
    
    Serial.println("Use your pointer"); //I scanned everyone's pointer finger
    delay(500);
  }
  delay(250);
}


void beep(){ 
  //beeps in hopes of someone closing the case
  
  Serial.println("Please close the");
  
  Serial.println("case!");
  for(int i=0;i<8;i++){
    tone(buzzerPin, 262, 500);
    delay(1000);
  }
  delay(5000); //wait for someone to close the case

  //if no one does, shut everything off
  
  fps.SetLED(LOW);
  while(true) delay(10000);
}

void waitForFinger(){
  static int timer; //contains timeout counter
  timer = 0; //resets the timer everytime this function starts
  while(!fps.IsPressFinger()){ //timeout of eight seconds
    timer++;
    delay(100); 
    if (timer>=80 && !fps.IsPressFinger()){
      beep();
    }
  } 
  timer = 0; //resets the timer everytime this function ends
}
