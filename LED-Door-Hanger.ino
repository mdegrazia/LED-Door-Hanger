//LED Door Hanger by Mari DeGrazia 
//arizona4n6@gmail.com
//This LED DoorHanger uses the following libraries
// MD_Paroloa: https://github.com/MajicDesigns/MD_Parola
// LED Control: https://github.com/wayoda/LedControl
//
// The user can connect to the web based interface using
// the IP address http://192.168.4.1 and password ihavenofork
//
//
//

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include "LedControl.h"

#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

/* Set these to your desired credentials. */
const char *ssid = "LEDDoorHanger";
const char *mypassword = "ihavenofork";
//Go to http://192.168.4.1 in a web browser

ESP8266WebServer server(80);

//Moving Eye stuff
#include "MD_EyePair.h"
#define MAX_EYE_PAIR 1
MD_EyePair E[MAX_EYE_PAIR];
// Miscellaneous defines
#define  DELAYTIME  500  // in milliseconds


//LED Control stuff
#define Width 16
#define Height 16
LedControl lc=LedControl(13,14,12,6); //datain, clk,cs, number devices

//Images to display on the 16X16 grid underneath the scrolling text by LEDcontrol. Sill images.
// to create new images, make a 16X16 image, and use http://javl.github.io/image2cpp/ to convert it to plain bytes
byte skull[32]=
{0x00, 0x00, 0x01, 0xc0, 0x06, 0x30, 0x08, 0x08, 0x10, 0x04, 0x20, 0x02, 0x20, 0x02, 0x20, 0x02,0x22, 0x12, 0x26, 0x32, 0x10, 0x84, 0x08, 0x08, 0x04, 0x10, 0x05, 0x50, 0x05, 0x50, 0x03, 0xe0};

byte chill[32]=
{0x07, 0xc0, 0x1f, 0xf0, 0x3f, 0xf8, 0x7f, 0xfc, 0x7f, 0xfc, 0x00, 0x00, 0xd8, 0x66, 0xd1, 0x46,0xe3, 0x8e, 0xff, 0xfe, 0x7f, 0xdc, 0x78, 0x3c, 0x3f, 0xf8, 0x1f, 0xf0, 0x07, 0xc0, 0x00, 0x00};

byte sad[32]=
{0x07, 0xc0, 0x1f, 0xf0, 0x3f, 0xf8, 0x7f, 0xfc, 0x7f, 0xfc, 0xff, 0xfe, 0xf3, 0x9e, 0xef, 0xee,0xff, 0xfe, 0xff, 0xfe, 0x7f, 0xfc, 0x78, 0x3c, 0x37, 0xd8, 0x1f, 0xf0, 0x07, 0xc0, 0x00, 0x00};

byte happy[32]=
{0x07, 0xc0, 0x1f, 0xf0, 0x3f, 0xf8, 0x7f, 0xfc, 0x63, 0x8c, 0xdf, 0xf6, 0xf3, 0x9e, 0xed, 0x6e,0xff, 0xfe, 0xff, 0xfe, 0x77, 0xdc, 0x78, 0x3c, 0x3f, 0xf8, 0x1f, 0xf0, 0x07, 0xc0, 0x00, 0x00};

byte heart[32]=
{0x07, 0xc0, 0x1f, 0xf0, 0x3f, 0xf8, 0x6b, 0xac, 0x41, 0x06, 0xe3, 0x8e, 0xf7, 0xde, 0xff, 0xfe,0xff, 0xfe, 0xe0, 0x0e, 0x70, 0x1c, 0x78, 0x3c, 0x3c, 0x78, 0x1f, 0xf0, 0x07, 0xc0, 0x00, 0x00};
//animated text-sprite Parola stuff

byte noenter[32]=
{0x07, 0xc0, 0x1f, 0xf0, 0x2f, 0xe8, 0x77, 0xdc, 0x7b, 0xbc, 0xfd, 0x7e, 0xfe, 0xfe, 0xfe, 0xfe, 0xfd, 0x7e, 0xfb, 0xbe, 0x77, 0xdc, 0x6f, 0xec, 0x3f, 0xf8, 0x1f, 0xf0, 0x07, 0xc0, 0x00, 0x00};
const uint16_t PAUSE_TIME = 1000; // in milliseconds


// Global variables
uint8_t  sprite_cur_string = 0;
char *sprite_msg[75] =
{
  "Hello",
  //"Animation"
};


