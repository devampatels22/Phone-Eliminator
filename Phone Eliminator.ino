/*
Course: COMP 444
Name: Devam Patel
Student Number: 360395
Final Project
Description: I have created a phone eliminator that helps users to focus for 25 minutes phone free with 5 minutes break.

*/
//include the servo library
#include <Servo.h>       
// screen display librarey
#include <LiquidCrystal.h>  
// count down timer librarey
#include "CountDown.h"

// assigning pin numbers for the lcd display
LiquidCrystal screen(13, 12, 11, 10, 9, 8);
// two different timers for focus  and break.
CountDown focusTimer(CountDown::MINUTES);
CountDown breakTimer(CountDown::MINUTES);

//create a servo object
Servo gateServo;         
// servo default position of 20
int servoPosition = 20; 

// assigning pin number for buzzer
int buzzerPin = 3;   

int servoPin = 6;       // assign servo pin to 6
int buttonRedPin = 4;   // assign red button pin to 4
int buttonGrnPin = 2;   // assign green button pin to 2
int lightRedPin = 7;    // assign red light pin to 7

int focusTime = 25;      // Focus Time
int breakTime = 5;      // Break Time

int counterEmergency = 0; //Emergency button counter

//boolean

bool onBreak = false;     //Break status boolean
bool startTimer = false;  //timer status boolean
bool focusOver = false;   // Focus status boolean

int fmins = focusTime; // Fmins assigned to Focus Time
int fsecs = 0;         // seconds for focus time

int bmins = breakTime; //bmins assigned to breakTime
int bsecs = 0;          // seconds for break time


// Setup is initializing the programs at the start of the robot only once.
void setup() {
  
  screen.begin(16, 2);            //LCD Screen size
  gateServo.attach(servoPin);      // initialize servo motor
 
  pinMode(buttonRedPin, INPUT_PULLUP);  // Set Red button to a certain Pin number
  pinMode(buttonGrnPin, INPUT_PULLUP);  // Set Blue Button to a certain Pin Number.
  pinMode(lightRedPin, OUTPUT); // Set pin 13 to output
  pinMode(buzzerPin, OUTPUT);   //Set PIEZO BUZZER to a certain Pin number.
  unlockBox();                  //Have the box be unlocked at the start
  welcomeMessage();             // Shows the welcome message at start.
  focusInfoMes();               // Focus Timer Message is shown.
}


void loop() {
  // When green button is pressed
  if (digitalRead(buttonGrnPin) == LOW) {
    buttonPressed();    // Givers user feedback of pressed button.
    startTimer = true;  // startTimer boolean is set to true.
    focusOver = false;  // FocusOver Boolean is set to false since focus timer has started=
    fmins = focusTime;  // assigning fmins to focusTime for every iterations after.
    focusTimer.start(0, 0, focusTime, 0); //Focus Timer is start
    badMusic();           //Music is played when the Box is locked
  }

  //When Red button is pressed.
  if (digitalRead(buttonRedPin) == LOW) {
    buttonPressed();    //Give user Feedback of pressed button.
    if (counterEmergency < 1) {
      counterEmergency++;
      focusTimer.stop();  //Stop the focus timer
      turnOffLight();     // Turn off the red light
      emergencyMusic();   // Play music
      unlockBox();        //unlock the box
      
      // Wait until green button is pressed to start back the focus timer
      while (digitalRead(buttonGrnPin) != LOW) {
        emergencyMes(); //Emergency message is displayed
      }
      //PLay sound of emergency
      startTimer = false; //set 
    } else {
      //display message Bail out emergency used.
      noEmergencyLftMes(); //Show no emergency Pause Left.
    }
  }

  //when Focus timer is on.
  if (startTimer == true) {
    lockBox();      //Keep the box locked
    turnOnLight();  //Keep the Light On
    focusCountDown(); //Show the focus timer CountDown

    //When Focus Timer ends
    if (fmins == 0 && fsecs == 0) {
      startTimer = false; 
      focusOver = true;
      focusEndMes(); //display Focus Timer End Message.

      //Until Green Button is pressed show the break timer start Info
      while (digitalRead(buttonGrnPin) != LOW) {
        breakStrtMes();
      }

      
      goodMusic();   //Play the msuic when Break timer starts
      breakMes();    //Show Break timer info
      breakTimer.start(0, 0, breakTime, 0); //start Break Timer
      fmins = focusTime; 
    }
  } else if (startTimer == false && focusOver == true) {
    turnOffLight(); //Turn off the red light 
    unlockBox();    // open the box
    if (bmins == 0 && bsecs == 0) { 
      startTimer = true;  // Start Timer is set to true
      focusOver = false;  // Set focusOver to false
      bmins = breakTime;

      // Wait until Green button is pressed than start Focus Timer
      while (digitalRead(buttonGrnPin) != LOW) {
        focusInfoMes();
      }
      badMusic();
      
    }
    breakCountDown();
  }
}

// Button Pressed Tone
void buttonPressed(){
  tone(buzzerPin, 2093, 150);  //C7
  delay(300);
}

