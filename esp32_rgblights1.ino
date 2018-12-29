// esp32_rgblights
//
// esp32 connected to an rgb ws281x style string
//
// Ben Dooks <ben@fluff.org>
// Copyright 2018 Ben Dooks

#include <NeoPixelBus.h>
#include <WiFi.h>
#include <WebServer.h>
//#include <AsyncWebServer.h>
#include <ESPmDNS.h>
#include <vector>

#include "anim.h"
#include "wifipass.h"

const uint16_t PixelCount = 50; // this example assumes 4 pixels, making it smaller will cause a failure
const uint8_t PixelPin = 22;  // make sure to set this to the correct pin, ignored for Esp8266

// wifi reconnect: https://www.esp32.com/viewtopic.php?t=3851&p=17506

#define colorSaturation 255

NeoPixelBus<NeoRgbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);

WebServer wwwserv(80);
#define server wwwserv

RgbColor col[50];
class AnimState *state;
class AnimState *state_ov;

std::vector<class AnimState *> anims;
std::vector<class AnimState *> anims_ov;

static void serv_root(void)
{
  //class AnimState *st; 

  String message = "<html><head><title>RGB Leds</title></head><body><h1>RGB LED controls</h1>";

  message += "<form action=\"/set_animation\">\n";
  message += "\t<select name=\"mode\">\n";
  for (class AnimState *st: anims) {
    message += "\t\t<option value=\"" + st->shortName() + "\"";
    if (st == state)
      message += " selected";
    message += ">" + st->longName() + "</option>\n";
  }
  message += "\t<br><input type=\"submit\">\n";
  message += "</form>\n\n";

  message += "<form action=\"/set_overlay\">\n";
  message += "\t<select name=\"mode\">\n";
  for (class AnimState *st: anims_ov) {
    message += "\t\t<option value=\"" + st->shortName() + "\"";
    if (st == state_ov)
      message += " selected";
    message += ">" + st->longName() + "</option>\n";
  }
  message += "\t<br><input type=\"submit\">\n";
  message += "</form>\n</body></html>\n";

  wwwserv.send(200, "text/html", message);
}

static void httpserv_refresh_to(String url)
{
  server.send(200, "text/html", "<html><head><meta http-equiv=\"refresh\" content=\"0; url="  + url +  "\" />" "</head></html><p>Done</p>");  
}

static void httpserv_handle_setoverlay(void)
{
  class AnimState *newstate = NULL;
  String to = server.arg("mode");

  if (to.equals("none")) {
    state_ov = NULL;
    httpserv_refresh_to("/");
    return;
  }

  for (class AnimState *st: anims_ov) {
    if (to.equals(st->shortName()))
      newstate = st;
  }

  if (newstate) {
    Serial.println("new mode is " + to);
    state_ov = newstate;
  } else {
    // todo - didn't find anything //
  }
  httpserv_refresh_to("/");
}

static void httpserv_handle_setanim(void)
{
  class AnimState *newstate = NULL;
  String to = server.arg("mode");

  for (class AnimState *st: anims) {
    if (to.equals(st->shortName()))
      newstate = st;
  }

  if (newstate) {
    Serial.println("new mode is " + to);
    state = newstate;
  } else {
    // todo - didn't find anything //
  }
  httpserv_refresh_to("/");
}

#define TABLE_STYLE "<style>table, th, td { border: 1px solid black; border-collapse: collapse; } th, td { padding: 5px; text-align: left}</style>\n"

static void httpserv_handle_dumplights(void)
{
  // todo - deal with the fact we made a response too big for wwwserv.send()
  String  msg;
  RgbColor *ptr = col;
  char row[64];
  int pixnr;

  //Serial.println("Dumping light state");

  msg = "<html><head><title>System status</title>" TABLE_STYLE "</head>\n<body>";
  msg += "<table style=\"width:100%x\">\n<tr><th>Nr</th><th>R</th><th>G</th><th>B</th></td>\n";

  for (pixnr = 0; pixnr < PixelCount; pixnr++, ptr++) {
    snprintf(row, sizeof(row), "<tr><td>#%d</td><td>%d</td><td>%d</td><td>%d</td></tr>\n",
             pixnr, ptr->R, ptr->G, ptr->B);
    //Serial.print(row);
    msg += row;
  }

  msg += "</body></html>\n";
  //Serial.print(msg);

  wwwserv.send(200, "text/html", msg);
}

