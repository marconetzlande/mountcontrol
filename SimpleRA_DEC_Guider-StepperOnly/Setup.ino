void setupStepper() 
{
  RA_Stepper.setup();
  DEC_Stepper.setup();

  pinMode(VOLTAGE_PIN, INPUT);
  
  pinMode(RA_PLUS, INPUT_PULLUP);
  pinMode(RA_MINUS, INPUT_PULLUP);
  pinMode(DEC_PLUS, INPUT_PULLUP);
  pinMode(DEC_MINUS, INPUT_PULLUP);

//  pinMode(GO_HOME_PIM, INPUT_PULLUP);

  pinMode(LED1, OUTPUT);
}

void setup () {
  Serial.begin(115200);
  setupRTC();
  setupStepper();

  /*
  while (true) {
    RA_Stepper.move(RA_STEPS);
    while (RA_Stepper.nextAction()) Serial.println(RA_Stepper.getSteps());
    delay(6000);
  }
  
  abort();
  */
  
  /*
  while (true) {
    getRTC_Time();
    Serial.print(rtcnow.year(), DEC);
    Serial.print('/');
    Serial.print(rtcnow.month(), DEC);
    Serial.print('/');
    Serial.print(rtcnow.day(), DEC);
    Serial.print(' ');
    Serial.print(rtcnow.hour(), DEC);
    Serial.print(':');
    Serial.print(rtcnow.minute(), DEC);
    Serial.print(':');
    Serial.print(rtcnow.second(), DEC);
    Serial.println();
    delay(500);
  }
  */
}
