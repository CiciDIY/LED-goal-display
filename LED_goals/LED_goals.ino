#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>     // https://github.com/tzapu/WiFiManager
#include <Adafruit_NeoPixel.h>

#define PIN 4
#define NUM_LEDS 120
int red = 1, green = 0;

WiFiClient client;
ESP8266WebServer server(80); // Start server on port 80 (default for a web-browser, change to your requirements, e.g. 8080 if your Router uses port 80
                             // To access server from the outsid of a WiFi network e.g. ESP8266WebServer server(8266); and then add a rule on your Router that forwards a
                             // connection request to http://your_network_ip_address:8266 to port 8266 and view your ESP server from anywhere.
                             // Example http://g6ejd.uk.to:8266 will be directed to http://192.168.0.40:8266 or whatever IP address your router gives to this server
                             
int goal_input, current_input, display_val, colour_step, LED_target,LED_left;
String Argument_Name, Clients_Response1, Clients_Response2;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_LEDS, PIN);

void setup() {
  Serial.begin(115200);
  pixels.begin(); // This initializes the NeoPixel library.
  pixels.show(); // Initialize all pixels to 'off'
  //WiFiManager intialisation. Once completed there is no need to repeat the process on the current board
  WiFiManager wifiManager;
  // New OOB ESP8266 has no Wi-Fi credentials so will connect and not need the next command to be uncommented and compiled in, a used one with incorrect credentials will
  // so restart the ESP8266 and connect your PC to the wireless access point called 'ESP8266_AP' or whatever you call it below in ""
  // wifiManager.resetSettings(); // Command to be included if needed, then connect to http://192.168.4.1/ and follow instructions to make the WiFi connection
  // Set a timeout until configuration is turned off, useful to retry or go to sleep in n-seconds
  wifiManager.setTimeout(180);
  //fetches ssid and password and tries to connect, if connections succeeds it starts an access point with the name called "ESP8266_AP" and waits in a blocking loop for configuration
  if(!wifiManager.autoConnect("ESP8266_AP")) {
    Serial.println("failed to connect and timeout occurred");
    delay(3000);
    ESP.reset(); //reset and try again
    delay(5000);
 
  }
  // At this stage the WiFi manager will have successfully connected to a network, or if not will try again in 180-seconds
  //----------------------------------------------------------------------------------------------------------------------
  Serial.println("WiFi connected..");
  server.begin(); Serial.println("Webserver started..."); // Start the webserver
  Serial.print("Use this URL to connect: http://");// Print the IP address
  Serial.print(WiFi.localIP());Serial.println("/");
  // NOTE: You must use the IP address assigned to YOUR Board when printed/displayed here on your serial port
  // that's the address you must use, not the one I used !
  
  // Next define what the server should do when a client connects
  server.on("/", HandleClient); // The client connected with no arguments e.g. http:192.160.0.40/
  server.on("/result", ShowClientResponse);
}

void HandleClient() {
  String webpage;
  webpage =  "<html>";
   webpage += "<head><title>Monthly goal LED Display</title>";
    webpage += "<style>";
     webpage += "body { background-color: #FFCCCC; font-family: Arial, Helvetica, Sans-Serif; Color: black;}";
    webpage += "</style>";
   webpage += "</head>";
   webpage += "<body>";
    webpage += "<h1><br>Monthly goal LED Display</h1>";  
    String IPaddress = WiFi.localIP().toString();
    webpage += "<form action='http://"+IPaddress+"' method='POST'>";
     webpage += "Please enter month target:  <input type='text' name='goal_income'><BR>";
     webpage += "Please enter current value: <input type='text' name='current_income'>&nbsp;<input type='submit' value='Enter'>"; // omit <input type='submit' value='Enter'> for just 'enter'
    webpage += "</form>";
   webpage += "</body>";
  webpage += "</html>";
  server.send(200, "text/html", webpage); // Send a response to the client asking for input
  if (server.args() > 0 ) { // Arguments were received
    for ( uint8_t i = 0; i < server.args(); i++ ) {
      Serial.print(server.argName(i)); // Display the argument
      Argument_Name = server.argName(i);
      if (server.argName(i) == "goal_income") {
        Serial.print(" Input received was: ");
        Serial.println(server.arg(i));
        Clients_Response1 = server.arg(i);
        goal_input = server.arg(i).toInt();   // use string.toInt()   if you wanted to convert the input to an integer number
        // e.g. range_maximum = server.arg(i).toFloat(); // use string.toFloat() if you wanted to convert the input to a floating point number
      }
      if (server.argName(i) == "current_income") {
        Serial.print(" Input received was: ");
        Serial.println(server.arg(i));
        Clients_Response2 = server.arg(i);
        current_input = server.arg(i).toInt();   // use string.toInt()   if you wanted to convert the input to an integer number
        // e.g. range_maximum = server.arg(i).toFloat(); // use string.toFloat() if you wanted to convert the input to a floating point number
      }
    }
  }
}

