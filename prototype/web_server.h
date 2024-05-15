/*
 * The webserver will attempt to connect to the primary ssid 5 times, with a delay
 * of 1 second between each attempt. If it fails to connect, it will retry
 * using the secondary ssid. The reasoning behind this: If I am at home, I want
 * it to connect to my home network. If I'm elsewhere, I want it to connect
 * to the hotspot on my phone. If it fails to connect to either, it will default
 * to its previously saved settings.
 */

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "secrets.h"

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// Web server html
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Infinitube</title>
  <style>
    html { font-family: Arial; display: inline-block; }
    body { max-width: 500px; margin:0px auto; padding: 0px 7px; }
    button { width: 32%%; height: 50px; font-size: 0.9rem; }
    table { width: 100%%; }
    h2 { font-size: 2.3rem; text-align: center }
    p { font-size: 1.9rem; }
    .buttons { display: flex; flex-wrap: wrap; }
    .break { flex-basis: 100%%; height: 10px; }
    .labelCol { width: 80px; }
    .valCol { width: 40px; padding-left: 14px; }
    .slider { -webkit-appearance: none; margin: 14px 0; width: 100%%;
      height: 15px; background: #FFD65C; outline: none; -webkit-transition: .2s;
      transition: opacity .2s; }
    .slider::-webkit-slider-thumb { -webkit-appearance: none; appearance: none;
      width: 25px; height: 25px; background: #003249; cursor: pointer; }
    .slider::-moz-range-thumb { width: 25px; height: 25px; background: #003249;
      cursor: pointer; }
  </style>
</head>
<body>
  <h2>Infinitube! :-O</h2>

  <table border="0">
  <tr>
    <td class="labelCol"><label id="labelSpeed" for="speedSlider">Speed</label></td>
    <td><input type="range" id="speedSlider" onchange="sendData('s',this.value)" min="1" max="10" value="%SPEEDVALUE%" step="1" class="slider"></td>
    <td class="valCol"><span id="speedValue">%SPEEDVALUE%</span></td>
  </tr>
  <tr>
    <td class="labelCol"><label id="labelDensity" for="densitySlider">Density</label></td>
    <td><input type="range" id="densitySlider" onchange="sendData('d',this.value)" min="1" max="10" value="%DENSITYVALUE%" step="1" class="slider"></td>
    <td class="valCol"><span id="densityValue">%DENSITYVALUE%</span></td>
  </tr>
  <tr>
    <td class="labelCol"><label id="labelWidth" for="widthSlider">Width</label></td>
    <td><input type="range" id="widthSlider" onchange="sendData('w',this.value)" min="1" max="10" value="%WIDTHVALUE%" step="1" class="slider"></td>
    <td class="valCol"><span id="widthValue">%WIDTHVALUE%</span></td>
  </tr>
  <tr>
    <td class="labelCol"><label id="labelOverlaySpeed" for="overlaySpeedSlider">Overlay Speed</label></td>
    <td><input type="range" id="overlaySpeedSlider" onchange="sendData('o',this.value)" min="1" max="10" value="%OVERLAYSPEEDVALUE%" step="1" class="slider"></td>
    <td class="valCol"><span id="overlaySpeedValue">%OVERLAYSPEEDVALUE%</span></td>
  </tr>
  <tr>
    <td class="labelCol"><label id="labelOverlayWidth" for="overlayWidthSlider">Overlay Width</label></td>
    <td><input type="range" id="overlayWidthSlider" onchange="sendData('x',this.value)" min="1" max="10" value="%OVERLAYWIDTHVALUE%" step="1" class="slider"></td>
    <td class="valCol"><span id="overlayWidthValue">%OVERLAYWIDTHVALUE%</span></td>
  </tr>
  </table>
  <div>
    <input type="checkbox" id="reverse" name="reverse" onclick="sendData('r',this.value)" />
    <label for="reverse">Reverse</label>
  </div>

  <h3>Patterns</h3>
  <div class="buttons">
    <button type="button" onclick="sendData('n',0)">Twinkle</button>
    <button type="button" onclick="sendData('n',1)">Twinkle: Random Fading Segments</button>
    <button type="button" onclick="sendData('n',2)">Twinkle: Random Flashing Segments</button>
    <button type="button" onclick="sendData('n',3)">Twinkle: Wave Overlay</button>
    <div class="break"></div>
    <button type="button" onclick="sendData('n',4)">Spiral: Single</button>
    <button type="button" onclick="sendData('n',5)">Spiral: Double</button>
    <div class="break"></div>
    <button type="button" onclick="sendData('n',6)">Lines: Rotating Pong</button>
    <button type="button" onclick="sendData('n',7)">Lines: Lasers</button>
    <button type="button" onclick="sendData('n',8)">Lines: Lasers All At Once</button>
    <button type="button" onclick="sendData('n',9)">Lines: Rainfall</button>
    <button type="button" onclick="sendData('n',10)">Lines: Basket Weaving</button>
    <button type="button" onclick="sendData('n',11)">Lines: Comet Trails</button>
    <div class="break"></div>
    <button type="button" onclick="sendData('n',12)">Spinning: Rotating Hexagons</button>
    <button type="button" onclick="sendData('n',13)">Spinning: Counter Rotating Hexagons</button>
    <button type="button" onclick="sendData('n',14)">Spinning: Variable Speed Rotation</button>
    <div class="break"></div>
    <button type="button" onclick="sendData('n',15)">Flashing Hexagons</button>
    <button type="button" onclick="sendData('n',16)">Flashing Hexagons Warp</button>
    <div class="break"></div>

    <h3>Color Palettes</h3>
    <div class="break"></div>
    <button type="button" onclick="sendData('p',0)">Fire</button>
    <button type="button" onclick="sendData('p',1)">Ocean</button>
    <button type="button" onclick="sendData('p',2)">Floral</button>
    <button type="button" onclick="sendData('p',3)">Ice</button>
    <button type="button" onclick="sendData('p',4)">Fairy</button>
    <button type="button" id="autoBtn" onclick="sendData('a',1)">Auto Cycle Palettes</button>
    <div class="break"></div>
    <label id="labelAutoChangeTime" for="secondsPerPalette">Seconds per palette on auto </label>
    <input id="secondsPerPalette" type="number" min="1" max="65535" onchange="sendData('t',this.value)" value="%SECONDSPERPALETTE%">
    <div class="break"></div>

    <h3>Color Modes</h3>
    <div class="break"></div>
    <button type="button" onclick="sendData('m',0)">Solid</button>
    <button type="button" onclick="sendData('m',1)">Index Gradient</button>
    <button type="button" onclick="sendData('m',2)">Depth Gradient</button>
    <button type="button" onclick="sendData('m',3)">Angle Gradient</button>
    <div class="break"></div>
  </div>

<script>
 var gateway = `ws://${window.location.hostname}/ws`;
  var websocket;
  window.addEventListener('load', onLoad);

  function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage; // <-- add this line
  }

  function onOpen(event) {
    console.log('Connection opened');
    document.getElementById('autoBtn').style.backgroundColor = '#baffb3';
  }

  function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
  }

  function onMessage(event) {
    var dataType = event.data.charAt(0);
    var dataValue = event.data.substring(1);
    switch (dataType){
    case 't':
      document.getElementById('secondsPerPalette').value = dataValue;
      break;
    case 's':
      document.getElementById('speedValue').innerHTML = dataValue;
      document.getElementById('speedSlider').value = dataValue;
      break;
    case 'd':
      document.getElementById('densityValue').innerHTML = dataValue;
      document.getElementById('densitySlider').value = dataValue;
      break;
    case 'w':
      document.getElementById('widthValue').innerHTML = dataValue;
      document.getElementById('widthSlider').value = dataValue;
      break;
    case 'o':
      document.getElementById('overlaySpeedValue').innerHTML = dataValue;
      document.getElementById('overlaySpeedSlider').value = dataValue;
      break;
    case 'x':
      document.getElementById('overlayWidthValue').innerHTML = dataValue;
      document.getElementById('overlayWidthSlider').value = dataValue;
      break;
    case 'a':
      const button = document.getElementById('autoBtn');
      if (dataValue == '1') button.style.backgroundColor = '#baffb3';
      else button.style.backgroundColor = '';
      break;
    }
  }

  function onLoad(event) {
    initWebSocket();
  }

  function sendData(type, val) {
    console.log(type+val);
    websocket.send(type+val);
  }
