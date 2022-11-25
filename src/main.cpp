#include <ArtnetWifi.h>
#include <Arduino.h>

// DEBUG SETUP
const bool debug = false;

// LASER SETUP
const int laserCount = 5;
int lasers [laserCount] = {12,13,5,23,19};

// WIFI SETUP
const char* ssid = "o2-WLAN-34A0"; // CHANGE FOR YOUR SETUP
const char* password = "28TVQ8R3EGXQL349"; // CHANGE FOR YOUR SETUP

WiFiUDP UdpSend;
ArtnetWifi artnet;

void initLasers()
{
  for (int i = 0; i < laserCount; i++)
  {
    pinMode(lasers[i], OUTPUT);
  }
}

void laserOn(int laserIndex)
{
  digitalWrite(lasers[laserIndex], HIGH);
  if (debug) {
    Serial.print("Laser ");
    Serial.print(laserIndex + 1);
    Serial.print(" ON.");
    Serial.println();
  }
}

void laserOff(int laserIndex) 
{
  digitalWrite(lasers[laserIndex], LOW);
  if (debug) {
    Serial.print("Laser ");
    Serial.print(laserIndex + 1);
    Serial.print(" OFF.");
    Serial.println();
  }
}

// connect to wifi – returns true if successful or false if not
bool ConnectWifi(void)
{
  bool state = true;
  int i = 0;

  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");
  
  // Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 20){
      state = false;
      break;
    }
    i++;
  }
  if (state) {
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(IPAddress(WiFi.localIP()));
  } else {
    Serial.println("");
    Serial.println("Connection failed.");
  }
  
  return state;
}

void parseDmxSequenceData(uint8_t* data)
{
  for (int i = 0; i < laserCount; i++) {
    if (data[i] > 127) {
      laserOn(i);
    } else {
      laserOff(i);
    }
  }
}
void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
{
  if (debug) {
    Serial.print("DMX: Univ: ");
    Serial.print(universe, DEC);
    Serial.print(", Seq: ");
    Serial.print(sequence, DEC);
    Serial.print(", Data (");
    Serial.print(length, DEC);
    Serial.print(")");
  }
  parseDmxSequenceData(data);
  if (debug) {
    Serial.println();
  }
}


// MAIN

void setup()
{
  initLasers();

  // set-up serial for debug output
  Serial.begin(115200);
  ConnectWifi();

  // this will be called for each packet received
  artnet.setArtDmxCallback(onDmxFrame);
  artnet.begin();
}

void loop()
{
  // we call the read function inside the loop
  artnet.read();
}