void ShowClientResponse() {
  String webpage;
  webpage =  "<html>";
   webpage += "<head><title>Monthly goal display</title>";
    webpage += "<style>";
     webpage += "body { background-color: #FFCCFF; font-family: Arial, Helvetica, Sans-Serif; Color: blue;}";
    webpage += "</style>";
   webpage += "</head>";
   webpage += "<body>";
    webpage += "<h1><br>ESP8266 Server - This was what the client sent</h1>";
    webpage += "<p>Name received was: " + Clients_Response1 + "</p>";
    webpage += "<p>Address received was: " + Clients_Response2 + "</p>";
   webpage += "</body>";
  webpage += "</html>";
  server.send(200, "text/html", webpage); // Send a response to the client asking for input
}

void loop() {
  server.handleClient(); 
  LED_light_up();
  }

void LED_light_up() {
  colour_step = (21/ NUM_LEDS);
  LED_target = (goal_input/NUM_LEDS);
  pixels.setPixelColor(0, pixels.Color(1.0, 0.0, 0.0));
  for (int colour_step, LED_target, LED_left = 0; LED_left = NUM_LEDS ; LED_left++) {
    red = red - colour_step;
    green = green + colour_step;
    if ((LED_target) >=  current_input) {
      pixels.setPixelColor(LED_left, pixels.Color((1),0,0));
    if ((LED_target * LED_left) >= current_input) {
      pixels.setPixelColor(LED_left, pixels.Color((1-colour_step),0,0));
    }
    }
        
  }
}
void rainbow() {
    //display_val = ((current_input/goal_input) * 100);
    //goal_input = (goal_input/10);
    if (current_input > goal_input) {
      pixels.clear();
      pixels.setPixelColor(0, pixels.Color(255,0,0));
      pixels.setPixelColor(1, pixels.Color(255,5,0));
      pixels.setPixelColor(2, pixels.Color(255,20,0));
      pixels.setPixelColor(3, pixels.Color(235,20,0));
      pixels.setPixelColor(4, pixels.Color(235,55,0));
      pixels.setPixelColor(5, pixels.Color(220,50,0));
      pixels.setPixelColor(6, pixels.Color(210,55,0));
      pixels.setPixelColor(7, pixels.Color(170,65,0));
      pixels.setPixelColor(8, pixels.Color(160,75,0));
      pixels.setPixelColor(9, pixels.Color(145,75,0));
      pixels.setPixelColor(10, pixels.Color(100,105,0));
      pixels.setPixelColor(11, pixels.Color(90,120,0));
      pixels.setPixelColor(12, pixels.Color(60,160,0));
      pixels.setPixelColor(13, pixels.Color(40,180,0));
      pixels.setPixelColor(14, pixels.Color(20,200,0));
      pixels.setPixelColor(15, pixels.Color(0,255,0));
      pixels.show();
    }
    if (current_input >= (goal_input * 0.995)) {
      pixels.clear();
      pixels.setPixelColor(0, pixels.Color(255,0,0));
      pixels.setPixelColor(1, pixels.Color(255,5,0));
      pixels.setPixelColor(2, pixels.Color(255,20,0));
      pixels.setPixelColor(3, pixels.Color(235,20,0));
      pixels.setPixelColor(4, pixels.Color(235,55,0));
      pixels.setPixelColor(5, pixels.Color(220,50,0));
      pixels.setPixelColor(6, pixels.Color(210,55,0));
      pixels.setPixelColor(7, pixels.Color(170,65,0));
      pixels.setPixelColor(8, pixels.Color(160,75,0));
      pixels.setPixelColor(9, pixels.Color(145,75,0));
      pixels.setPixelColor(10, pixels.Color(100,105,0));
      pixels.setPixelColor(11, pixels.Color(90,120,0));
      pixels.setPixelColor(12, pixels.Color(60,160,0));
      pixels.setPixelColor(13, pixels.Color(40,180,0));
      pixels.setPixelColor(14, pixels.Color(20,200,0));
      pixels.show();
    }
    if (current_input >= (goal_input * 0.91)) {
      pixels.clear();
      pixels.setPixelColor(0, pixels.Color(255,0,0));
      pixels.setPixelColor(1, pixels.Color(255,5,0));
      pixels.setPixelColor(2, pixels.Color(255,20,0));
      pixels.setPixelColor(3, pixels.Color(235,20,0));
      pixels.setPixelColor(4, pixels.Color(235,55,0));
      pixels.setPixelColor(5, pixels.Color(220,50,0));
      pixels.setPixelColor(6, pixels.Color(210,55,0));
      pixels.setPixelColor(7, pixels.Color(170,65,0));
      pixels.setPixelColor(8, pixels.Color(160,75,0));
      pixels.setPixelColor(9, pixels.Color(145,75,0));
      pixels.setPixelColor(10, pixels.Color(100,105,0));
      pixels.setPixelColor(11, pixels.Color(90,120,0));
      pixels.setPixelColor(12, pixels.Color(60,160,0));
      pixels.setPixelColor(13, pixels.Color(40,180,0));
      pixels.show();
    }
    if (current_input >= (goal_input * 0.83)) {
      pixels.clear();
      pixels.setPixelColor(0, pixels.Color(255,0,0));
      pixels.setPixelColor(1, pixels.Color(255,5,0));
      pixels.setPixelColor(2, pixels.Color(255,20,0));
      pixels.setPixelColor(3, pixels.Color(235,20,0));
      pixels.setPixelColor(4, pixels.Color(235,55,0));
      pixels.setPixelColor(5, pixels.Color(220,50,0));
      pixels.setPixelColor(6, pixels.Color(210,55,0));
      pixels.setPixelColor(7, pixels.Color(170,65,0));
      pixels.setPixelColor(8, pixels.Color(160,75,0));
      pixels.setPixelColor(9, pixels.Color(145,75,0));
      pixels.setPixelColor(10, pixels.Color(100,105,0));
      pixels.setPixelColor(11, pixels.Color(90,120,0));
      pixels.setPixelColor(12, pixels.Color(60,160,0));
      pixels.show();
    }
    if (current_input >= (goal_input * 0.77)) {
      pixels.clear();
      pixels.setPixelColor(0, pixels.Color(255,0,0));
      pixels.setPixelColor(1, pixels.Color(255,5,0));
      pixels.setPixelColor(2, pixels.Color(255,20,0));
      pixels.setPixelColor(3, pixels.Color(235,20,0));
      pixels.setPixelColor(4, pixels.Color(235,55,0));
      pixels.setPixelColor(5, pixels.Color(220,50,0));
      pixels.setPixelColor(6, pixels.Color(210,55,0));
      pixels.setPixelColor(7, pixels.Color(170,65,0));
      pixels.setPixelColor(8, pixels.Color(160,75,0));
      pixels.setPixelColor(9, pixels.Color(145,75,0));
      pixels.setPixelColor(10, pixels.Color(100,105,0));
      pixels.setPixelColor(11, pixels.Color(90,120,0));
      pixels.show();
    }
    if (current_input >= (goal_input * 0.70)) {
      pixels.clear();
      pixels.setPixelColor(0, pixels.Color(255,0,0));
      pixels.setPixelColor(1, pixels.Color(255,5,0));
      pixels.setPixelColor(2, pixels.Color(255,20,0));
      pixels.setPixelColor(3, pixels.Color(235,20,0));
      pixels.setPixelColor(4, pixels.Color(235,55,0));
      pixels.setPixelColor(5, pixels.Color(220,50,0));
      pixels.setPixelColor(6, pixels.Color(210,55,0));
      pixels.setPixelColor(7, pixels.Color(170,65,0));
      pixels.setPixelColor(8, pixels.Color(160,75,0));
      pixels.setPixelColor(9, pixels.Color(145,75,0));
      pixels.setPixelColor(10, pixels.Color(100,105,0));
      pixels.show();
    }
    if (current_input >= (goal_input * 0.63)) {
      pixels.clear();
      pixels.setPixelColor(0, pixels.Color(255,0,0));
      pixels.setPixelColor(1, pixels.Color(255,5,0));
      pixels.setPixelColor(2, pixels.Color(255,20,0));
      pixels.setPixelColor(3, pixels.Color(235,20,0));
      pixels.setPixelColor(4, pixels.Color(235,55,0));
      pixels.setPixelColor(5, pixels.Color(220,50,0));
      pixels.setPixelColor(6, pixels.Color(210,55,0));
      pixels.setPixelColor(7, pixels.Color(170,65,0));
      pixels.setPixelColor(8, pixels.Color(160,75,0));
      pixels.setPixelColor(9, pixels.Color(145,75,0));
      pixels.show();
    }
    if (current_input >= (goal_input * 0.57)) {
      pixels.clear();
      pixels.setPixelColor(0, pixels.Color(255,0,0));
      pixels.setPixelColor(1, pixels.Color(255,5,0));
      pixels.setPixelColor(2, pixels.Color(255,20,0));
      pixels.setPixelColor(3, pixels.Color(235,20,0));
      pixels.setPixelColor(4, pixels.Color(235,55,0));
      pixels.setPixelColor(5, pixels.Color(220,50,0));
      pixels.setPixelColor(6, pixels.Color(210,55,0));
      pixels.setPixelColor(7, pixels.Color(170,65,0));
      pixels.setPixelColor(8, pixels.Color(160,75,0));
    }
    if (current_input >= (goal_input * 0.50)) {
      pixels.clear();
      pixels.setPixelColor(0, pixels.Color(255,0,0));
      pixels.setPixelColor(1, pixels.Color(255,5,0));
      pixels.setPixelColor(2, pixels.Color(255,20,0));
      pixels.setPixelColor(3, pixels.Color(235,20,0));
      pixels.setPixelColor(4, pixels.Color(235,55,0));
      pixels.setPixelColor(5, pixels.Color(220,50,0));
      pixels.setPixelColor(6, pixels.Color(210,55,0));
      pixels.setPixelColor(7, pixels.Color(170,65,0));
      pixels.show();
    }
    if (current_input >= (goal_input * 0.44)) {
      pixels.clear();
      pixels.setPixelColor(0, pixels.Color(255,0,0));
      pixels.setPixelColor(1, pixels.Color(255,5,0));
      pixels.setPixelColor(2, pixels.Color(255,20,0));
      pixels.setPixelColor(3, pixels.Color(235,20,0));
      pixels.setPixelColor(4, pixels.Color(235,55,0));
      pixels.setPixelColor(5, pixels.Color(220,50,0));
      pixels.setPixelColor(6, pixels.Color(210,55,0));
      pixels.show();
    }
    if (current_input >= (goal_input * 0.37)) {
      pixels.clear();
      pixels.setPixelColor(0, pixels.Color(255,0,0));
      pixels.setPixelColor(1, pixels.Color(255,5,0));
      pixels.setPixelColor(2, pixels.Color(255,20,0));
      pixels.setPixelColor(3, pixels.Color(235,20,0));
      pixels.setPixelColor(4, pixels.Color(235,55,0));
      pixels.setPixelColor(5, pixels.Color(220,50,0));
      pixels.show();
    }
        if (current_input >= (goal_input * 0.31)) {
      pixels.clear();
      pixels.setPixelColor(0, pixels.Color(255,0,0));
      pixels.setPixelColor(1, pixels.Color(255,5,0));
      pixels.setPixelColor(2, pixels.Color(255,20,0));
      pixels.setPixelColor(3, pixels.Color(235,20,0));
      pixels.setPixelColor(4, pixels.Color(235,55,0));
      pixels.show();
    }
    if (current_input >= (goal_input * 0.31)) {
      pixels.clear();
      pixels.setPixelColor(0, pixels.Color(255,0,0));
      pixels.setPixelColor(1, pixels.Color(255,5,0));
      pixels.setPixelColor(2, pixels.Color(255,20,0));
      pixels.setPixelColor(3, pixels.Color(235,20,0));
      pixels.show();
    }
    if (current_input >= (goal_input * 0.24)) {
      pixels.clear();
      pixels.setPixelColor(0, pixels.Color(255,0,0));
      pixels.setPixelColor(1, pixels.Color(255,5,0));
      pixels.setPixelColor(2, pixels.Color(255,20,0));
      pixels.show();
    }
    if (current_input >= (goal_input * 0.17)) {
      pixels.clear();
      pixels.setPixelColor(0, pixels.Color(255,0,0));
      pixels.setPixelColor(1, pixels.Color(255,5,0));
      pixels.show();
    if (current_input >= (goal_input * 0.10)) {
      pixels.clear();
      pixels.setPixelColor(0, pixels.Color(255,0,0));
      pixels.show();
    }
  }
}

