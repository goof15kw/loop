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

float m=-0.5;
float b=55.0;

// Data wire is plugged into pin 3 on the Arduino
#define ONE_WIRE_BUS 3
#define BROCHE_RELAI 4
#define LED 13
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

// valeur lue des senseurs
float tempEau=45;
float tempExt=21;

// hysteresis, +- combien on tolere autour de la cible
float tolerance=1; 
//pour que la premiere loop laisse le circuit ferme (chauffe).
float tempCible=tempEau-(2 * tolerance) ; 

// compteur d'erreurs
int lectureRateEau=0;
int lectureRateExt=0;

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
  digitalWrite(BROCHE_RELAI,CHAUFFE);  
  pinMode(LED,OUTPUT);
  digitalWrite(LED,HIGH);
}

void printTemperature(float tempC)
{
    Serial.print(tempC);
    Serial.print("C");      
}

// retourne 1 si le param est une temperature
int temperatureValide(float tempf)
{
  float tempInvalide[]={-127.00, 0.00 , 85.00}; 
  float ttolerence=0.01;

  int i=0;
  for (i=0; i<3;i++) 
  {
    if(abs(tempf-tempInvalide[i]) <= ttolerence ) {return 0; }
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
	flashLaLumiere(3); 
   } else { 
	Serial.print("Chauffe pas(delta[");
 	digitalWrite(BROCHE_RELAI,CHAUFFE_PAS);
	flashLaLumiere(2);
    }
  } else { 
     Serial.print("Touche pas (delta[");
     flashLaLumiere(1);
  }
  Serial.print(delta);
  Serial.print("] Cible [");
  Serial.print(tempCible);
  Serial.print("C]) Erreur Eau[");
  Serial.print(lectureRateEau);
  Serial.print("] Erreur Ext[");
  Serial.print(lectureRateExt);
  Serial.print("]");
}

void calculeCible()
{
    tempCible=m*tempExt+b; 
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
	lectureRateEau=0;
  } else { lectureRateEau++; }

  temp=sensors.getTempC(thermometreExterieur);
  if ( temperatureValide(temp) ) {
	tempExt=temp;
	lectureRateExt=0;
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

void flashLaLumiere(int nb)
{
  for (int i=0;i<nb;i++)
  {
    digitalWrite(LED,LOW);
    delay(150);
    digitalWrite(LED,HIGH);
    delay(150);
  }
}
