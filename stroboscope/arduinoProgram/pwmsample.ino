void setup() {
 
  TCCR1A =   _BV(COM1B1) | _BV(WGM10);  // PWM, Phase and Frequency Correct, Non Inverting
  
  TCCR1B =   _BV(CS10) | _BV(WGM13);  // no prescaling
   
  DDRB |= _BV(PB1) | _BV(PB2); 
  OCR1A = 50000;
   OCR1B = 10 ;

}

void loop() {

  long i = 0;
  for (i=0; i < 50000; i++) {
    OCR1B = i;
    delay(10);
  }
  

}
