#define INPUT0 2
#define INPUT1 3
#define INPUT2 4
#define INPUT3 5
#define INPUT4 6
#define INPUT5 7
#define INPUT6 8

#define LED0 9
#define LED1 10
#define LED2 11
#define LED3 12
#define LED4 14
#define LED5 15
#define LED6 15

#define INPUTS 7

#define OFF   0
#define ON    1
#define FLASH 2

char inputState;
char debounceTime = 50;

char inputs[] =  {INPUT0, INPUT1, INPUT2, INPUT3, INPUT4, INPUT5, INPUT6};
char outputs[] = {LED0, LED1, LED2, LED3, LED4, LED5, LED6};
char ledState[INPUTS];
char prevState[INPUTS];
long debounce[INPUTS];
char buffer[16];
char *bufferPtr = buffer;
char *bufferStart = buffer;

void setup(){
  Serial.begin(57600);
  Serial.print("Starting flashingButtons (");
  Serial.print(INPUTS);
  Serial.println(" buttons)");
  for(char i = 0; i < INPUTS; i++){
    pinMode(inputs[i], INPUT_PULLUP);
    pinMode(outputs[i], OUTPUT);
    prevState[i] = digitalRead(inputs[i]);
    ledState[i] = OFF;
  }
}

void handleButtons(){
  char currentState;
  for(char pin = 0; pin< INPUTS; pin++){
    currentState = digitalRead(inputs[pin]);
    if((prevState[pin] == LOW && currentState == HIGH) || (prevState[pin] == HIGH && currentState == LOW)){
      // start the timeout
      debounce[pin] = millis() + debounceTime;
    }
    if(debounce[pin] && millis() >= debounce[pin]){
      // check if we should toggle
      if(currentState == HIGH){
        // it's a button press
        Serial.print("RELEASE ");
      }
      if(currentState == LOW){
        // it's a button release
        Serial.print("PRESS ");
      }
      Serial.println(pin + 0);
      debounce[pin] = 0;
    }
    prevState[pin] = currentState;
  }
}

boolean processMessage(){
  if(!strncmp(buffer, "LED ", 4)){
    if(buffer[4] >= '0' && buffer[4] <= '9' && buffer[5] == ' '){
      char led = buffer[4] - 48;      
      char *cmd = &buffer[6];
      if(!strcmp(cmd, "ON")){
        ledState[led] = ON;
      }else if(!strcmp(cmd, "OFF")){
        ledState[led] = OFF;
      }else if(!strcmp(cmd, "FLASH")){
        ledState[led] = FLASH;
      }else{
        return false;
      }
      return true;
    }
  }
  return false;
}

void handleSerial(){
  if(Serial.available()){
    char incoming = Serial.read();
    if((incoming == '\r' || incoming == '\n')){
      if(bufferPtr != bufferStart){
        //process the message
        *bufferPtr = 0;
        if(processMessage()){
          Serial.print(buffer);
          Serial.println(" OK");
        }else{
          Serial.println("ERROR");
        }
        bufferPtr = bufferStart;
      }
    }else{
      *bufferPtr = incoming;
      bufferPtr++;
    }
  }
}

void handleLeds(){
  for(char led = 0; led < INPUTS; led++){
    if(ledState[led] == OFF){
      digitalWrite(outputs[led], LOW);
    }else if(ledState[led] == ON){
      digitalWrite(outputs[led], HIGH);
    }else if(ledState[led] == FLASH){
      digitalWrite(outputs[led], (millis() / 500) % 2 ? HIGH : LOW);
    }
  }
}

void loop(){
  handleButtons();
  handleSerial();
  handleLeds();
}
