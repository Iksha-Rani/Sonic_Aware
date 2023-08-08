#include <ESP8266WiFi.h>
#include <Wire.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// Wifi network station credentials
#define WIFI_SSID "Not Available"
#define WIFI_PASSWORD "cozycozy"
// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN "6263183253:AAHJDk7NNGuFKktRJgzkBQ2dngWNWR_ZN-c"

// Use @myidbot (IDBot) to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
#define CHAT_ID "1309747620"

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

const int soundSensorPin = 16;
const int sampleWindow = 50;                              // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;

const int threshold =70;
void setup() {
  Serial.begin(115200);
  Serial.println();

 // attempt to connect to Wifi network:
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  secured_client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Retrieving time: ");
  configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);

  bot.sendMessage(CHAT_ID, "Bot started up", "");
  pinMode(soundSensorPin, INPUT);
}

void loop() {
   // Read sound sensor value
  int soundValue = digitalRead(soundSensorPin);
  unsigned long startMillis= millis();                   // Start of sample window
   float peakToPeak = 0;                                  // peak-to-peak level
 
   unsigned int signalMax = 0;                            //minimum value
   unsigned int signalMin = 1024;

    // Check if sound level crosses threshold
   bool aboveThreshold = false;

  // If sound is detected, send a message to Telegram
   while (millis() - startMillis < sampleWindow)
   {
      sample = analogRead(soundSensorPin);                    //get reading from microphone
      if (sample < 1024)                                  // toss out spurious readings
      {
         if (sample > signalMax)
         {
            signalMax = sample;                           // save just the max levels
         }
         else if (sample < signalMin)
         {
            signalMin = sample;                           // save just the min levels
         }
      }
      if (sample >=threshold){
        aboveThreshold = true;
      }
   }
 
   peakToPeak = signalMax - signalMin;                    // max - min = peak-peak amplitude
   int db = map(peakToPeak,20,900,49.5,90);             //calibrate for deciBels


   
   if (db <= 49.5)
   {
     String message = "PEACEFUL(db: " + String(db) + ")";
      bot.sendPhoto(CHAT_ID, "https://www.google.com/imgres?imgurl=https%3A%2F%2Fpng.pngtree.com%2Fbackground%2F20220720%2Foriginal%2Fpngtree-photography-wallpaper-of-lugu-lake-with-quiet-natural-scenery-in-yunnan-picture-image_1688111.jpg&tbnid=n1ggoo00okWRiM&vet=12ahUKEwjsjvOXt5z-AhVOCrcAHT_yD88QMygMegQIARBe..i&imgrefurl=https%3A%2F%2Fpngtree.com%2Ffree-backgrounds-photos%2Fquiet-scenery&docid=fXBYPDDTjiCavM&w=1200&h=800&q=http%20jpg%20link%20for%20quiet%20scenery&ved=2ahUKEwjsjvOXt5z-AhVOCrcAHT_yD88QMygMegQIARBe", message);
      Serial.println("COMMUNICATING THE INFO");
      delay(800); // wait 0.8 seconds before sending new message
   }
   else if (db >49.5 && db<60)
   {
     String message = "QUIET DOWN(db: " + String(db) + ")";
     bot.sendMessage(CHAT_ID,"https://www.google.com/imgres?imgurl=https%3A%2F%2Fmedia.istockphoto.com%2Fid%2F1132759988%2Fphoto%2Fclose-up-portrait-of-her-she-nice-cute-lovely-attractive-charming-cheerful-wavy-haired-lady.jpg%3Fs%3D612x612%26w%3D0%26k%3D20%26c%3D_Dnavtd5yXu7l8UkvfiVQc2q08x5vEEDtFaiJsDjIpQ%3D&tbnid=gezySSENaj05lM&vet=12ahUKEwiIpo2xn53-AhWlCbcAHTezChQQMygFegQIARBN..i&imgrefurl=https%3A%2F%2Fwww.istockphoto.com%2Fphotos%2Fbe-quiet&docid=-EQAQVsswIxipM&w=612&h=408&q=HTTP%20JPEG%20IMAGE%20be%20quiet&hl=en&ved=2ahUKEwiIpo2xn53-AhWlCbcAHTezChQQMygFegQIARBN", message);
     Serial.println("COMMUNICATING THE INFO");
     delay(1000); // wait 1 seconds before sending new one
   }
   else if (db>=60)
   {
     String message = "TOO NOISY!!(db: " + String(db) + ")";
      bot.sendPhoto(CHAT_ID, "https://www.google.com/imgres?imgurl=https%3A%2F%2Fimages.pexels.com%2Fphotos%2F2034851%2Fpexels-photo-2034851.jpeg%3Fcs%3Dsrgb%26dl%3Dpexels-edoardo-tommasini-2034851.jpg%26fm%3Djpg&tbnid=dIjhnyMjW2LsVM&vet=12ahUKEwirjqO4vJz-AhWcznMBHZOnCngQMygCegQIARBB..i&imgrefurl=https%3A%2F%2Fwww.pexels.com%2Fsearch%2Fnightclub%2F&docid=dx6h8oN1MIdkHM&w=5946&h=3964&q=http%20jpg%20pic%20for%20clubbing&hl=en&ved=2ahUKEwirjqO4vJz-AhWcznMBHZOnCngQMygCegQIARBB", message);
     Serial.println("sending warning to user!");
      if (aboveThreshold)
   {
     String message = "Consider lowering the volume.Sound level crossed the threshold: " + String(70) + " dB";
     bot.sendMessage(CHAT_ID, message);
     Serial.println("Message sent");
     delay(500); }// wait 0.5 seconds before sending new message
     delay(800); // wait 0.8 seconds before sending new one
   }}


