// Load Wi-Fi library
  #include <WiFi.h>
  #include <Arduino.h>
  
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
  const int output14 = 14;
  const int output12 = 12;
  const int output27 = 27;

  String output14State = "off";
  String output12State = "off";
  String output13State = "off";
  String output27State = "off";
  bool warning;
  int value;
    
void setup() {
  
  Serial.begin(9600);
  
  // Initialisation des variables
  pinMode(ledPin, OUTPUT);
  pinMode(output14, OUTPUT);
  pinMode(output12, OUTPUT);
  pinMode(output27, OUTPUT);
  pinMode(pResistor, INPUT);
  
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

 value = analogRead(pResistor);
//Valeur de seuil à définir
  if (value < 2000 ){
    digitalWrite(ledPin, LOW);  //Turn led off
    output13State = "off";
  }else{
    digitalWrite(ledPin, HIGH); //Turn led on
    output13State = "on";
  }
  delay(500);
  
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
              
// Affichage du code HTML
  client.println("<!DOCTYPE html><html>");
  client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  client.println("<link rel=\"icon\" href=\"data:,\">");
  // CSS to style the on/off buttons 
  // Feel free to change the background-color and font-size attributes to fit your preferences
  client.println("<style>html { font-family: Arial; color: #FFFFFF; background-color : #141d26  ; display: inline-block; margin: 0px auto; text-align: center;}");
  client.println(".button { padding:6px 0 6px 0;font:Bold 13px Arial;background:#478bf9;color:#fff;width:90px;border-radius:2px;border:none;");
  client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
  client.println(".button2 {background-color: #555555;}");
  client.println(".button3 {background-color: #d34836;  height : auto ; width : 190 px;}</style></head>");           
// En tête de page
  client.println("<body><h1>Lampe connectee</h1>");
client.println("Intensite lumineuse :");
  client.println(value);

client.println("<p><a href=\"http://192.168.43.110\"><button class=\"button button3\"> F5 </button></a></p>");
  
// Affichage de l'état de la LED témoin       
  if (output13State == "off") {
    client.println("<p>La lumiere est eteinte</p>");
  } else {
    client.println("<p>La lumiere est allumee</p>");
    } 

  if (header.indexOf("GET /12/on") >= 0) {
              Serial.println("GPIO 12 on");
              output12State = "on";
              digitalWrite(output12, HIGH);
            } else if (header.indexOf("GET /12/off") >= 0) {
              Serial.println("GPIO 12 off");
              output12State = "off";
              digitalWrite(output12, LOW);
            }

  if (header.indexOf("GET /14/on") >= 0) {
    Serial.println("GPIO 14 on");
    output14State = "on";
    output27State = "on";
    digitalWrite(output14, HIGH);
    digitalWrite(output27, HIGH);
  } else if (header.indexOf("GET /14/off") >= 0) {
    Serial.println("GPIO 14 off");
    output14State = "off";
    output27State = "off";
    digitalWrite(output14, LOW);
    digitalWrite(output27, LOW);
  }

  
    client.println("<p>Affichage de l'avertissement : " + output14State + "</p>");
            // If the output27State is off, it displays the ON button       
            if (output14State=="off") {
              client.println("<p><a href=\"/14/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/14/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

  client.println("<p>Autoriser l'allumage ? " + output12State + "</p>");
            // If the output27State is off, it displays the ON button       
            if (output12State=="off") {
              client.println("<p><a href=\"/12/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/12/off\"><button class=\"button button2\">OFF</button></a></p>");
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
