#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>

#define buttonPin1  12 // Enroll
#define buttonPin2  11 // Ok
#define buttonPin3  10 //UP
#define buttonPin4  9  //Down

#define RedLed   13
#define GrLed    8
#define Buzz     7

int button1_State = 0;
int button2_State = 0;
int button3_State = 0;
int button4_State = 0;

int count_value = 1;
int prestate =0;

bool EnrollStatus = false;

LiquidCrystal_I2C lcd(0x27,16,2);
SoftwareSerial mySerial(2, 3);

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
uint8_t id;


//////////////////Functions////////////////////////////



uint8_t readnumber(void) {
  uint8_t num = 0;
  
  while (EnrollStatus) {
    
        // read the state of the pushbutton value:
      button2_State = digitalRead(buttonPin2);
      button3_State = digitalRead(buttonPin3);
      button4_State = digitalRead(buttonPin4);
      // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
      if (button3_State == HIGH && prestate == 0) {
        count_value++;  
      lcd.setCursor(0,1);
      lcd.print(count_value);
      lcd.print(" ");
        prestate = 1;
      }
    //decrement
      else if (button4_State == HIGH && prestate == 0) {
        count_value--;

        if (count_value <= 1)
        {
          count_value = 1;
        }
        
      lcd.setCursor(0,1);
      lcd.print(count_value);
      lcd.print(" ");
      prestate = 1;
      } 
      else if (button2_State == HIGH && prestate == 0)
      {
        int tempNum = count_value;
        lcd.setCursor(0,0);
        lcd.print("Enrolling ID #");
        lcd.setCursor(0,1);
        lcd.print(tempNum);
        prestate = 1;
        break;
      }
      
    else if(button2_State == LOW && button3_State == LOW && button4_State == LOW) {
        prestate = 0;
      }
  }
  return count_value;
}

uint8_t getFingerprintEnroll() {

  id = readnumber();
  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #");
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
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
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
    Serial.println("Stored!");
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

  return true;
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
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  /// Finger sensor setup
  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  lcd.setCursor(4,0);
  lcd.print("Student");
  lcd.setCursor(2,1);
  lcd.print("Attendance");
  delay(2000);
  lcd.clear();

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

  if (button1_State == HIGH && prestate == 0)
  {
    lcd.setCursor(0,0);
    lcd.print("Enter Student ID");
    EnrollStatus = true;
    while (! getFingerprintEnroll() );
  }

}