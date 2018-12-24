// Load Wi-Fi library
  #include <WiFi.h>

// Replace with your network credentials
  const char* ssid     = "TEST123";
  const char* password = "11111111";

// Set web server port number to 80
  WiFiServer server(80);

// Variable to store the HTTP request
  String header;

// Assign output variables to GPIO pins
  const int ledPin = 13; //Pin de la LED témoin
  const int pResistor = 37; //Pin de la Photorésistance
  const int relais = 12; //Pin du relais
  const int alarme = 14; //Pin alarme
  int value; //Valeur de l'intensité lumineuse

  String outputRelaisState = "off";
  int alarmeState = LOW;
  
// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
  unsigned long previousMillis = 0;        // will store last time LED was updated
  unsigned long previousMillis2 = 0;
  // constants won't change:
  const long interval = 500;           // interval at which to blink (milliseconds)
  const long interval2 = 1000;
  
void setup() {
  
  Serial.begin(115200);
  
  // Initialisation des variables
  pinMode(ledPin, OUTPUT);
  pinMode(relais, OUTPUT);
  pinMode(alarme, OUTPUT);
  pinMode(pResistor, INPUT);

  digitalWrite(relais,LOW);
  
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){

unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
// On prend la valeur de la photorésistance
  value = analogRead(pResistor);
  
//Valeur de seuil à définir
  if (value < 10){
    digitalWrite(ledPin, LOW);  //Turn led off
    digitalWrite(alarme, LOW);
  }else{
    digitalWrite(ledPin, HIGH); //Turn led on
    
    }
  }

  if (value >= 10){
      if (currentMillis - previousMillis2 >= interval2) {
    // save the last time you blinked the LED
    previousMillis2 = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (alarmeState == LOW) {
      alarmeState = HIGH;
    } else {
      alarmeState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(alarme, alarmeState);
  }
  }
  
WiFiClient client = server.available();   // Listen for incoming clients

if (client) {                             // If a new client connects,
  Serial.println("New Client.");          // print a message out in the serial port
  String currentLine = "";                // make a String to hold incoming data from the client
  while (client.connected()) {            // loop while the client's connected
    if (client.available()) {             // if there's bytes to read from the client,
      char c = client.read();             // read a byte, then
      Serial.write(c);                    // print it out the serial monitor
      header += c;
      if (c == '\n') {                    // if the byte is a newline character
        // if the current line is blank, you got two newline characters in a row.
        // that's the end of the client HTTP request, so send a response:
        if (currentLine.length() == 0) {
          // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
          // and a content-type so the client knows what's coming, then a blank line:
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println("Connection: close");
          client.println();

 // turns the GPIOs on and off
 if (header.indexOf("GET /26/on") >= 0) {
     Serial.println("GPIO 26 on");
     outputRelaisState = "on";
     digitalWrite(relais, HIGH);
 } else if (header.indexOf("GET /26/off") >= 0) {
     Serial.println("GPIO 26 off");
     outputRelaisState = "off";
     digitalWrite(relais, LOW);
 }
              
// Affichage du code HTML
  client.println("<!DOCTYPE html><html>");
  client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  client.println("<link rel=\"icon\" href=\"data:,\">");
  // CSS to style the on/off buttons 
  // Feel free to change the background-color and font-size attributes to fit your preferences
  client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
  client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
  client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
  client.println(".button2 {background-color: #555555;}</style></head>");
            
// En tête de page
  client.println("<body><h1>ESP32 Web Server</h1>");
            
// Affichage de l'état de la LED témoin  
  client.println("<p>GPIO 26 - State </p>");       
  if (value <10) {
    client.println("<p>La lumiere est eteinte</p>");
  } else {
    client.println("<p>La lumiere est allumee</p>");
    } 

// Display current state, and ON/OFF buttons for GPIO relais  
  client.println("<p>GPIO 26 - State " + outputRelaisState + "</p>");
            // If the output26State is off, it displays the ON button       
  if (outputRelaisState=="off") {
    client.println("<p><a href=\"/26/on\"><button class=\"button\">ON</button></a></p>");
  } else {
    client.println("<p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p>");
    } 
                   
  client.println("</body></html>");
            
// The HTTP response ends with another blank line
  client.println();
  // Break out of the while loop
  break;
  } else { // if you got a newline, then clear currentLine
    currentLine = "";
    }
  } else if (c != '\r') {  // if you got anything else but a carriage return character,
    currentLine += c;      // add it to the end of the currentLine
    }
  }
}

// Clear the header variable
header = "";
// Close the connection
client.stop();
Serial.println("Client disconnected.");
Serial.println("");
}
}
