/*************************************************** 
  This is a library for the Adafruit BMP085/BMP180 Barometric Pressure + Temp sensor

  Designed specifically to work with the Adafruit BMP085 or BMP180 Breakout 
  ----> http://www.adafruit.com/products/391
  ----> http://www.adafruit.com/products/1603

  These displays use I2C to communicate, 2 pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/
#include <math.h>

#define boolean bool 
  
#define BMP085_DEBUG 0

#define BMP085_I2CADDR 0x77

#define BMP085_ULTRALOWPOWER 0
#define BMP085_STANDARD      1
#define BMP085_HIGHRES       2
#define BMP085_ULTRAHIGHRES  3
#define BMP085_CAL_AC1           0xAA  // R   Calibration data (16 bits)
#define BMP085_CAL_AC2           0xAC  // R   Calibration data (16 bits)
#define BMP085_CAL_AC3           0xAE  // R   Calibration data (16 bits)    
#define BMP085_CAL_AC4           0xB0  // R   Calibration data (16 bits)
#define BMP085_CAL_AC5           0xB2  // R   Calibration data (16 bits)
#define BMP085_CAL_AC6           0xB4  // R   Calibration data (16 bits)
#define BMP085_CAL_B1            0xB6  // R   Calibration data (16 bits)
#define BMP085_CAL_B2            0xB8  // R   Calibration data (16 bits)
#define BMP085_CAL_MB            0xBA  // R   Calibration data (16 bits)
#define BMP085_CAL_MC            0xBC  // R   Calibration data (16 bits)
#define BMP085_CAL_MD            0xBE  // R   Calibration data (16 bits)

#define BMP085_CONTROL           0xF4 
#define BMP085_TEMPDATA          0xF6
#define BMP085_PRESSUREDATA      0xF6
#define BMP085_READTEMPCMD       0x2E
#define BMP085_READPRESSURECMD   0x34


class Adafruit_BMP085 {
 public:
  Adafruit_BMP085();
  boolean begin(uint8_t mode = BMP085_ULTRAHIGHRES);  // by default go highres
  float readTemperature(void);
  int32_t readPressure(void);
  float readAltitude(float sealevelPressure = 101325); // std atmosphere
  uint16_t readRawTemperature(void);
  uint32_t readRawPressure(void);
  
 private:
  uint8_t read8(uint8_t addr);
  uint16_t read16(uint8_t addr);
  void write8(uint8_t addr, uint8_t data);

  uint8_t oversampling;

  int16_t ac1, ac2, ac3, b1, b2, mb, mc, md;
  uint16_t ac4, ac5, ac6;
};


/*************************************************** 
  This is a library for the Adafruit BMP085/BMP180 Barometric Pressure + Temp sensor

  Designed specifically to work with the Adafruit BMP085 or BMP180 Breakout 
  ----> http://www.adafruit.com/products/391
  ----> http://www.adafruit.com/products/1603

  These displays use I2C to communicate, 2 pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

Adafruit_BMP085::Adafruit_BMP085() {
}


boolean Adafruit_BMP085::begin(uint8_t mode) {
  if (mode > BMP085_ULTRAHIGHRES) 
    mode = BMP085_ULTRAHIGHRES;
  oversampling = mode;

  Wire.begin();

  if (read8(0xD0) != 0x55) return false;

  /* read calibration data */
  ac1 = read16(BMP085_CAL_AC1);
  ac2 = read16(BMP085_CAL_AC2);
  ac3 = read16(BMP085_CAL_AC3);
  ac4 = read16(BMP085_CAL_AC4);
  ac5 = read16(BMP085_CAL_AC5);
  ac6 = read16(BMP085_CAL_AC6);

  b1 = read16(BMP085_CAL_B1);
  b2 = read16(BMP085_CAL_B2);

  mb = read16(BMP085_CAL_MB);
  mc = read16(BMP085_CAL_MC);
  md = read16(BMP085_CAL_MD);
#if (BMP085_DEBUG == 1)
  Serial.print("ac1 = "); Serial.println(ac1, DEC);
  Serial.print("ac2 = "); Serial.println(ac2, DEC);
  Serial.print("ac3 = "); Serial.println(ac3, DEC);
  Serial.print("ac4 = "); Serial.println(ac4, DEC);
  Serial.print("ac5 = "); Serial.println(ac5, DEC);
  Serial.print("ac6 = "); Serial.println(ac6, DEC);

  Serial.print("b1 = "); Serial.println(b1, DEC);
  Serial.print("b2 = "); Serial.println(b2, DEC);

  Serial.print("mb = "); Serial.println(mb, DEC);
  Serial.print("mc = "); Serial.println(mc, DEC);
  Serial.print("md = "); Serial.println(md, DEC);
