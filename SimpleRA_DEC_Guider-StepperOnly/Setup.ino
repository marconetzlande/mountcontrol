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
}