// Turn Servo motor 90 degrees to close position
void lockBox() {
  servoPosition = 100;             //convert the potentiometer number to a servo position from 20-160
  gateServo.write(servoPosition);  //move the servo according to servopositon
}

// turn Servo motor 90 degree to open position
void unlockBox() {
  servoPosition = 20;              //convert the potentiometer number to a servo position from 20-160
  gateServo.write(servoPosition);  //move the servo according to servopositon
}

//Turn the Red light ON 
void turnOnLight() {
  digitalWrite(lightRedPin, HIGH);  // Turn on the LED
}

//Turn Red light OFF
void turnOffLight() {
  digitalWrite(lightRedPin, LOW);  // Turn off the LED
}

//Show focus Timer 
void focusCountDown() {
  screen.clear();
  screen.setCursor(0, 0);
  screen.print("Focus Time");
  screen.setCursor(0, 1);
  screen.print("Remaining: ");

  screen.setCursor(11, 1);
  //calculations for assigning focus mins and secs
  fmins = ((focusTimer.remaining() % 3600) / 60);
  fsecs = (focusTimer.remaining() % 60);

  showTime(fmins, fsecs, 11); //Helper Function to show time
}

// helper function to show focus end message.
void focusEndMes() {
  screen.clear();
  screen.setCursor(0, 0);
  screen.print("Focus Time Over.");
  delay(2000);
}

//helper function to show break start message
void breakStrtMes() {
  screen.clear();
  screen.setCursor(0, 0);
  screen.print("Press Green bttn");
  screen.setCursor(0, 1);
  screen.print("to start Break!");
  delay(400);
}

//helper function to show break timer info message
void breakMes() {
  screen.clear();
  screen.setCursor(0, 0);
  screen.print("Break Time!");
  screen.setCursor(0, 1);
  screen.print("5 mins!");
  delay(2000);
}

//helper function for break count down timer
void breakCountDown() {
  screen.clear();
  screen.setCursor(0, 0);
  screen.print("Break Time");
  screen.setCursor(0, 1);
  screen.print("Remaining: ");
  screen.setCursor(11, 1);
  bmins = ((breakTimer.remaining() % 3600) / 60);
  bsecs = (breakTimer.remaining() % 60);
  showTime(bmins, bsecs, 11);
}

//helper function emergency message 
void emergencyMes() {
  screen.clear();
  screen.setCursor(0, 0);
  screen.print("Emergency Used!");
  screen.setCursor(0, 1);
  delay(2000);
}

//helper function No emergency pause left message 
void noEmergencyLftMes() {
  screen.clear();
  screen.setCursor(0, 0);
  screen.print("No Emergency");
  screen.setCursor(0, 1);
  screen.print("Pause left!");
  delay(2000);
}

////helper function for showing the time on the screen display
void showTime(int min, int sec, int posCursor) {
  screen.print(min);

  if (min < 10) {
    screen.setCursor(posCursor + 1, 1);
    screen.print(":");
    screen.setCursor(posCursor + 2, 1);
    //adds a zero at the start of the secs for formats
    if (sec < 10) {
      screen.print("0");
      screen.setCursor(posCursor + 3, 1);
      screen.print(sec);
    } else {
      screen.print(sec);
    }
  } else {
    screen.setCursor(posCursor + 2, 1);
    screen.print(":");
    screen.setCursor(posCursor + 3, 1);
    if (sec < 10) {
      screen.print("0");
      screen.setCursor(posCursor + 4, 1);
      screen.print(sec);
    } else {
      screen.print(sec);
    }
  }
  delay(1000);
  screen.clear();
}

//helper function for welcome message 
void welcomeMessage() {
  screen.clear();
  screen.setCursor(0, 0);
  screen.print("Welcome To");

  screen.setCursor(0, 1);
  screen.print("Phone Elimnator!");
  delay(2000);
}

//helper function for focus information message 
void focusInfoMes() {
  screen.clear();
  screen.setCursor(0, 0);
  screen.print("Focus Timer");
  screen.setCursor(0, 1);
  screen.print("25 Mins!");
  delay(400);
}

// helper function for good music play
void goodMusic() {
  tone(buzzerPin, 1318, 150); 
  delay(175);
  tone(buzzerPin, 1567, 150); 
  delay(175);
  tone(buzzerPin, 2637, 150); 
  delay(175);
  tone(buzzerPin, 2093, 150); 
  delay(175);
  tone(buzzerPin, 2349, 150); 
  delay(175);
  tone(buzzerPin, 3135, 500); 
}

// helper function for bad music play
void badMusic() {
  tone(buzzerPin, 130, 250);  
  delay(275);
  tone(buzzerPin, 73, 250);  
  delay(275);
  tone(buzzerPin, 65, 150);  
  delay(175);
  tone(buzzerPin, 98, 500);  
}

// helper function for emergency music play
void emergencyMusic(){
  tone(buzzerPin, 98, 500);  
  delay(175);
  tone(buzzerPin, 2349, 500);  
  delay(175);
  tone(buzzerPin, 98, 500);  
  delay(175);
  tone(buzzerPin, 2349, 500); 
}