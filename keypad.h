void setupKeypad(){
  // matrix keypad
  pinMode(keypadOutputClockPin, OUTPUT); // make the clock pin an output
  pinMode(keypadOutputDataPin , OUTPUT); // make the data pin an output

  pinMode(ploadPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, INPUT);

  digitalWrite(clockPin, LOW);
  digitalWrite(ploadPin, HIGH);  

  // nav keys
  pinMode(navploadPin, OUTPUT);
  pinMode(navclockPin, OUTPUT);
  pinMode(navdataPin, INPUT);

  digitalWrite(navclockPin, LOW);
  digitalWrite(navploadPin, HIGH);  
}



/* This function is essentially a "shift-in" routine reading the
 * serial Data from the shift register chips and representing
 * the state of those pins in an unsigned integer (or long).
 */
 
byte read_shift_regs(int row){
  byte bitVal;
  byte bytesVal;

  digitalWrite(ploadPin, LOW);
  delayMicroseconds(PULSE_WIDTH_USEC);
  digitalWrite(ploadPin, HIGH);
  

    /* Loop to read each bit value from the serial out line
     * of the SN74HC165N.
    */
    for(int i = 0; i < DATA_WIDTH; i++)
    {
        bitVal = digitalRead(dataPin);

        /* Set the corresponding bit in bytesVal.*/
        bytesVal |= (bitVal << ((DATA_WIDTH-1) - i));

        /* Pulse the Clock (rising edge shifts the next bit).*/
        digitalWrite(clockPin, HIGH);
        delayMicroseconds(PULSE_WIDTH_USEC);
        digitalWrite(clockPin, LOW);
    }

    return(bytesVal);
    
}

void scanKeypad(){

  // scan the key matrix
  shiftOut(keypadOutputDataPin, keypadOutputClockPin, LSBFIRST,  254);
  newKeyPadValues[0] = read_shift_regs(1);
  
  shiftOut(keypadOutputDataPin, keypadOutputClockPin, LSBFIRST,  247);
  newKeyPadValues[1] = read_shift_regs(2);
  
  shiftOut(keypadOutputDataPin, keypadOutputClockPin, LSBFIRST,  251);
  newKeyPadValues[2] = read_shift_regs(3);
  
  shiftOut(keypadOutputDataPin, keypadOutputClockPin, LSBFIRST,  253);
  newKeyPadValues[3] = read_shift_regs(4);
}
 
 
 
  
void handleKeypad(){
  int bitVal;
  
  
  for (int row = 0; row <=3;  row++){
    if (newKeyPadValues[row] != oldKeyPadValues[row]){
     
      lcd.clear();
    
        for (byte mask = 00000001; mask>0; mask <<= 1) {
          
          if (newKeyPadValues[row] & mask){ bitVal = 1; }
         
          curPosition = buttonMapping[row][i];

        switch(editMode){ 
          
          case 0: // play
            
            if ( bitVal == 1){ // button pressed
              if ( patternData[currentChannel][0][curPosition] == 1){ 
                patternData[currentChannel][0][curPosition] = 0;
              } else { 
                patternData[currentChannel][0][curPosition] = 1;
              }
            } 
            
            break;
            
            
          case 1: // edit
          
            if ( bitVal == 1){
              currentStep = curPosition;
              updateLCD=1; 
            }
            
            break;
            
            
          case 2: // record
      
            if ( bitVal == 1){ // pressed button
                if (recordLastNote == curPosition){ // hold it
                     patternData[currentChannel][0][tickCounter] = 2;
                     patternData[currentChannel][1][tickCounter] = curPosition+oct3;
                }
                  
                if (recordLastNote != curPosition && recordLastPosition != i){
                    //MIDI.sendNoteOff(recordLastNote+oct3,0,currentChannel +1);  
                    MIDI.sendNoteOn(curPosition+oct3,127,currentChannel +1);
                   
                    patternData[currentChannel][0][tickCounter] = 1;
                    patternData[currentChannel][1][tickCounter] = curPosition+oct3;
                    
                    recordLastNote = curPosition;
                    recordLastPosition = i;   
                } 
            } else { // released button
              if ( recordLastNote == curPosition && recordLastPosition == i) {     
                 MIDI.sendNoteOff(recordLastNote+oct3,0,currentChannel +1);  
                 patternData[currentChannel][0][tickCounter] = 0;
                 patternData[currentChannel][1][tickCounter] = curPosition+oct3;
                 recordLastNote=0;
                 recordLastPosition = 0;
               }
            }
            
            break;
  
        } 
        
        oldKeyPadValues[row] = newKeyPadValues[row];
      } 
    }
  } 
}
