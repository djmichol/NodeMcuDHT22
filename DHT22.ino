
#include <ESP8266WiFi.h>
#include "DHT.h"

const char* ssid = "...";
const char* password = "...";

int ledPin = 13;  //Led D7 - GPIO13
#define DHTPIN 2 // DHT D4 - GPIO2

DHT dht(DHTPIN, DHT22);

WiFiServer server(80);

int WiFiStrength = 0;

void setup() {
  Serial.begin(9600);
  delay(10);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  dht.begin();

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  // Set the ip address of the webserver
  // WiFi.config(WebServerIP, Gatway, Subnet)

  WiFi.config(IPAddress(192, 168, 1, 222), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));

  // connect to WiFi router
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

}

float humidity;
float temp;

void loop() {

  WiFiStrength = WiFi.RSSI(); // get dBm from the ESP8266
  humidity = dht.readHumidity();
  temp = dht.readTemperature();

  // Serial data
  Serial.print("Temperature: ");
  Serial.println(temp);
  Serial.print("Humidity: ");
  Serial.println(humidity);
  Serial.print("WiFi Strength: ");
  Serial.print(WiFiStrength); Serial.println("dBm");
  Serial.println(" ");
  delay(1000); // slows amount of data sent via serial

  // check to for any web server requests. ie - browser requesting a page from the webserver
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");

  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");

  client.println("<html>");
  client.println(" <head>");
  client.println("<meta http-equiv=\"refresh\" content=\"60\">");
  client.println(" <script type=\"text/javascript\" src=\"https://www.gstatic.com/charts/loader.js\"></script>");
  client.println("  <script type=\"text/javascript\">");
  client.println("    google.charts.load('current', {'packages':['gauge']});");
  client.println("    google.charts.setOnLoadCallback(drawChart);");
  client.println("   function drawChart() {");

  client.println("      var data = google.visualization.arrayToDataTable([ ");
  client.println("        ['Label', 'Value'], ");
  client.print("        ['Temperature',  ");
  client.print(temp);
  client.println(" ], ");
  client.println("       ]); ");
  // setup the google chart options here
  client.println("    var options = {");
  client.println("      width: 120, height: 120,");
  client.println("      max: 40, min: 0,");
  client.println("      redFrom: 24, redTo: 40,");
  client.println("      yellowFrom: 0, yellowTo: 18,");
  client.println("      greenFrom: 18, greenTo: 24,");
  client.println("       minorTicks: 2");
  client.println("    };");

  client.println("   var chart = new google.visualization.Gauge(document.getElementById('chart_temp_div'));");
  client.println("  chart.draw(data, options);");
  client.println("  setInterval(function() {");
  client.print("  data.setValue(0, 1, ");
  client.print(temp);
  client.println("    );");
  client.println("    chart.draw(data, options);");
  client.println("    }, 13000);");

  client.println("      var data_hum = google.visualization.arrayToDataTable([ ");
  client.println("        ['Label', 'Value'], ");
  client.print("        ['Humidity',  ");
  client.print(humidity);
  client.println(" ], ");
  client.println("       ]); ");
  // setup the google chart options here
  client.println("    var options_hum = {");
  client.println("      width: 120, height: 120,");
  client.println("      max: 100, min: 0,");
  client.println("      redFrom: 65, redTo: 100,");
  client.println("      yellowFrom: 0, yellowTo: 40,");
  client.println("      greenFrom: 40, greenTo: 65,");
  client.println("       minorTicks: 5");
  client.println("    };");

  client.println("   var chart_hum = new google.visualization.Gauge(document.getElementById('chart_hum_div'));");
  client.println("  chart_hum.draw(data_hum, options_hum);");
  client.println("  setInterval(function() {");
  client.print("  data_hum.setValue(0, 1, ");
  client.print(humidity);
  client.println("    );");
  client.println("    chart_hum.draw(data_hum, options_hum);");
  client.println("    }, 13000);");


  client.println("  }");
  client.println(" </script>");

  client.println("  </head>");
  client.println("  <body>");

  client.print("<h1 style=\"size:12px;\">Temperature and humidity</h1>");

  // show some data on the webpage and the guage
  client.println("<table><tr><td>");

  client.print("WiFi Signal Strength: ");
  client.println(WiFiStrength);
  client.println("dBm<br>");
  client.print("Temperature: ");
  client.println(temp);
  client.print("<br>Humidity: ");
  client.println(humidity);
  client.println("<br><a href=\"/REFRESH\"\"><button>Refresh</button></a>");

  client.println("</td><td>");
  // below is the google temp chart html
  client.println("<div id=\"chart_temp_div\" style=\"width: 150px; height: 120px;\"></div>");
  client.println("</td><td>");
  // below is the google hum chart html
  client.println("<div id=\"chart_hum_div\" style=\"width: 150px; height: 120px;\"></div>");
  client.println("</td></tr></table>");

  client.println("<body>");
  client.println("</html>");
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");


}

