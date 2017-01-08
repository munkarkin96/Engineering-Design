#include "Wire.h"
#include "CMPS03.h"

CMPS03 cmps03;

int leftMotor = 5; //motor pins
int rightMotor = 6;
int leftMotorValue;
int rightMotorValue;
int a, b; //binary data of tiltSensor();
int buzzer = 8; 

int bearingValue; 

int doorInput1 =13; //driver pins
int doorInput2 = 12; 
int doorPWM = 10; //motor driver execution pin

int posLDR = A9; 
int posLaser = 15;
int limitSwitch = 14;
int switchValue; //value of limit switch

int valueLDR; 
int secondValueLDR; 

int laserStatus=0;

int doorCount; 

int motorStatus = 0; // should laser system be activated ----> 1 to activate laser system 

char speedInput; //selection of speed 

int valueRead; 
int tempValueRead;


double initialBearing; 
double faultBearing; 
//  double differenceAngle;
int speedMotor;



void setup()
{
  pinMode (buzzer, OUTPUT);
  pinMode (doorInput1, OUTPUT);
  pinMode (doorInput2, OUTPUT);
  pinMode (posLDR, INPUT);
  pinMode (limitSwitch, INPUT);
  pinMode (posLaser, OUTPUT);
  // pinMode (leftMotor, OUTPUT);
  //pinMode (rightMotor, OUTPUT);
  Serial.begin(9600);
  Wire.begin(9600);

}


void loop (){ 
  double errorValue; 
  
  Serial.print("\nCurrent bearing: ");
  Serial.println (cmps03.read()/10);
  Serial.print ("\nValue of limit switch: ");
  Serial.println(digitalRead(limitSwitch));

  if (Serial.available () >=0){

    //tiltSensor();  // I might remove tilt sensor all together if the pins are not enough
  
    bluetoothValue();
    laserPointer();

    // turn left 
    if (valueRead == 4){
      turnLeft90();
    }

    //turn right 
    else if (valueRead == 5){
      turnRight90();
    }

    // moving forward, but selecting different speed 
    else if (valueRead == 3){
      speedInput = 'S';
      executeMotor();  
      while (valueRead ==3){ // while loop is needed to make sure executeMotor() only runs once ---> to get initialBearing 
        bluetoothValue();    
        errorValue = correctionFactor();
        executeCorrection(errorValue);   
      }
    }   


    else if (valueRead ==2 ){
      speedInput = 'M';
      executeMotor();
      while (valueRead ==2){ // while loop is needed to make sure executeMotor() only runs once 
        bluetoothValue();
        errorValue = correctionFactor();
        executeCorrection(errorValue);
      }         
    }

    else if (valueRead == 1){
      speedInput = 'F';
      executeMotor(); 
      while (valueRead ==1){ // while loop is needed to make sure executeMotor() only runs once 
        bluetoothValue ();
        errorValue = correctionFactor();
        executeCorrection(errorValue);
      }
    }

    else if (valueRead == 6) {
      openDoor(); 
    }

    else if (valueRead == 7){
      closeDoor(); 
    } 

    else if (valueRead ==8){
      stopButton();
    }

    else if (valueRead ==9){
      turnLeft(); // edit 
    }

    else if (valueRead ==10){
      turnRight(); // edit
    }

    else if (valueRead ==11){
      lineOfSightON();
    }

    else if (valueRead ==12){
      lineOfSightOFF();
    }

  }

} // close bracket of loop function 



/*void tiltSensor (void) { 
 
 a = digitalRead (4);
 b = digitalRead (7);
 
 if (a ==0 && b ==0){
 digitalWrite (buzzer, LOW);
 } else if ( a == 1 && b == 1 ) { 
 Serial.print("\nShip has been capsized!!!!!!");
 digitalWrite (buzzer,HIGH);
 } 
 }*/

void turnRight90 (void){
  motorStatus = 0 ; 
  leftMotorValue = 0;
  rightMotorValue = 0;

  analogWrite (rightMotor, rightMotorValue);
  analogWrite (leftMotor,leftMotorValue); // stop moving for a while 

    delay (1000);

  bearingValue = cmps03.read()/10;
  Serial.println("\nCurrent bearing: ");
  Serial.print(bearingValue);
  int stopValue = bearingValue + 90; 


  if (stopValue >360){
    stopValue = stopValue - 360;

    while ( bearingValue < 90 && bearingValue > stopValue ){  
      bearingValue = cmps03.read()/10;
      Serial.print("\nTurning right ......");
      Serial.println("\nCurrent bearing: ");
      Serial.println(bearingValue); 
      leftMotorValue = 255; // 255 denotes speed of motor, I need to callibrate next time  
      analogWrite (leftMotor, leftMotorValue);
    }
  } 

  else { 
    while (bearingValue <stopValue){
      bearingValue = cmps03.read()/10;
      Serial.print("\nTurning right ......");
      Serial.println("\nCurrent bearing: ");
      Serial.println(bearingValue);  
      leftMotorValue = 255;
      analogWrite (leftMotor, leftMotorValue);
    }
  }

  leftMotorValue = 0;
  rightMotorValue = 0;

  analogWrite (rightMotor, rightMotorValue);
  analogWrite (leftMotor,leftMotorValue); 

  delay(100);
}


