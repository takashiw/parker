int blowerIn = A0;
int bendData;
char publishString[40];
int DELAY = 1000;
int threshold = 40;
int onCountMax = 10;
int onCount = 0;
int offCount = 0;
int offCountMax = 3;


void setup() {
  pinMode(blowerIn, INPUT);
  pinMode(D7, OUTPUT);

}

void loop() {
  bendData = analogRead(blowerIn);
  sprintf(publishString, "%d", bendData);
  /*Particle.publish("Bend Raw", publishString);*/
  if(bendData > threshold){
    onCount++;
  } else {
     offCount++;
  }
  if(onCount > onCountMax){
    offCount = 0;
    onCount = 0;
    Particle.publish("ParkerHeaterFan", "On");
        Particle.publish("Fan Raw ", publishString);
  } else if (offCount > offCountMax){
    offCount = 0;
    onCount = 0;
    Particle.publish("ParkerHeaterFan", "Off");
    Particle.publish("Fan Raw ", publishString);
  }
  delay(DELAY);

}