#endif
  return true;
}

uint16_t Adafruit_BMP085::readRawTemperature(void) {
  write8(BMP085_CONTROL, BMP085_READTEMPCMD);
  delay(5);
#if BMP085_DEBUG == 1
  Serial.print("Raw temp: "); Serial.println(read16(BMP085_TEMPDATA));
#endif
  return read16(BMP085_TEMPDATA);
}

uint32_t Adafruit_BMP085::readRawPressure(void) {
  uint32_t raw;

  write8(BMP085_CONTROL, BMP085_READPRESSURECMD + (oversampling << 6));

  if (oversampling == BMP085_ULTRALOWPOWER) 
    delay(5);
  else if (oversampling == BMP085_STANDARD) 
    delay(8);
  else if (oversampling == BMP085_HIGHRES) 
    delay(14);
  else 
    delay(26);

  raw = read16(BMP085_PRESSUREDATA);

  raw <<= 8;
  raw |= read8(BMP085_PRESSUREDATA+2);
  raw >>= (8 - oversampling);

 /* this pull broke stuff, look at it later?
  if (oversampling==0) {
    raw <<= 8;
    raw |= read8(BMP085_PRESSUREDATA+2);
    raw >>= (8 - oversampling);
  }
 */

#if BMP085_DEBUG == 1
  Serial.print("Raw pressure: "); Serial.println(raw);
#endif
  return raw;
}


int32_t Adafruit_BMP085::readPressure(void) {
  int32_t UT, UP, B3, B5, B6, X1, X2, X3, p;
  uint32_t B4, B7;

  UT = readRawTemperature();
  UP = readRawPressure();

#if BMP085_DEBUG == 1
  // use datasheet numbers!
  UT = 27898;
  UP = 23843;
  ac6 = 23153;
  ac5 = 32757;
  mc = -8711;
  md = 2868;
  b1 = 6190;
  b2 = 4;
  ac3 = -14383;
  ac2 = -72;
  ac1 = 408;
  ac4 = 32741;
  oversampling = 0;
#endif

  // do temperature calculations
  X1=(UT-(int32_t)(ac6))*((int32_t)(ac5))/pow(2,15);
  X2=((int32_t)mc*pow(2,11))/(X1+(int32_t)md);
  B5=X1 + X2;

#if BMP085_DEBUG == 1
  Serial.print("X1 = "); Serial.println(X1);
  Serial.print("X2 = "); Serial.println(X2);
  Serial.print("B5 = "); Serial.println(B5);
#endif

  // do pressure calcs
  B6 = B5 - 4000;
  X1 = ((int32_t)b2 * ( (B6 * B6)>>12 )) >> 11;
  X2 = ((int32_t)ac2 * B6) >> 11;
  X3 = X1 + X2;
  B3 = ((((int32_t)ac1*4 + X3) << oversampling) + 2) / 4;

#if BMP085_DEBUG == 1
  Serial.print("B6 = "); Serial.println(B6);
  Serial.print("X1 = "); Serial.println(X1);
  Serial.print("X2 = "); Serial.println(X2);
  Serial.print("B3 = "); Serial.println(B3);
#endif

  X1 = ((int32_t)ac3 * B6) >> 13;
  X2 = ((int32_t)b1 * ((B6 * B6) >> 12)) >> 16;
  X3 = ((X1 + X2) + 2) >> 2;
  B4 = ((uint32_t)ac4 * (uint32_t)(X3 + 32768)) >> 15;
  B7 = ((uint32_t)UP - B3) * (uint32_t)( 50000UL >> oversampling );

#if BMP085_DEBUG == 1
  Serial.print("X1 = "); Serial.println(X1);
  Serial.print("X2 = "); Serial.println(X2);
  Serial.print("B4 = "); Serial.println(B4);
  Serial.print("B7 = "); Serial.println(B7);
#endif

  if (B7 < 0x80000000) {
    p = (B7 * 2) / B4;
  } else {
    p = (B7 / B4) * 2;
  }
  X1 = (p >> 8) * (p >> 8);
  X1 = (X1 * 3038) >> 16;
  X2 = (-7357 * p) >> 16;

#if BMP085_DEBUG == 1
  Serial.print("p = "); Serial.println(p);
  Serial.print("X1 = "); Serial.println(X1);
  Serial.print("X2 = "); Serial.println(X2);
#endif

  p = p + ((X1 + X2 + (int32_t)3791)>>4);
#if BMP085_DEBUG == 1
  Serial.print("p = "); Serial.println(p);
#endif
  return p;
}