void turnLeft90 (void){
  motorStatus = 0 ; 
  leftMotorValue = 0;
  rightMotorValue = 0;

  analogWrite (rightMotor, rightMotorValue);
  analogWrite (leftMotor,leftMotorValue); // stop moving for a while 

    delay (1000);

  bearingValue = cmps03.read()/10;
  Serial.println("\nCurrent bearing: ");
  Serial.print(bearingValue);
  int stopValue = bearingValue - 90; 


  if (stopValue < 0 ){
    stopValue = 360 - (360 - stopValue);

    while ( bearingValue < 270 && bearingValue < stopValue ){ 
      bearingValue = cmps03.read()/10;
      Serial.print("\nTurning left ......");
      Serial.println("\nCurrent bearing: ");
      Serial.println(bearingValue);  
      rightMotorValue = 255; // 255 denotes speed of motor, I need to callibrate next time  
      analogWrite (rightMotor, rightMotorValue);
    }
  } 

  else { 
    while (bearingValue > stopValue){
      bearingValue = cmps03.read()/10;
      Serial.print("\nTurning left ......");
      Serial.println("\nCurrent bearing: ");
      Serial.println(bearingValue); 
      rightMotorValue = 255;
      analogWrite (rightMotor, rightMotorValue);
    }
  }

  leftMotorValue = 0;
  rightMotorValue = 0;

  analogWrite (rightMotor, rightMotorValue);
  analogWrite (leftMotor,leftMotorValue); 

  delay(100);
}

void closeDoor (void){
  
  
  Serial.println("\nValue of limit switch: ");
  switchValue = digitalRead(limitSwitch);
  Serial.println (switchValue);
  digitalWrite (doorInput1, HIGH);
  digitalWrite (doorInput2, LOW); 
  delay(500);
  if (switchValue == 1){
      for (int i=75;i>5;i=i-10){ // value is int i=75 ; CALLIBRATED
      Serial.print("\nMechanical Arm closing ......"); 
      analogWrite (doorPWM, i);
      delay (450); // take note that it will not work without delay 
    }
    
    delay(1000); //DO NOT REMOVE THIS DELAY
    analogWrite (doorPWM, 0);
    delay(1000); // DO NOT REMOVE THIS DELAY
    
  } 
  else if (switchValue ==0){
    Serial.print("\nMechanical Arm is already closed."); 
  }
    
}

void openDoor (void){
  Serial.println("\nValue of limit switch: ");
  switchValue = digitalRead(limitSwitch);
  Serial.println (switchValue);

  digitalWrite (doorInput2, HIGH);
  digitalWrite (doorInput1, LOW);
  delay(500);
  if (switchValue == 0){
    for (int i=75;i>5;i=i-10){ // value has been CALLIBRATED to be 75
      switchValue = digitalRead (limitSwitch); 
      if (switchValue == 1){
        break;
      }
      Serial.print("\nMechanical Arm opening ......"); // 0,1 backward 
      analogWrite (doorPWM, i);
      delay(450); // I tested it out and it will not work without delay
    }

    delay(1000); // DO NOT REMOVE THIS DELAY
    analogWrite (doorPWM, 0);
    delay(1000); // DO NOT REMOVE THIS DELAY

  } 
  else if (switchValue ==1){
    Serial.print("\nMechanical Arm is already opened");
  }
}

void lineOfSightON (void) { 
  digitalWrite (posLaser, HIGH);
  Serial.println("\n pointer has been turned ON");
  laserStatus = 1; 


} // close bracket of the function 

void lineOfSightOFF (void){
  digitalWrite (posLaser, LOW);
  Serial.println("\nLaser pointer has been turned OFF");
  laserStatus = 0; 
}

void stopButton (void){
  motorStatus = 0; 
  Serial.print("\nStopped.");
  digitalWrite (leftMotor, 0);
  digitalWrite (rightMotor, 0);
  delay(100);
}