static void httpserv_handle_lightboard(void)
{
  String  msg;
  RgbColor *ptr = col;
  char row[64];
  int pixnr;

 msg = "<html><head><meta http-equiv=\"refresh\" content=\"0.1;\" /><title>System status</title>" TABLE_STYLE "</head>\n<body>";
  msg += "<table style=\"width:100%x\">\n";

  for (pixnr = 0; pixnr < PixelCount; pixnr++, ptr++) {
    if ((pixnr % 10) == 0)
      msg += "\n<tr>\n";
    
    snprintf(row, sizeof(row), "\t<td bgcolor=\"#%02x%02x%02x\">#</td>\n", ptr->R, ptr->G, ptr->B);
    
    msg += row;
     if ((pixnr % 10) == 9)
      msg += "</tr>";
 
  }

  msg += "</body></html>\n";
  //Serial.print(msg);

  wwwserv.send(200, "text/html", msg);

  
}

static void httpserv_handle_status(void)
{
  String msg;

  msg = "<html><head><title>System status</title></head>\n<body>";
  
  msg += "<h2>Runtime status</h2>\n";
  msg += "<p>Heap free: ";
  msg +=  ESP.getFreeHeap();
  msg += " bytes\n";
  msg += "<p>Uptime ";
  msg += (millis() / 1000);
  msg += " seconds\n";

  msg += "<h2>Software version</h2>\n";
  msg += "System version 0.1-beta\n";

  wwwserv.send(200, "text/html", msg);
}


void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  Serial.println("ESP32 RGB String server");

  if (false) {
  //WiFi.begin("Hackspace", "T3h4x0rZ");
    WiFi.begin(WIFI_NET, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.print("Connected, IP addr");
  Serial.println(WiFi.localIP());
  } else {
    WiFi.softAP("rgblights", "unsecure");
    Serial.print("IP Address is: ");
    Serial.print(WiFi.softAPIP());
    Serial.println("");
  }

  
  wwwserv.on("/", HTTP_GET, serv_root);
  wwwserv.on("/status", httpserv_handle_status);
  wwwserv.on("/dumplights", httpserv_handle_dumplights);
  wwwserv.on("/lightboard", httpserv_handle_lightboard);
  wwwserv.on("/set_animation", httpserv_handle_setanim);
  wwwserv.on("/set_overlay", httpserv_handle_setoverlay);
  wwwserv.begin();
 
  Serial.println("setting initial colour...");
  strip.Begin();

  if (MDNS.begin("rgblights")) {
    Serial.println("MDNS responder started");
  } else {
    Serial.println("Cannot start MDNS");
  }

  // set the default animation and add it to the list
  state = new ChaserAnim(col, PixelCount);
  anims.push_back(state);

  anims.push_back(new ChaserAnim2(col, PixelCount));

  // push some simple sinewave animations
  anims.push_back(new SineWaveAnim(col, PixelCount, "sinered", "Sinewave Red", 255, 0, 0));
  anims.push_back(new SineWaveAnim(col, PixelCount, "sinegreen", "Sinewave Green", 0, 255, 0));
  anims.push_back(new SineWaveAnim(col, PixelCount, "sineblue", "Sinewave Blue", 0, 0, 255));
  anims.push_back(new SineWaveAnim(col, PixelCount, "sinecyan", "Sinewave Cyan", 0, 255, 255));
  anims.push_back(new SineWaveAnim(col, PixelCount, "sinepurple", "Sinewave Purple", 127, 0, 255));
  anims.push_back(new SineWaveAnim(col, PixelCount, "sineyellow", "Sinewave Yellow", 255, 255, 0));
  anims.push_back(new SineWaveAnim(col, PixelCount, "sineorange", "Sinewave Orange", 255, 128, 0));
  anims.push_back(new SineWaveAnim(col, PixelCount, "sinewhite", "Sinewave White", 255, 255, 255));

  state_ov = new NoAnim(col, PixelCount);
  anims_ov.push_back(state_ov);
  anims_ov.push_back(new ChaserAnimOverlay(col, PixelCount));

  int i;
  for (i = 0; i  < PixelCount; i++) {
    strip.SetPixelColor(i, RgbColor(0, 0, 127));
  }
  strip.Show();

  Serial.println("starting...");
}

static bool timeTo(unsigned long *last, unsigned long interval, unsigned long curtime)
{
  if ((curtime - *last) >= interval) {
    *last = curtime;
    return true;
  }

  return false;
}

//static RgbColor from_col = RgbColor(0, 0, 0);
//static RgbColor to_col = RgbColor(0, 255, 0);


unsigned long upd_time = 0;

void loop() {
  unsigned long curtime;
  int i;

  wwwserv.handleClient();
  
  curtime = millis();
  if (timeTo(&upd_time, 100, curtime)) {
    state->updateState();
    if (state_ov)
      state_ov->updateState();
    for (i = 0; i < PixelCount; i++)
      strip.SetPixelColor(i, col[i]);
    strip.Show();
  }
}
