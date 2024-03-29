#include <ESPmDNS.h>
#include "WiFi.h"
#include <ESPAsyncWebSrv.h>
#include <Adafruit_Sensor.h>
#include <HCSR04.h>
#include "Adafruit_VL53L0X.h"
#define p_trigger 12
#define p_echo 14
#define PIN_GREEN_LED   17
#define PIN_YELLOW_LED  18
#define PIN_RED_LED     19
#define PIN_BUZZER      5

UltraSonicDistanceSensor distanceSensor(p_trigger, p_echo);  
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

int distancia;
int distancialaser;



#define SOUND_SPEED 0.034
long duration;
float distanceCm;
const char* ssid = "vini";
const char* password = "Vini2005"; 
AsyncWebServer server(80);
String readDistance(){
  digitalWrite(p_trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(p_trigger, LOW);
 
  duration = pulseIn(p_echo, HIGH);
  
  distanceCm = duration * SOUND_SPEED/2;
  Serial.println("Distance:");
  Serial.println(distanceCm);
   return String(distanceCm);
   

}
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>Distancia da Agua para o sensor </h2>
   
  <p>
   <i class="fa fa-tint" aria-hidden="true" style="color:#00add6;"></i> 
    <span class="dht-labels">Distancia</span> 
    <span id="Distance">%DISTANCE%</span>
    <sup class="units">Cm</sup>
  </p>
</body>
<script>  
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("Distance").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/distanceCm", true);
  xhttp.send();
}, 1000 ) ;

</script>
</html>)rawliteral";
String processor(const String& var){
  //Serial.println(var);
   if(var =="DISTANCE"){
   return readDistance();
  }
  return String();
}
void Pinos()
{
  pinMode(PIN_GREEN_LED,  OUTPUT);
  pinMode(PIN_YELLOW_LED, OUTPUT);
  pinMode(PIN_RED_LED,    OUTPUT);
  pinMode(PIN_BUZZER,     OUTPUT);  
}
void setup(){
  
  Serial.begin(115200);
  Pinos();
  pinMode(p_trigger, OUTPUT); 
  pinMode(p_echo, INPUT); 
  
//sensor a laser 
   while (! Serial) {
    delay(1);
  }
  
  Serial.println("Adafruit VL53L0X test");
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }
  // power 
  Serial.println(F("VL53L0X API Simple Ranging example\n\n")); 

  
  // Connect to Wi-Fi
   WiFi.begin(ssid, password);
   while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("connected to wifi ....");
  if(!MDNS.begin("esp32")) {
     Serial.println("Error starting mDNS");
     return;
}
  
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
   server.on("/distanceCm", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readDistance().c_str());
  });


  // Start server
  server.begin();
}

void loop(){

VL53L0X_RangingMeasurementData_t measure;
    
  Serial.print("");
  lox.rangingTest(&measure, false); 

  if (measure.RangeStatus != 4) {  
    Serial.print("Distancia (mm): "); Serial.println(measure.RangeMilliMeter);
  } else {
    Serial.println(" fora de alcance ");
  }
    
  delay(100);



   distancialaser=measure.RangeMilliMeter;
   distancia=distanceSensor.measureDistanceCm();
  
  Serial.println("------------------");
   Serial.print("distancia:");
   Serial.print(distancia);
   Serial.println("Cm");
   Serial.println("------------------");
    delay(1000);
    for(int i=PIN_GREEN_LED; i<=PIN_RED_LED; i++)
  {
    digitalWrite(i, LOW);    
  }

 
  digitalWrite(PIN_BUZZER, LOW);
    

  if( distancialaser <= 50 )
  {
    delay(100);
    digitalWrite(PIN_RED_LED, HIGH); 
    digitalWrite(PIN_BUZZER, HIGH);
  }
  
  else if(distancialaser <80 && distancialaser >50
  )
  {
    delay(300);
    digitalWrite(PIN_YELLOW_LED, HIGH);
    delay(500);
  }
 
  else if(distancialaser > 80 )
  {
    delay(500);
    digitalWrite(PIN_GREEN_LED, HIGH);
   
  }
  
}
