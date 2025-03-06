#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>
#include <EEPROM.h>
#include <HardwareSerial.h>

#define buttonPin1  12 // Enroll
#define buttonPin2  11 // Ok
#define buttonPin3  10 //UP
#define buttonPin4  9  //Down

#define RedLed   13
#define GrLed    8
#define Buzz     7

bool wifiTemp = false;

int button1_State = 0;
int button2_State = 0;
int button3_State = 0;
int button4_State = 0;

int count_value = 1;
int prestate =0;
int student_id = 0;
int Totalfinger = 100;

bool EnrollStatus = false;
bool EnrollBnt = false;
bool BackBtn = false;

bool FingerArray[100] = {};


LiquidCrystal_I2C lcd(0x27,16,2);
SoftwareSerial mySerial(2, 3);
SoftwareSerial EspSerial(4, 5);

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
uint8_t id;


//////////////////Functions////////////////////////////

uint8_t readnumber(void) {
  
    lcd.setCursor(0,0);
    lcd.print("Enter Student ID");
    lcd.setCursor(0,1);
    lcd.print("Student ");
    int cnt = 0;
  
  while (EnrollStatus) {
    
        // read the state of the pushbutton value:
      button1_State = digitalRead(buttonPin1);
      button2_State = digitalRead(buttonPin2);
      button3_State = digitalRead(buttonPin3);
      button4_State = digitalRead(buttonPin4);
      // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
      if (button1_State == HIGH){
        cnt ++;
        if (cnt > 10000)
        {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("BACK");
          BackBtn = true;
          cnt = 0;
          EnrollStatus = false;
          prestate = 1;
          break;
          /* code */
        }
      }
      else if (button3_State == HIGH && prestate == 0) { // increment 
        count_value++;
        
        if (EEPROM.read(count_value) == 1)
        {
          count_value++;
          /* code */
        }
          
      lcd.setCursor(8,1);
      lcd.print(count_value);
      lcd.print(" ");
        prestate = 1;
      }
    //decrement
      else if (button4_State == HIGH && prestate == 0) { // Decrement 
        count_value--;

        if (count_value <= 1)
        {
          count_value = 1;
        }

        if (EEPROM.read(count_value) == 1)
        {
          count_value++;
          /* code */
        }
        
      lcd.setCursor(8,1);
      lcd.print(count_value);
      lcd.print(" ");
      prestate = 1;
      } 
      else if (button2_State == HIGH && prestate == 0) // ok Pressed !!!
      {
        int tempNum = count_value;
        EnrollStatus = false;
        EnrollBnt = true;
        student_id = count_value;
        prestate = 1;
        break;
      }
      
    else if(button1_State == LOW && button2_State == LOW && button3_State == LOW && button4_State == LOW) {
        prestate = 0;
      }
  }
  return count_value;
}

uint8_t getFingerprintEnroll() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Enrolling ID ");
  lcd.setCursor(0,13);
  lcd.print(student_id);
  prestate = 0;

  if (student_id == 0)
  {
    return;
    /* code */
  }else{
    id = student_id;
  }

  if (BackBtn)
  {
    BackBtn = false;
    return;
    /* code */
  }
  
    //id = student_id;
  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("valid finger Enrl");
  Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  Serial.println("Remove finger");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Place Again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  Serial.print("ID "); Serial.println(id);

  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    EEPROM.write(id,1);
    Serial.println("Stored!!");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Stored !!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  } 

  EnrollBnt = false;
  EnrollStatus = true;
  return true;
}

uint8_t getFingerprintID() {

  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Finger not match");
    delay(1000);
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Student ");
  lcd.setCursor(8,0);
  lcd.print(finger.fingerID);
  lcd.setCursor(0,1);
  lcd.print("Found !!!");
  EspSerial.write(finger.fingerID);

  delay(1000);

  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}


uint8_t deleteFingerprint(uint8_t id) {
  uint8_t p = -1;

  p = finger.deleteModel(id);

  if (p == FINGERPRINT_OK) {
    Serial.println("Deleted!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not delete in that location");
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
  } else {
    Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
  }

  return p;
}

//////////////////Functions////////////////////////////

void setup() {
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(buttonPin3, INPUT);
  pinMode(buttonPin4, INPUT);

  pinMode(RedLed, OUTPUT);
  pinMode(GrLed, OUTPUT);
  pinMode(Buzz, OUTPUT);
  Serial.begin(115200);
  EspSerial.begin(115200);

  lcd.init();
  lcd.backlight();

  while (!wifiTemp)
  {
    if (Serial.available()>0)
    {
      Serial.print(EspSerial.read());
      /* code */
    }
  } 

  /// Finger sensor setup
  finger.begin(57600);
  lcd.setCursor(4,0);
  lcd.print("Student");
  lcd.setCursor(2,1);
  lcd.print("Attendance");
  delay(2000);
  lcd.clear();

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  if (finger.verifyPassword()) {
    lcd.setCursor(0,0);
    lcd.println("Module Found!");
  } else {
    lcd.setCursor(0,0);
    lcd.println("Module Not Found");
    while (1) { delay(1); }
  }

 
  
}

void loop() {

  button1_State = digitalRead(buttonPin1);
  button2_State = digitalRead(buttonPin2);

  if (button1_State == HIGH && prestate == 0)
  {
      prestate = 1;
      EnrollStatus = true;
      
      readnumber();      
      if (EnrollBnt)
      {
        while (! getFingerprintEnroll() );
        /* code */
      }

  }
  else if (button2_State == HIGH && prestate == 0) /// delete finger
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Delet Student ID ");

    delay(1000);

    if (button1_State == HIGH && prestate == 0){

    }else if (button1_State == LOW && button2_State == LOW)
    {
      prestate = 0;
      return;
      //break;
    }
    prestate = 1;
  }
  else if(button1_State == LOW && button2_State == LOW){
    prestate = 0;
  } 

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Place Finger !!");
  getFingerprintID();

}