</script>
</body>
</html>
)rawliteral";

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    Serial.println((char*)data);
    String message = String((char*)data);
    char dataType = message.charAt(0);
    String dataValue = message.substring(1);

    switch (dataType) {
      case 'n':
        activePattern = dataValue.toInt();
        ws.textAll(message);
        break;
      case 'p':
        palette.setPalette(dataValue.toInt());
        ws.textAll(message);
        autoCyclePalettes = false;
        ws.textAll("a0");
        break;
      case 't':
        palette.setSecondsPerPalette(dataValue.toInt());
        ws.textAll(message);
        break;
      case 'm':
        palette.setColorMode(dataValue.toInt());
        ws.textAll(message);
        break;
      case 's':
        speed = dataValue.toInt();
        ws.textAll(message);
        break;
      case 'd':
        density = dataValue.toInt();
        ws.textAll(message);
        break;
      case 'w':
        width = dataValue.toInt();
        ws.textAll(message);
        break;
      case 'o':
        overlaySpeed = dataValue.toInt();
        ws.textAll(message);
        break;
      case 'x':
        overlayWidth = dataValue.toInt();
        ws.textAll(message);
        break;
      case 'a':
        autoCyclePalettes = !autoCyclePalettes;
        if (autoCyclePalettes) ws.textAll("a1");
        else ws.textAll("a0");
        break;
      case 'r':
        reverse = !reverse;
        break;
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

String processor(const String& var){
  if(var == "AUTOCYCLEPALETTES"){
    return String(autoCyclePalettes);
  }
  if(var == "SECONDSPERPALETTE"){
    return String(palette.getSecondsPerPalette());
  }
  if(var == "SPEEDVALUE"){
    return String(speed);
  }
  if(var == "DENSITYVALUE"){
    return String(density);
  }
  if(var == "WIDTHVALUE"){
    return String(width);
  }
  if(var == "OVERLAYSPEEDVALUE"){
    return String(overlaySpeed);
  }
  if(var == "OVERLAYWIDTHVALUE"){
    return String(overlayWidth);
  }
  return "";
}

void setupWebServer(){

  uint8_t connectionAttempts = 0;

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay (1000);
    Serial.println("Connecting to primary WiFi ...");
    connectionAttempts++;
    if (connectionAttempts > 5) break;
  }

  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid2, password2);
    delay (1000);
    Serial.println("Connecting to secondary WiFi ...");
    connectionAttempts++;
    if (connectionAttempts > 10) break;
  }

  // Print ESP Local IP Address
  Serial.print("Local IP address: ");
  Serial.println(WiFi.localIP());

  initWebSocket();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  server.begin();
}
