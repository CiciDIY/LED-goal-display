#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>

#define PIN 4

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
/* RGB web server with ESP8266-01
* only 2 GPIOs available: 0 and 2
* but RX and TX can also be used as: 3 and 1
* we use 0=red 2=green 3=blue
* analogWrite with values received from web page
*
* web server with captive portal works but better use fixed domain: http://rgb
* web page returns POST request with 3 RGB parameters
* web page inspired by https://github.com/dimsumlabs/nodemcu-httpd
* Serial Monitor for debugging but interferes with Blue channel GPIO3=RX
* switch off Serial for full RGB
*/

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

const char *ssid = "RGB";
// const char *password = "87654321";

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;
ESP8266WebServer webServer(80);

String webpage = ""
"<!DOCTYPE html><html><head><title>RGB control</title><meta name='mobile-web-app-capable' content='yes' />"
"<meta name='viewport' content='width=device-width' /></head><body style='margin: 0px; padding: 0px;'>"
"<canvas id='colorspace'></canvas></body>"
"<script type='text/javascript'>"
"(function () {"
" var canvas = document.getElementById('colorspace');"
" var ctx = canvas.getContext('2d');"
" function drawCanvas() {"
" var colours = ctx.createLinearGradient(0, 0, window.innerWidth, 0);"
" for(var i=0; i <= 360; i+=10) {"
" colours.addColorStop(i/360, 'hsl(' + i + ', 100%, 50%)');"
" }"
" ctx.fillStyle = colours;"
" ctx.fillRect(0, 0, window.innerWidth, window.innerHeight);"
" var luminance = ctx.createLinearGradient(0, 0, 0, ctx.canvas.height);"
" luminance.addColorStop(0, '#ffffff');"
" luminance.addColorStop(0.05, '#ffffff');"
" luminance.addColorStop(0.5, 'rgba(0,0,0,0)');"
" luminance.addColorStop(0.95, '#000000');"
" luminance.addColorStop(1, '#000000');"
" ctx.fillStyle = luminance;"
" ctx.fillRect(0, 0, ctx.canvas.width, ctx.canvas.height);"
" }"
" var eventLocked = false;"
" function handleEvent(clientX, clientY) {"
" if(eventLocked) {"
" return;"
" }"
" function colourCorrect(v) {"
" return Math.round(1023-(v*v)/64);"
" }"
" var data = ctx.getImageData(clientX, clientY, 1, 1).data;"
" var params = ["
" 'r=' + colourCorrect(data[0]),"
" 'g=' + colourCorrect(data[1]),"
" 'b=' + colourCorrect(data[2])"
" ].join('&');"
" var req = new XMLHttpRequest();"
" req.open('POST', '?' + params, true);"
" req.send();"
" eventLocked = true;"
" req.onreadystatechange = function() {"
" if(req.readyState == 4) {"
" eventLocked = false;"
" }"
" }"
" }"
" canvas.addEventListener('click', function(event) {"
" handleEvent(event.clientX, event.clientY, true);"
" }, false);"
" canvas.addEventListener('touchmove', function(event){"
" handleEvent(event.touches[0].clientX, event.touches[0].clientY);"
"}, false);"
" function resizeCanvas() {"
" canvas.width = window.innerWidth;"
" canvas.height = window.innerHeight;"
" drawCanvas();"
" }"
" window.addEventListener('resize', resizeCanvas, false);"
" resizeCanvas();"
" drawCanvas();"
" document.ontouchmove = function(e) {e.preventDefault()};"
" })();"
"</script></html>";

//////////////////////////////////////////////////////////////////////////////////////////////////

void handleRoot() {
// Serial.println("handle root..");
String red = webServer.arg(0); // read RGB arguments
String green = webServer.arg(1);
String blue = webServer.arg(2);

analogWrite(0, red.toInt());
analogWrite(2, green.toInt());
analogWrite(3, blue.toInt());

// Serial.println(red.toInt()); // for TESTING
// Serial.println(green.toInt()); // for TESTING
// Serial.println(blue.toInt()); // for TESTING
webServer.send(200, "text/html", webpage);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {

pinMode(0, OUTPUT);
pinMode(2, OUTPUT);
pinMode(3, OUTPUT);

analogWrite(0, 1023);
analogWrite(2, 1023);
analogWrite(3, 1023);

delay(1000);
// Serial.begin(9600);
// Serial.println();

WiFi.mode(WIFI_AP);
WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
WiFi.softAP(ssid);

// if DNSServer is started with "*" for domain name, it will reply with provided IP to all DNS request
dnsServer.start(DNS_PORT, "rgb", apIP);

webServer.on("/", handleRoot);

webServer.begin();

testRGB();
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {

dnsServer.processNextRequest();
webServer.handleClient();

}
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

void testRGB() { // fade in and out of Red, Green, Blue

analogWrite(0, 1023); // R off
analogWrite(2, 1023); // G off
analogWrite(3, 1023); // B off

fade(0); // R
fade(2); // G
fade(3); // B
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void fade(int pin) {

for (int u = 0; u < 1024; u++) {
analogWrite(pin, 1023 - u);
delay(1);
}
for (int u = 0; u < 1024; u++) {
analogWrite(pin, u);
delay(1);
}
}
