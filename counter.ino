int buttonUp = D5;
int buttonDown = D6;
int led1 = D7;
int tempChange = 0;

int buttonUpState;
int buttonDownState;
char publishString[40];
int DELAY = 375;
int defaultTemp = 75;


void setup() {

  pinMode(buttonUp, INPUT);
  pinMode(led1, OUTPUT);

}

void loop() {
  buttonUpState = digitalRead(buttonUp);
  buttonDownState = digitalRead(buttonDown);
  if(buttonUpState == HIGH){
    digitalWrite(led1, HIGH);
    tempChange = tempChange + 1;
    sprintf(publishString, "%d", defaultTemp + tempChange);
    Particle.publish("Parker Temp", publishString);
    delay(DELAY);
  } else if(buttonDownState == HIGH){
    digitalWrite(led1, HIGH);
    tempChange = tempChange - 1;
    sprintf(publishString, "%d", defaultTemp + tempChange);
    Particle.publish("Parker Temp", publishString);
    delay(DELAY);
  } else {
    digitalWrite(led1, LOW);
  }
  /*Particle.publish("WOW");*/
}
