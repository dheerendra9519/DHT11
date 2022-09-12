#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>
#include "MAX30100.h"

// Replace with your network credentials
const char* ssid = "JioFi3_31F18B";
const char* password = "z9e7v7mpp7";

#define DHTPIN D5     // Digital pin D1 connected to the DHT sensor

// Uncomment the type of sensor in use:
#define DHTTYPE    DHT11     // DHT 11
//#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);



// current temperature & humidity, updated in loop()
float t = 0.0;
float h = 0.0;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;    // will store last time DHT was updated

// Updates DHT readings every 10 seconds
const long interval = 10000;  

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
    body {
  //background-color: green;
  background-image: url('https://www.mumbailive.com/images/media/images/images_1558164704380_temperature.jpg?bg=db9131&crop=645%2C362.10526315789474%2C0%2C37.6824413976117&fit=crop&fitToScale=w%2C1368%2C768&h=432.2807017543859&height=400&w=770&width=645');
}
    h2 { font-size: 3.0rem;
        font-family: verdana;
        color: blue;
        border: 5px;
        border-style: outset;
        border-color: #f542e3;
         border-radius: 50px;
        padding: -1px  55px;
         margin: 25px 300px ;
          }
          h3 {
            font-size: 5.0rem;
            border: 10px MediumSeaGreen;
            border-style: ridge ; 
            margin: 25px 185px ;
            }
    p { font-size: 3.0rem;
         font-family: fantasy;
         color: lightgoldenrodyellow;
         border: 13px solid black;
         border-radius: 25px;
         padding: -1px  55px;
         margin: 15px 125px ;
         
         }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
  
    }
  </style>
</head>
</html>
<body>
  <h3>Digital Thermometers with ESP8266 DHT Server </h3>
  <h2> Completed by  </h2>
  <h1> Dheerendra </h1>
  <h1> Puroo </h1>
  <h1> Shrey </h1>
  <h1> Tanmay </h1>
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="dht-labels">Temperature</span> 
    <span id="temperature">%TEMPERATURE%</span>
    <sup class="units"> °C</sup>
  </p>
  <p>
    <i class="fas fa-tint" style="color:#00add6;"></i> 
    <span class="dht-labels">Humidity</span>
    <span id="humidity">%HUMIDITY%</span>
    <sup class="units">%</sup>
  </p>
 <div>
         <i class="fas fa-location-arrow" style = "font-size:1.0rem;color:black;"></i>
         <span style="font-size:1.0rem;"> Indra nagar Orai </span>
    </div>
  <h4>
    <i class="fab fa-youtube" style="font-size:1.0rem;color:red;"></i>
    <span style="font-size:1.0rem;">Subscribe to </span>
    <a href="https://www.youtube.com/channel/UCXLeFwLPd1T_RfeUiKoVopQ" target="_blank" style="font-size:1.0rem;">Arduino project  YouTube Channel {TECHWARRIORS}</a>
  </h4>
  <div class="mapouter">
  <div class="gmap_canvas">
  <iframe width="133" height="147" id="gmap_canvas" src="https://maps.google.com/maps?q=orai&t=&z=13&ie=UTF8&iwloc=&output=embed" frameborder="0" scrolling="no" marginheight="0" marginwidth="0"></iframe>
  <a href="https://123movies-to.org">123movies</a>
  <br>
  <style>.mapouter{position:relative;text-align:right;height:147px;width:133px;}
  </style>
  <a href="https://www.embedgooglemap.net">google maps insert</a><style>.gmap_canvas {overflow:hidden;background:none!important;height:147px;width:133px;}
  </style>
  </div></div>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/humidity", true);
  xhttp.send();
}, 10000 ) ;
</script>
</html>)rawliteral";

// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMPERATURE"){
    return String(t);
  }
  else if(var == "HUMIDITY"){
    return String(h);
  }
  return String();
}

void setup(){
  
  
  Serial.begin(115200);
  dht.begin();
  
  
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");
  }

  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(t).c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(h).c_str());
  });

  // Start server
  server.begin();
}
 
void loop(){  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you updated the DHT values
    previousMillis = currentMillis;
    // Read temperature as Celsius (the default)
    float newT = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    //float newT = dht.readTemperature(true);
    // if temperature read failed, don't change t value
    if (isnan(newT)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      t = newT;
      Serial.println(t);
      Serial.println("'c");
    }
    // Read Humidity
    float newH = dht.readHumidity();
    // if humidity read failed, don't change h value 
    if (isnan(newH)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      h = newH;
      Serial.println(h);
      Serial.println(" %");
    }
  }
}