void turnLeft (void){
  motorStatus = 0 ; 
  leftMotorValue = 0;
  rightMotorValue = 0;

  analogWrite (rightMotor, rightMotorValue);
  analogWrite (leftMotor,leftMotorValue); // stop moving for a while 

    delay (1000);

  while (valueRead == 9){
    bluetoothValue();
    Serial.println("\nTurning Left ......");
    rightMotorValue = 255; // 255 denotes speed of motor, I need to callibrate next time  
    analogWrite (rightMotor, rightMotorValue);
  }
}

void turnRight (void){
  motorStatus = 0 ; 
  leftMotorValue = 0;
  rightMotorValue = 0;

  analogWrite (rightMotor, rightMotorValue);
  analogWrite (leftMotor,leftMotorValue); // stop moving for a while 

    delay (1000);

  while (valueRead == 10){
    bluetoothValue();
    Serial.println("\nTurning Right ......");
    leftMotorValue = 255; // 255 denotes speed of motor, I need to callibrate next time  
    analogWrite (leftMotor, leftMotorValue);
  }
}

void bluetoothValue (void){
  tempValueRead = Serial.read();
  if (tempValueRead >=0){
    valueRead = tempValueRead;
    Serial.print ("\nIncoming bluetooth value: ");
    Serial.println(valueRead);
  }
}




void executeMotor (void){
  if (speedInput =='S'){
    Serial.print("\nSPEED: Slow/100");
    speedMotor = 100; 
  } 
  else if (speedInput =='M'){
    Serial.print("\nSPEED: Medium/150"); 
    speedMotor = 150;
  } 
  else if (speedInput =='F'){
    Serial.print("\nSPEED: Fast/255");
    speedMotor = 220;  
  }
  rightMotorValue = speedMotor; 
  leftMotorValue = rightMotorValue;
  analogWrite (leftMotor, leftMotorValue);
  analogWrite (rightMotor, rightMotorValue);

  initialBearing = cmps03.read()/10; //reads the bearing only once, executeMotor () is not in the while loop 
  motorStatus = 1; 

}


double correctionFactor (void){
  //left is faster than the right,turns RIGHT. 
  //right is faster than the left,turns LEFT.
  if (speedInput =='S'){
    Serial.print("\nExecuting Slow Mode");
  } 
  else if (speedInput =='M'){
    Serial.print("\nExecuting Medium Mode"); 
  } 
  else if (speedInput =='F'){
    Serial.print("\nExecuting Fast Mode");
  }

  faultBearing = cmps03.read()/10; 
  Serial.print("\nCurrent bearing: ");
  Serial.println(faultBearing);

  double error; 
  error = faultBearing - initialBearing; //this is the line that causes this function to repeat itself SOLVE THIS SHIT NOW 

  // the problem is the "differenceAngle"  variable 


  if ( error >180){
    error = -(360-error); //negative difference ---> deviated towards LEFT, turns RIGHT to readjust 
  } 
  else if(error < -180){
    error+=360; //positive difference ---> deviated towards RIGHT, turns LEFT to readjust 
  } 

  return error; 
}

void executeCorrection(double x){
  // from here
  if (x>90 || x<-90){
    if (x > 90){
      motorStatus = 0;
      while (x > 10){
        Serial.print("\nExecuting correctionOverride.");
        faultBearing = cmps03.read()/10; 
        Serial.print("\nCurrent bearing: ");
        Serial.println(faultBearing);

        x = faultBearing - initialBearing; //this is the line that causes this function to repeat itself SOLVE THIS SHIT NOW 

        if ( x >180){
          x = -(360-x); //negative difference ---> deviated towards LEFT, turns RIGHT to readjust 
        } 
        else if(x < -180){
          x+=360; //positive difference ---> deviated towards RIGHT, turns LEFT to readjust 
        } 

        analogWrite (leftMotor, 0);
        analogWrite (rightMotor, speedMotor);
        Serial.print("\nCurrent leftMotorValue:");
        Serial.println (leftMotorValue);
        Serial.print("\nCurrent rightMotorValue:");
        Serial.println(rightMotorValue);
        //delay(100);
      }

    } 
    else if (x < - 90){
      motorStatus = 0; 
      while (x< -10){
        Serial.print("\nExecuting correctionOverride.");
        faultBearing = cmps03.read()/10; 
        Serial.print("\nCurrent bearing: ");
        Serial.println(faultBearing);

        x = faultBearing - initialBearing; //this is the line that causes this function to repeat itself SOLVE THIS SHIT NOW 

        if (x >180){
          x = -(360-x); //negative difference ---> deviated towards LEFT, turns RIGHT to readjust 
        } 
        else if(x < -180){
          x+=360; //positive difference ---> deviated towards RIGHT, turns LEFT to readjust 
        }
        analogWrite (leftMotor, speedMotor);
        analogWrite (rightMotor, 0);
        Serial.print("\nCurrent leftMotorValue:");
        Serial.println (leftMotorValue);
        Serial.print("\nCurrent rightMotorValue:");
        Serial.println(rightMotorValue);
        //delay(100);
      }
    }
  }

  else {
    //to here    
    leftMotorValue = speedMotor - x;
    rightMotorValue = speedMotor + x; 

    if (leftMotorValue <0){
      leftMotorValue = leftMotorValue*(-1);
    } 
    else if (leftMotorValue > 0){
      leftMotorValue = leftMotorValue; 
    } 

    if (rightMotorValue <0){
      rightMotorValue = rightMotorValue*(-1);
    } 
    else if (rightMotorValue >0){
      rightMotorValue = rightMotorValue;
    }


    if (leftMotorValue > 255){
      leftMotorValue = 255;
    }

    if (rightMotorValue > 255){
      rightMotorValue = 255;
    }

    analogWrite (leftMotor, leftMotorValue);
    analogWrite (rightMotor, rightMotorValue);
    delay(100);
    Serial.print("\nCurrent leftMotorValue:");
    Serial.println (leftMotorValue);
    Serial.print("\nCurrent rightMotorValue:");
    Serial.println(rightMotorValue);

  }
  
  laserPointer();
}

