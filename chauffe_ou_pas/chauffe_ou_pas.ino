#include <OneWire.h>
#include <DallasTemperature.h>

// Voir pente.gnumeric
//Text	Teau					Text	Teau			
//-20	65				p1	-20	65			
//-18	62,75				p2	20	20			m=(x2-x1)/(y2-y1)
//-16	60,5									*-1,125*
//-10	53,75									
//-5	48,125									y=mx+b
//0	42,5									b=y-mx
//5	36,875									*42,5*
//10	31,25									
//15	25,625									
//20	20									

float m=-1.125;
float b=42.5;

// Data wire is plugged into pin 3 on the Arduino
#define ONE_WIRE_BUS 3
#define BROCHE_RELAI 4
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// Assign the addresses of your 1-Wire temp sensors.
// See the tutorial on how to obtain these addresses:
// http://www.hacktronics.com/Tutorials/arduino-1-wire-address-finder.html

DeviceAddress thermometreExterieur = { 0x28, 0x75, 0x5F, 0x14, 0x04, 0x00, 0x00, 0x3F };
DeviceAddress thermometreEau = { 0x28, 0x8C, 0xDF, 0x07, 0x04, 0x00, 0x00, 0xA0 };

#define CHAUFFE LOW
#define CHAUFFE_PAS HIGH

float tempEau=45;
float tempExt=21;

float tempCible=45;
float tolerance=1; // hysteresis, combien on tolere autour de la cible

void setup(void)
{
  // start serial port
  Serial.begin(9600);
  // Start up the library
  sensors.begin();
  // set the resolution to 12 bit (fast enough?)
  sensors.setResolution(thermometreExterieur, 12);
  sensors.setResolution(thermometreEau, 12);

  pinMode(BROCHE_RELAI, OUTPUT);
  digitalWrite(BROCHE_RELAI,CHAUFFE_PAS);  
}

void printTemperature(float tempC)
{
//  float tempC = sensors.getTempC(deviceAddress);
  if (tempC == -127.00 || tempC == 0.00 || tempC == 85.00 ) {
    Serial.print("Error getting temperature [");
    Serial.print(tempC);
    Serial.print("C]");
  } else {
    Serial.print(tempC);
    Serial.print("C");    
  }
}

// retourne 1 si le param est une temperature
int temperatureValide(float tempe)
{
  if (tempe == -127.00 || tempe == 0.00 || tempe == 85.00 ) {
  //   Serial.print("Error getting temperature [");
  //   Serial.print(tempC);
  //   Serial.print("C]");
	return 0; 
  }
  return 1;
}

void verifieTempEau()
{ 
  // si tempEau < cible 
  //  34 - 45 = -15 
  float delta=tempEau - tempCible;
  if (abs(delta) > tolerance)
  {
    if (delta < 0.0 ) { 
	Serial.print("Chauffe (delta[");
	digitalWrite(BROCHE_RELAI,CHAUFFE);
    } else { 
	Serial.print("Chauffe pas(delta[");
 	digitalWrite(BROCHE_RELAI,CHAUFFE_PAS);
    }
  } else { 
     Serial.print("Touche pas (delta[");
     Serial.print(delta);
     Serial.print("])");
  }
  Serial.print(delta);
  Serial.print("] Cible [");
  Serial.print(tempCible);
  Serial.print("C])");
}

int lectureRateEau=0;
int lectureRateExt=0;

void calculeCible()
{
    tempCible=m*tempExt+b; 
//  Serial.print("Cible: ");
//	Serial.print(cible);
//	Serial.print("C\n\r");
}

void loop(void)
{ 
  float temp=0;
  delay(2000);
  Serial.print("Getting temperatures...\n\r");
  sensors.requestTemperatures();
  
  temp=sensors.getTempC(thermometreEau);
  if ( temperatureValide(temp) ) {
	tempEau=temp;
  } else { lectureRateEau++; }

  temp=sensors.getTempC(thermometreExterieur);
  if ( temperatureValide(temp) ) {
	tempExt=temp;
  }else { lectureRateExt++; }
  
  Serial.print("Temperature eau: ");
  printTemperature(tempEau);
  Serial.print("\n\r");
  Serial.print("Temprature extrieure: ");
  printTemperature(tempExt);
  Serial.print("\n\r");

  calculeCible();  
  verifieTempEau();

  
}
