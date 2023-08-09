void code(String val) {
  int a=0; int b=0; int c=0; int d=0;
  for (int i=0; i<=10; i++) {
  if (val[i] == '0') {load = load + "00000";}
  if (val[i] == '1') {load = load + "00011";}
  if (val[i] == '2') {load = load + "00101";}
  if (val[i] == '3') {load = load + "00110";}
  if (val[i] == '4') {load = load + "01001";}
  if (val[i] == '5') {load = load + "01010";}
  if (val[i] == '6') {load = load + "01100";}
  if (val[i] == '7') {load = load + "01111";}
  if (val[i] == '8') {load = load + "10001";}
  if (val[i] == '9') {load = load + "10010";}
  if (val[i] == 'A') {load = load + "10100";}
  if (val[i] == 'B') {load = load + "10111";}
  if (val[i] == 'C') {load = load + "11000";}
  if (val[i] == 'D') {load = load + "11011";}
  if (val[i] == 'E') {load = load + "11101";}
  if (val[i] == 'F') {load = load + "11110";}
  }

   for (int p=0; p<9; p++) {display.print(header[p]);}
   display.print(" ");
   for (int o=0; o<50; o++) { 
      int convoy;
      convoy = String(load[o]).toInt();
      payload[o] = convoy;
      display.print(payload[o]);
      }


    for (int o=0; o<50; o+=5) { if ( load[o] == '1') { a++ ;} }
    for (int o=1; o<50; o+=5) { if ( load[o] == '1') { b++ ;} }
    for (int o=2; o<50; o+=5) { if ( load[o] == '1') { c++ ;} }
    for (int o=3; o<50; o+=5) { if ( load[o] == '1') { d++ ;} }   
   
    if ((a % 2) == 0 ) { a=0;} else { a = 1;} ;
    if ((b % 2) == 0 ) { b=0;} else { b = 1;} ;
    if ((c % 2) == 0 ) { c=0;} else { c = 1;} ;   
    if ((d % 2) == 0 ) { d=0;} else { d = 1;} ;
    parity[0] = a;
    parity[1] = b;
    parity[2] = c;
    parity[3] = d;
    display.print("\n");
    for (int p=0; p<5; p++) {display.print(parity[p]);}

  return load;
}


void encode(int clock_half, int signal) {
  int man_encoded = clock_half ^ signal;
  
  //if it's 1, set the pin LOW (this will tune the antenna and the reader sees this as a high signal)
  //if it's 0, set the pin to HIGH  (this will detune the antenna and the reader sees this as a low signal)
  if(man_encoded == 0)
  {
     digitalWrite(coil_pin, LOW);
  }
  else
  {
    digitalWrite(coil_pin, HIGH);
  }
}


void emulate(int header[], int payload[], int parity[]){
      for (int h=0; h<=8; h++) {
      //Serial.print(header[h]);
      encode(0, header[h]);
      delayMicroseconds(256);
      encode(1, header[h]);
      delayMicroseconds(256); 
      }
      
      for (int l=0; l<50; l++) {
      //Serial.print(payload[l]);
      encode(0, payload[l]);
      delayMicroseconds(256);
      encode(1, payload[l]);
      delayMicroseconds(256);       
      }
      
      for (int p=0; p<5; p++) {
      //Serial.print(parity[p]);
      encode(0, parity[p]);
      delayMicroseconds(256);
      encode(1, parity[p]);
      delayMicroseconds(256);      
        }
  }