// Sprite Definitions
const uint8_t F_PMAN1 = 6;
const uint8_t W_PMAN1 = 8;
uint8_t PROGMEM pacman1[F_PMAN1 * W_PMAN1] =  // gobbling pacman animation
{
  0x00, 0x81, 0xc3, 0xe7, 0xff, 0x7e, 0x7e, 0x3c,
  0x00, 0x42, 0xe7, 0xe7, 0xff, 0xff, 0x7e, 0x3c,
  0x24, 0x66, 0xe7, 0xff, 0xff, 0xff, 0x7e, 0x3c,
  0x3c, 0x7e, 0xff, 0xff, 0xff, 0xff, 0x7e, 0x3c,
  0x24, 0x66, 0xe7, 0xff, 0xff, 0xff, 0x7e, 0x3c,
  0x00, 0x42, 0xe7, 0xe7, 0xff, 0xff, 0x7e, 0x3c,
};

const uint8_t F_PMAN2 = 6;
const uint8_t W_PMAN2 = 18;
uint8_t PROGMEM pacman2[F_PMAN2 * W_PMAN2] =  // ghost pursued by a pacman
{
  0x00, 0x81, 0xc3, 0xe7, 0xff, 0x7e, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe,
  0x00, 0x42, 0xe7, 0xe7, 0xff, 0xff, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe,
  0x24, 0x66, 0xe7, 0xff, 0xff, 0xff, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe,
  0x3c, 0x7e, 0xff, 0xff, 0xff, 0xff, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe,
  0x24, 0x66, 0xe7, 0xff, 0xff, 0xff, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe,
  0x00, 0x42, 0xe7, 0xe7, 0xff, 0xff, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe,
};

const uint8_t F_INVADER = 2;
const uint8_t W_INVADER = 10;
uint8_t PROGMEM invader[F_INVADER * W_INVADER] =  // space invader
{
  0x0e, 0x98, 0x7d, 0x36, 0x3c, 0x3c, 0x36, 0x7d, 0x98, 0x0e,
  0x70, 0x18, 0x7d, 0xb6, 0x3c, 0x3c, 0xb6, 0x7d, 0x18, 0x70,
};

const uint8_t F_HEART = 5;
const uint8_t W_HEART = 9;
uint8_t PROGMEM heartbeat[F_HEART * W_HEART] =  // beating heart
{
  0x0e, 0x11, 0x21, 0x42, 0x84, 0x42, 0x21, 0x11, 0x0e,
  0x0e, 0x1f, 0x33, 0x66, 0xcc, 0x66, 0x33, 0x1f, 0x0e,
  0x0e, 0x1f, 0x3f, 0x7e, 0xfc, 0x7e, 0x3f, 0x1f, 0x0e,
  0x0e, 0x1f, 0x33, 0x66, 0xcc, 0x66, 0x33, 0x1f, 0x0e,
  0x0e, 0x11, 0x21, 0x42, 0x84, 0x42, 0x21, 0x11, 0x0e,
};
        
// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 6
#define CLK_PIN   14
#define DATA_PIN  13
#define CS_PIN    12

MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
MD_MAX72XX M = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);


uint8_t scrollSpeed = 100;    // default frame delay value
textEffect_t scrollEffect = PA_SCROLL_LEFT;
textPosition_t scrollAlign = PA_LEFT;
uint16_t scrollPause = 25; // in milliseconds

// Global message buffers shared by Serial and Scrolling functions
#define  BUF_SIZE  75
char curMessage[BUF_SIZE] = { "" };
char newMessage[BUF_SIZE] = { "Keep Out" };
bool newMessageAvailable = true;
String newImage = "skull";
String wipeImage = "pacman";

byte displayimage[32];
bool doWipe = false;
int wipeCount = 0;

void setup()
{
  
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, mypassword);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  
  // Next define what the server should do when a client connects
  server.on("/", HandleClient); // The client connected with no arguments e.g. http:192.160.0.40/
  server.on("/result", ShowClientResponse);
  server.begin();
  Serial.println("HTTP server started");
  Serial.println(sprite_msg[0]);
  Serial.println(sprite_cur_string);

  P.begin(6);
  P.setZone(0,0,1);
  P.setZone(1,2,3);
  P.setZone(2,4,5);
  P.setZone(3,0,5);
  
  
  P.displayZoneText(2,curMessage, scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);
  M.begin();

  Serial.println(sprite_msg[0]);
  Serial.println(sprite_cur_string);

  
  // initialize the eye view
  for (uint8_t i=0; i<MAX_EYE_PAIR; i++)
    E[i].begin(i*2, &M, DELAYTIME);
    
}

