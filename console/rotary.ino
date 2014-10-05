void InitRotary(uint8_t downPin, uint8_t upPin, uint8_t button){
//
// pass the DIGITAL pins. Sample code uses Analog inputs, but they are read with digitalRead
encoder = new ClickEncoder(downPin, upPin, button);
  Timer1.initialize(1000);

}

void timerIsr() {
  encoder->service();
}

void EnableRotaryScanner(){
  Timer1.attachInterrupt(timerIsr); 
}

void DisableRotaryScanner(){
   Timer1.detachInterrupt(); 
}