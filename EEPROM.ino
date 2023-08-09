void emulate_buffer1() {
        display.clearDisplay();
      for (int eep=0; eep<50; eep++) {
        payload[eep] = EEPROM.read(eep);
        display.print(payload[eep]);
      }        
      display.print(" ");
      for (int eep=50; eep<55; eep++) {
        parity[eep-50] = EEPROM.read(eep);
        display.print(parity[eep-50]);
      }
      
      display.display();
      while(true) {
      digitalWrite(opto, HIGH);
      emulate(header, payload, parity);
      digitalWrite(opto, LOW);
      }
}

void emulate_buffer2() {
      display.clearDisplay();
      for (int eep=55; eep<105; eep++) {
        payload[eep-55] = EEPROM.read(eep);
        display.print(payload[eep-55]);
      }        
      display.print(" ");
      for (int eep=105; eep<110; eep++) {
        parity[eep-105] = EEPROM.read(eep);
        display.print(parity[eep-105]);
      }
      
      display.display();
      while(true) {
      digitalWrite(opto, HIGH);
      emulate(header, payload, parity);
      digitalWrite(opto, LOW);
      }
}


void emulate_buffer3() {
      display.clearDisplay();
      for (int eep=110; eep<160; eep++) {
        payload[eep-110] = EEPROM.read(eep);
        display.print(payload[eep-110]);
      }        
      display.print(" ");
      for (int eep=160; eep<165; eep++) {
        parity[eep-160] = EEPROM.read(eep);
        display.print(parity[eep-160]);
      }
      
      display.display();
      while(true) {
      digitalWrite(opto, HIGH);
      emulate(header, payload, parity);
      digitalWrite(opto, LOW);
      }
}

void emulate_buffer4() {
      display.clearDisplay();
      for (int eep=165; eep<215; eep++) {
        payload[eep-165] = EEPROM.read(eep);
        display.print(payload[eep-165]);
      }        
      display.print(" ");
      for (int eep=215; eep<220; eep++) {
        parity[eep-215] = EEPROM.read(eep);
        display.print(parity[eep-215]);
      }
      
      display.display();
      while(true) {
      digitalWrite(opto, HIGH);
      emulate(header, payload, parity);
      digitalWrite(opto, LOW);
      }
}

void emulate_buffer5() {
      display.clearDisplay();
      for (int eep=220; eep<270; eep++) {
        payload[eep-220] = EEPROM.read(eep);
        display.print(payload[eep-220]);
      }        
      display.print(" ");
      for (int eep=270; eep<275; eep++) {
        parity[eep-270] = EEPROM.read(eep);
        display.print(parity[eep-270]);
      }
      
      display.display();
      while(true) {
      digitalWrite(opto, HIGH);
      emulate(header, payload, parity);
      digitalWrite(opto, LOW);
      }
}