void loop()
{   
 server.handleClient();
    
 if (P.displayAnimate())
  {   
    //show image on the lower 4 quadrents after the current animation stops
    if ((doWipe == false)&& (P.getZoneStatus(3))) { 
      if (newImage == "skull"){
        displayImage(skull);
      }
      if (newImage =="chill"){
        displayImage(chill);
      }
       if (newImage =="heart"){
        displayImage(heart);
      }
      if (newImage =="happy"){
        displayImage(happy);
      }
      if (newImage =="sad"){
        displayImage(sad);
      }
      if (newImage =="noenter"){
        displayImage(noenter);
      }

      //uncomment below lines if you add in a new image
      /*
      if (newImage =="myImage"){
        displayImage(myImage);
      }
      */
      if (newImage =="eyes"){
        lc.clearDisplay(2);
        lc.clearDisplay(3);
        for (uint8_t i=0; i<MAX_EYE_PAIR; i++)
        E[i].animate();
      }
    }
    
    //the wipe animation takes the whole board, so wait for current animations to stop first
    if (doWipe == true){
       //first time running the wipe? If so, intialize
       if (wipeCount == 0){
          //wait for current animation to stop
          
     
          if (P.getZoneStatus(2)){
              sprite_cur_string = 0;
            
              
              P.displayZoneText(3,sprite_msg[sprite_cur_string], PA_LEFT, 100, PAUSE_TIME, PA_SPRITE, PA_SPRITE);
            
             
              if (wipeImage== "pacman"){
                Serial.println("pacman here");
                P.setSpriteData(pacman1, W_PMAN1, F_PMAN1, pacman2, W_PMAN2, F_PMAN2);
              }
              

            if (wipeImage== "invader"){
              Serial.println("invader here");
                P.setSpriteData(invader,W_INVADER, F_INVADER, invader, W_INVADER, F_INVADER);
              }

             if (wipeImage== "heartbeat"){
              
                P.setSpriteData(heartbeat,W_HEART, F_HEART, heartbeat, W_HEART, F_HEART);
              }
            
            
            wipeCount ++;
          }
        }
    }
    
    //zone 2 has the scroll
    if (P.getZoneStatus(2)){
      if (newMessageAvailable){
      strcpy(curMessage, newMessage);
      newMessageAvailable = false;
      //reset wipe count since we are scrolling now.
      doWipe = false;
      wipeCount = 0;
      }

      if ((doWipe!= true) && (P.getZoneStatus(3))) {
        P.displayReset(2);
      }
    }
    
    if ((doWipe == true) && (P.getZoneStatus(2))){
      if (P.getZoneStatus(3)){  
        //for text-sprite animation 
        if (sprite_cur_string == ARRAY_SIZE(sprite_msg)){
          sprite_cur_string = 0;
        }
    
        P.setTextBuffer(sprite_msg[sprite_cur_string]);
        P.displayReset(3);
        sprite_cur_string++;
      }
    }
  }//end if p.DisplayAnimate
  
}


void HandleClient() {
  
  String webpage;
  webpage =  "<html>";
   webpage += "<head><title>LED Door Hanger</title>";
    webpage += "<style>";
     webpage += "body { background-color: #E6E6FA; font-family: Arial, Helvetica, Sans-Serif; Color: blue;}";
    webpage += "</style>";
    webpage += "<meta name='viewport' content='width=device-width, initial-scale=1' />"; 
   webpage += "</head>";
   webpage += "<body>";
     String IPaddress = WiFi.localIP().toString();
     webpage += "<h1>LED Door Hanger</h1>";
     webpage += "<hr>";
     webpage += "<form action='http://192.168.4.1/result' method='POST'>";
     webpage += "<h2>Display Text<h2></h2>";
     webpage += "<input type='text' name='user_input'><br>";
     webpage += "<hr>";
     webpage += "<h2>Image to Display</h1>";
     webpage += "<input type='radio' name='image' value='skull'>Skull<br>";
     webpage += "<input type='radio' name='image' value='chill'>Chill Emoji<br>";
     webpage += "<input type='radio' name='image' value='heart'>Heart Emoji<br>";
     webpage += "<input type='radio' name='image' value='happy'>Happy Emoji<br>";     
     webpage += "<input type='radio' name='image' value='sad'>Sad Emoji<br>";
     webpage += "<input type='radio' name='image' value='noenter'>Do Not Enter Sign<br>";
     webpage += "<input type='radio' name='image' value='eyes'>Animated Eyes<br>";
     webpage += "<input type='radio' name='image' value='pacman'>Text Wipe with PacMan<br>";
     webpage += "<input type='radio' name='image' value='invader'>Text Wipe with Invaders<br>";
      webpage += "<input type='radio' name='image' value='heartbeat'>Text Wipe with Heart<br>";
     webpage += "<hr><br>";
     webpage += "<input type='submit' value='Update'>";
    webpage += "</form>";
   webpage += "</body>";
  webpage += "</html>";
  server.send(200, "text/html", webpage); // Send a response to the client asking for input
}

