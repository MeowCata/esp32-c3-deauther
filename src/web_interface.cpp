#include <WebServer.h>
#include "web_interface.h"
#include "definitions.h"
#include "deauth.h"
#include "esp_adc_cal.h"

WebServer server(80);
int num_networks;

float readTemperature() {
  return temperatureRead();
}

String getEncryptionType(wifi_auth_mode_t encryptionType);

void redirect_root() {
  server.sendHeader("Location", "/");
  server.send(301);
}

void handle_root() {
  float temperature = readTemperature(); 

  String html = R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32-Deauther</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            line-height: 1.6;
            color: #333;
            max-width: 800px;
            margin: 0 auto;
            padding: 20px;
            background-color: #f4f4f4;
        }
        h1, h2 {
            color: #2c3e50;
        }
        table {
            width: 100%;
            border-collapse: collapse;
            margin-bottom: 20px;
        }
        th, td {
            padding: 12px;
            text-align: left;
            border-bottom: 1px solid #ddd;
        }
        th {
            background-color: #3498db;
            color: white;
        }
        tr:nth-child(even) {
            background-color: #f2f2f2;
        }
        form {
            background-color: white;
            padding: 20px;
            border-radius: 5px;
            box-shadow: 0 2px 5px rgba(0,0,0,0.1);
            margin-bottom: 20px;
        }
        input[type="text"], input[type="submit"] {
            width: 100%;
            padding: 10px;
            margin-bottom: 10px;
            border: 1px solid #ddd;
            border-radius: 4px;
        }
        input[type="submit"] {
            background-color: #3498db;
            color: white;
            border: none;
            cursor: pointer;
            transition: background-color 0.3s;
        }
        input[type="submit"]:hover {
            background-color: #2980b9;
        }
    </style>
</head>
<body>
    <h1>Wireless Deauth</h1>
    <h2>ChipTemp: )" + String(temperature, 1) + R"( °C</h2>

    <h2>WiFi Networks</h2>
    <table>
        <tr>
            <th>Number</th>
            <th>SSID</th>
            <th>BSSID</th>
            <th>Channel</th>
            <th>Range</th>
            <th>Encryption</th>
        </tr>
)";

  for (int i = 0; i < num_networks; i++) {
    String encryption = getEncryptionType(WiFi.encryptionType(i));
    html += "<tr><td>" + String(i) + "</td><td>" + WiFi.SSID(i) + "</td><td>" + WiFi.BSSIDstr(i) + "</td><td>" + 
            String(WiFi.channel(i)) + "</td><td>" + String(WiFi.RSSI(i)) + "</td><td>" + encryption + "</td></tr>";
  }

  html += R"(
    </table>

    <form method="post" action="/rescan">
        <input type="submit" value="Rescan networks">
    </form>

    <form method="post" action="/deauth">
        <h2>Launch Deauth-Attack</h2>
        <input type="text" name="net_num" placeholder="Network Number">
        <input type="submit" value="Launch Attack">
    </form>

    <p>Eliminated devices: )" + String(eliminated_stations) + R"(</p>

    <form method="post" action="/stop">
        <input type="submit" value="Stop Deauth-Attack">
    </form>
    <form method="post" action="/deauth_all">
        <input type="submit" value="DEAUTH ALL">
    </form>
    <form method="post" action="/stop_ap">
        <input type="submit" value="Stop AP">
    </form>
</body>
</html>
)";

  server.send(200, "text/html", html);
}

void handle_deauth() {
  int wifi_number = server.arg("net_num").toInt();
  uint16_t reason = server.arg("reason").toInt();

  String html = R"(
<!DOCTYPE html>
<html lang="en">
<head>...</head>
<body>
    <div class="alert)";

  if (wifi_number < num_networks) {
    html += R"(">
        <h2>Starting Deauth-Attack!</h2>
        <p>Deauthenticating network number: )" + String(wifi_number) + R"(</p>
        <p>Reason code: )" + String(reason) + R"(</p>
    </div>)";
    start_deauth(wifi_number, DEAUTH_TYPE_SINGLE, reason);
  } else {
    html += R"( error">
        <h2>Error: Invalid Network Number</h2>
        <p>Please select a valid network number.</p>
    </div>)";
  }

  html += R"(
    <a href="/" class="button">Back to Home</a>
</body>
</html>
  )";

  server.send(200, "text/html", html);
}

void handle_deauth_all() {
  uint16_t reason = server.arg("reason").toInt();

  String html = R"(
<!DOCTYPE html>
<html lang="en">
<head>...</head>
<body>
    <div class="alert">
        <h2>Starting Deauth-Attack on All Networks!</h2>
        <p>WiFi will shut down now. To stop the attack, please reset the ESP32.</p>
        <p>Reason code: )" + String(reason) + R"(</p>
    </div>
</body>
</html>
  )";

  server.send(200, "text/html", html);
  server.stop();
  start_deauth(0, DEAUTH_TYPE_ALL, reason);
}

void handle_rescan() {
  num_networks = WiFi.scanNetworks();
  redirect_root();
}

void handle_stop() {
  stop_deauth();
  redirect_root();
}

void handle_stop_ap() {
  WiFi.softAPdisconnect(true);
  redirect_root();
}

void start_web_interface() {
  server.on("/", handle_root);
  server.on("/deauth", handle_deauth);
  server.on("/deauth_all", handle_deauth_all);
  server.on("/rescan", handle_rescan);
  server.on("/stop", handle_stop);
  server.on("/stop_ap", handle_stop_ap);
  handle_rescan();
  server.begin();
}

void web_interface_handle_client() {
  server.handleClient();
}

String getEncryptionType(wifi_auth_mode_t encryptionType) {
  switch (encryptionType) {
    case WIFI_AUTH_OPEN:
      return "Open";
    case WIFI_AUTH_WEP:
      return "WEP";
    case WIFI_AUTH_WPA_PSK:
      return "WPA_PSK";
    case WIFI_AUTH_WPA2_PSK:
      return "WPA2_PSK";
    case WIFI_AUTH_WPA_WPA2_PSK:
      return "WPA_WPA2_PSK";
    case WIFI_AUTH_WPA2_ENTERPRISE:
      return "WPA2_ENTERPRISE";
    default:
      return "UNKNOWN";
  }
}