/*  
 void correctionOverride (double error){
 
 double differenceAngle = error; 
 Serial.print("\ndifferenceAnle in correctionOverride1: ");
 Serial.println(differenceAngle);
 //left is faster than the right,turns RIGHT. 
 //right is faster than the left,turns LEFT.
 if (differenceAngle > 90){
 motorStatus = 0;
 while (differenceAngle > 10){
 faultBearing = cmps03.read()/10; 
 Serial.print("\nFault value: ");
 Serial.println(faultBearing);
 
 differenceAngle = faultBearing - initialBearing; //this is the line that causes this function to repeat itself SOLVE THIS SHIT NOW 
 
 Serial.print("\ndifferenceAnle in correctionOverride2(a): ");
 Serial.println(differenceAngle);
 
 if ( differenceAngle >180){
 differenceAngle = -(360-differenceAngle); //negative difference ---> deviated towards LEFT, turns RIGHT to readjust 
 } else if(error < -180){
 differenceAngle+=360; //positive difference ---> deviated towards RIGHT, turns LEFT to readjust 
 } 
 
 analogWrite (leftMotor, 0);
 analogWrite (rightMotor, speedMotor);
 //delay(100);
 }
 
 } else if (differenceAngle < - 90){
 motorStatus = 0; 
 while (differenceAngle < -10){
 
 faultBearing = cmps03.read()/10; 
 Serial.print("\nFault value: ");
 Serial.println(faultBearing);
 
 differenceAngle = faultBearing - initialBearing; //this is the line that causes this function to repeat itself SOLVE THIS SHIT NOW 
 Serial.print("\ndifferenceAnle in correctionOverride2(b): ");
 Serial.println(differenceAngle);
 if ( differenceAngle >180){
 differenceAngle = -(360-differenceAngle); //negative difference ---> deviated towards LEFT, turns RIGHT to readjust 
 } else if(error < -180){
 differenceAngle+=360; //positive difference ---> deviated towards RIGHT, turns LEFT to readjust 
 } 
 
 analogWrite (leftMotor, speedMotor);
 analogWrite (rightMotor, 0);
 //delay(100);
 }
 }
 
 analogWrite (leftMotor, speedMotor);
 analogWrite (rightMotor, speedMotor);
 //delay(100);
 motorStatus = 1;
 
 }
 */
 
 void laserPointer (void){
   if (laserStatus==0){
     Serial.print("\nValue of LDR is: ");
     valueLDR = analogRead(posLDR); 
     secondValueLDR = 9999999 ; //to avoid getting uninitialized values, to be acted as default values
     Serial.println (valueLDR);
     delay(500); // may be removed later 
   }
    
   int boundaryLDR = 10;
      //to be callibrated, this is the boundary where if the laser point shines on it, the value definitely exceeds it 
      
   if (laserStatus == 1 && motorStatus==1){
     secondValueLDR = analogRead(posLDR);
     Serial.println("\nInitial value of LDR: ");
     Serial.print (valueLDR);
     Serial.println("\nChanges to the value of LDR: ");
     Serial.print(secondValueLDR);
     
   }
   
   int differenceLDR = secondValueLDR - valueLDR; 
   
   if (differenceLDR <0){
     differenceLDR = -differenceLDR;
   }
   
   if ( differenceLDR < boundaryLDR){ 
     Serial.println("\nThe tank is full!");
     digitalWrite (buzzer, HIGH);
   } else {
     digitalWrite(buzzer, LOW); 
     Serial.println("\nThe tank is not full yet."); 
   }
   delay(100);
 }
     
      