void ShowClientResponse() {
  static char *message = newMessage;
  //static char *testmessage = sprite_msg
  if (server.args() > 0 ) {
    for ( uint8_t i = 0; i < server.args(); i++ ) {
      if (server.argName(i) == "user_input") {
         if (server.arg(i) != ""){
           Serial.print(server.arg(i));
           //int messagelength = server.arg(i).length()+1;
           //char message[messagelength];
           server.arg(i).toCharArray(message,75);
           strcpy (*sprite_msg,message);
           Serial.print("Message");
           Serial.print(message);
           
           //newMessage = message;
           newMessageAvailable = true;
         }
      }
      if( server.argName(i) == "image"){
        Serial.print(server.arg(i));
        newImage = server.arg(i);
        doWipe = false;
      }

      if ((server.argName(i) == "image") && (server.arg(i) == "pacman")){
        Serial.print(server.arg(i));
        newImage = server.arg(i);
        wipeImage = "pacman";
        doWipe = true;
        wipeCount = 0;
      }

      if ((server.argName(i) == "image") && (server.arg(i) == "invader")){
        Serial.print(server.arg(i));
        newImage = server.arg(i);
        wipeImage = "invader";
        doWipe = true;
        wipeCount = 0;
      }

      if ((server.argName(i) == "image") && (server.arg(i) == "heartbeat")){
        Serial.print(server.arg(i));
        newImage = server.arg(i);
        wipeImage = "heartbeat";
        doWipe = true;
        wipeCount = 0;
      }
   }
   String webpage;
  webpage =  "<html>";
   webpage += "<head><title>LED Door Hanger</title>";
    webpage += "<style>";
     webpage += "body { background-color: #E6E6FA; font-family: Arial, Helvetica, Sans-Serif; Color: blue;}";
    webpage += "</style>";
   webpage += "</head>";
   webpage += "<meta name='viewport' content='width=device-width, initial-scale=1' />"; 
   webpage += "<body>";
    webpage += "<h1><br>Updated</h1>";
     webpage += "The display will update after the current animation completes.<br><br>";
     webpage += "Return to <a href='http://192.168.4.1'>update page</a>";
    
   webpage += "</body>";
  webpage += "</html>";
  server.send(200, "text/html", webpage); 
}
}


// the LED matrix are layed out like 3<--2<--1<--0, so we need to remap the image to the right sections.
void displayImage(byte imagename[]){

//loop through and map out the upper left of theimage to (matrix  3) )
for (int i = 0; i < 16; i++)  
  {
        //only grab the first  8 bytes, which are the even ones (0,2,4,6,8,12.14.16)
        if ((i % 2) == 0){
          lc.setRow(3,i/2,imagename[i]); //the row number will be equal to i/2
    }
  }

    //loop through and map out the upper right of theimage to (matrix  3) )
    for (int i = 0; i < 16; i++)  
  {
        //show on 
    
    if ((i % 2)!= 0){
       lc.setRow(2,i/2,imagename[i]);
    }
  }


  //loop through the first 16 bytes
for (int i = 16; i < 32; i++)  
  {
        //only grab the first  8 bytes, which are the even ones (0,2,4,6,8,12.14.16)
        if ((i % 2) == 0){
          lc.setRow(1,(i/2)-8,imagename[i]); //the row number will be equal to (i/2)-8
    }
  }


//loop through the first 16 bytes
for (int i = 16; i < 32; i++)  
  {
        //only grab the first  8 bytes, which are the even ones (0,2,4,6,8,12.14.16)
        if ((i % 2) != 0){
          lc.setRow(0,(i/2)-8,imagename[i]); //the row number will be equal to (i/2)-8
    }
  }

  
}


