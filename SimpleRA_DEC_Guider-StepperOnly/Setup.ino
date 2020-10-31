void setupStepper() 
{
  RA_Stepper.setup();
  DEC_Stepper.setup();
  
  pinMode(RA_PLUS, INPUT_PULLUP);
  pinMode(RA_MINUS, INPUT_PULLUP);
  pinMode(DEC_PLUS, INPUT_PULLUP);
  pinMode(DEC_MINUS, INPUT_PULLUP);

  pinMode(7, INPUT_PULLUP);

  pinMode(LED1, OUTPUT);
}

void setup () {
  Serial.begin(115200);
  setupStepper();
}
