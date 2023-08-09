void reading() {
  while (currentFrame==2) {
  digitalWrite(psu_rdm, HIGH);
  
  if (ssrfid.available() > 0){
    bool call_extract_tag = false;
    
    int ssvalue = ssrfid.read(); // read 
    if (ssvalue == -1) { // no data was read
      return;
    }

    if (ssvalue == 2) { // RDM630/RDM6300 found a tag => tag incoming 
      buffer_index = 0;
    } else if (ssvalue == 3) { // tag has been fully transmitted  
      ssrfid.end();
      call_extract_tag = true; // extract tag at the end of the function call
      
    }

    if (buffer_index >= BUFFER_SIZE) { // checking for a buffer overflow (It's very unlikely that an buffer overflow comes up!)
      //Serial.println("Error: Buffer overflow detected! ");
      return;
    }
    
    buffer[buffer_index++] = ssvalue; // everything is alright => copy current value to buffer

    if (call_extract_tag == true) {
      if (buffer_index == BUFFER_SIZE) {
        unsigned tag = extract_tag();
        digitalWrite(psu_rdm, LOW);


        break;
      } else { // something is wrong... start again looking for preamble (value: 2)
        buffer_index = 0;
        return;
        }
      }    
    }    
  }
}
unsigned extract_tag() {
    display.setCursor(0, 0);
    display.clearDisplay();
    display.setTextSize(1); 
    display.setTextColor(SSD1306_WHITE);
      for (int b=1; b<11; b++) { 
        incoming=incoming + (char (buffer[b])); //Exporting raw data to Incoming Char Array Variable
        }
    uint8_t msg_head = buffer[0];
    uint8_t *msg_data = buffer + 1; // 10 byte => data contains 2byte version + 8byte tag
    uint8_t *msg_data_version = msg_data;
    uint8_t *msg_data_tag = msg_data + 2;
    uint8_t *msg_checksum = buffer + 11; // 2 byte
    uint8_t msg_tail = buffer[13];

    // print message that was sent from RDM630/RDM6300
    //Serial.println("--------");

    //Serial.print("Message-Head: ");
    //Serial.println(msg_head);

    //Serial.println("Message-Data (HEX): ");
    for (int i = 0; i < DATA_VERSION_SIZE; ++i) {
      //Serial.print(char(msg_data_version[i]));
      display.print(char(msg_data_version[i]));
    }
    //Serial.println(" (version)");
    display.print(" (V.)");
    for (int i = 0; i < DATA_TAG_SIZE; ++i) {
      //Serial.print(char(msg_data_tag[i]));
    
    display.print(char(msg_data_tag[i]));
    }

    display.print(F(" (tag)"));
    //Serial.println(" (tag)");

    //Serial.print("Message-Checksum (HEX): ");
    for (int i = 0; i < CHECKSUM_SIZE; ++i) {
      //Serial.print(char(msg_checksum[i]));
    }
    //Serial.println("");

    //Serial.print("Message-Tail: ");
    //Serial.println(msg_tail);

    //Serial.println("--");

    long tag = hexstr_to_value(msg_data_tag, DATA_TAG_SIZE);
    //Serial.print("Extracted Tag: ");
    //Serial.println(tag);
    display.print("000");display.print(tag);
    display.print(F(" (serial)"));

    long checksum = 0;
    for (int i = 0; i < DATA_SIZE; i+= CHECKSUM_SIZE) {
      long val = hexstr_to_value(msg_data + i, CHECKSUM_SIZE);
      checksum ^= val;
    }
    //Serial.print("Extracted Checksum (HEX): ");
    //Serial.print(checksum, HEX);
    
    if (checksum == hexstr_to_value(msg_checksum, CHECKSUM_SIZE)) { // compare calculated checksum to retrieved checksum
      //Serial.print(" (OK)"); // calculated checksum corresponds to transmitted checksum!
      
    } else {
      //Serial.print(" (NOT OK)"); // checksums do not match
      display.println(F("(NOT OK)"));
      
    }

    //Serial.println("");
    //Serial.println("--------");
    

    display.fillRoundRect(0, 20, 60, 32, 5, SSD1306_WHITE);
    display.setTextColor(BLACK);
    display.setCursor(18,22);
    display.print("SAVE");
    display.fillRoundRect(64, 20, 64, 32, 5, SSD1306_WHITE);
    display.setCursor(76,22);
    display.print("EMULATE");

    display.display();      // Show initial text
    display.setTextColor(WHITE);

    currentFrame = 5;
    return tag;

}
long hexstr_to_value(char *str, unsigned int length) { // converts a hexadecimal value (encoded as ASCII string) to a numeric value
  char* copy = malloc((sizeof(char) * length) + 1); 
  memcpy(copy, str, sizeof(char) * length);
  copy[length] = '\0'; 
  // the variable "copy" is a copy of the parameter "str". "copy" has an additional '\0' element to make sure that "str" is null-terminated.
  long value = strtol(copy, NULL, 16);  // strtol converts a null-terminated string to a long value
  free(copy); // clean up 
  return value;
}