float Adafruit_BMP085::readTemperature(void) {
  int32_t UT, X1, X2, B5;     // following ds convention
  float temp;

  UT = readRawTemperature();

#if BMP085_DEBUG == 1
  // use datasheet numbers!
  UT = 27898;
  ac6 = 23153;
  ac5 = 32757;
  mc = -8711;
  md = 2868;
#endif

  // step 1
  X1 = (UT - (int32_t)ac6) * ((int32_t)ac5) / pow(2,15);
  X2 = ((int32_t)mc * pow(2,11)) / (X1+(int32_t)md);
  B5 = X1 + X2;
  temp = (B5+8)/pow(2,4);
  temp /= 10;
  
  return temp;
}

float Adafruit_BMP085::readAltitude(float sealevelPressure) {
  float altitude;
  float pressure = readPressure();
  altitude = 44330 * (1.0 - pow(pressure /sealevelPressure,0.1903));
  return altitude;
}


/*********************************************************************/

uint8_t Adafruit_BMP085::read8(uint8_t a) {
  uint8_t ret;

  Wire.beginTransmission(BMP085_I2CADDR); // start transmission to device 
  Wire.write(a); // sends register address to read from
  Wire.endTransmission(); // end transmission
  
  Wire.beginTransmission(BMP085_I2CADDR); // start transmission to device 
  Wire.requestFrom(BMP085_I2CADDR, 1);// send data n-bytes read
  ret = Wire.read(); // receive DATA
  Wire.endTransmission(); // end transmission

  return ret;
}

uint16_t Adafruit_BMP085::read16(uint8_t a) {
  uint16_t ret;

  Wire.beginTransmission(BMP085_I2CADDR); // start transmission to device 
  Wire.write(a); // sends register address to read from
  Wire.endTransmission(); // end transmission
  
  Wire.beginTransmission(BMP085_I2CADDR); // start transmission to device 
  Wire.requestFrom(BMP085_I2CADDR, 2);// send data n-bytes read
  ret = Wire.read(); // receive DATA
  ret <<= 8;
  ret |= Wire.read(); // receive DATA
  Wire.endTransmission(); // end transmission

  return ret;
}

void Adafruit_BMP085::write8(uint8_t a, uint8_t d) {
  Wire.beginTransmission(BMP085_I2CADDR); // start transmission to device 
  Wire.write(a); // sends register address to read from
  Wire.write(d);  // write data
  Wire.endTransmission(); // end transmission
}

/*************************************************** 
  This is an example for the BMP085 Barometric Pressure & Temp Sensor

  Designed specifically to work with the Adafruit BMP085 Breakout 
  ----> https://www.adafruit.com/products/391

  These displays use I2C to communicate, 2 pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

// SPARK CORE PINOUT ---------------------------------
// Connect VCC of the BMP085 sensor to 3.3* (NOT Vin!)
// Connect GND to GND
// Connect SCL to i2c clock - on Spark Core this is D1
// Connect SDA to i2c data - on Spark Core this is D0
// EOC is not used, it signifies an end of conversion
// XCLR is a reset pin, also not used here

Adafruit_BMP085 bmp;
  
int buttonUp = D5;
int buttonDown = D6;
int led1 = D7;
int tempChange = 0;
 
int buttonUpState;
int buttonDownState;
char publishString[40];
int DELAY = 375;
int defaultTemp = 72;

int loopCount=0;
  
void setup() {
  pinMode(buttonUp, INPUT);
  pinMode(led1, OUTPUT);    
    
  Serial.begin(9600);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
}
  
void loop() {
  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
    // Serial.print("Real altitude = ");
    // Serial.print(bmp.readAltitude(101500));
    // Serial.println(" meters");
    
    buttonUpState = digitalRead(buttonUp);
    buttonDownState = digitalRead(buttonDown);
    if(buttonUpState == HIGH){
        digitalWrite(led1, HIGH);
        tempChange = tempChange + 1;
    } else if(buttonDownState == HIGH){
        digitalWrite(led1, HIGH);
        tempChange = tempChange - 1;
    } else {
        digitalWrite(led1, LOW);
    }

    if(loopCount > 6){
        double tempFahrenheit = bmp.readTemperature() * 1.8 + 32;
        Particle.publish("ParkerTemperature ", String(tempFahrenheit), 60, PUBLIC);
        Particle.publish("ParkerPressure ", String(bmp.readPressure()), 60, PUBLIC);
        // Calculate altitude assuming 'standard' barometric
        // pressure of 1013.25 millibar = 101325 Pascal
        Particle.publish("ParkerAltitude ", String(bmp.readAltitude()), 60, PUBLIC);
        Particle.publish("ParkerGoalTemp", String(defaultTemp + tempChange));
        loopCount = 0;
    }
    loopCount++;
    delay(DELAY);
}
