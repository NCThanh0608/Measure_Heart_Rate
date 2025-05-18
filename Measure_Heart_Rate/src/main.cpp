#ifdef MAIN
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <math.h>
#include "Kalman.h"
#include "MAX30100_PulseOximeter.h"

#define REPORTING_PERIOD_MS     1
#define REFESHING_PERIOD_MS     10
#define BUZZER_PIN              27

Kalman bpmKalman;
TFT_eSPI tft = TFT_eSPI(); 
PulseOximeter pox;

uint32_t tsLastReport = 0;
uint32_t lastTFTRefesh = 0;
unsigned long lastUpdate = 0;
float heartRate;
uint8_t oxi;
int graphX = 2;         
const int graphBottom = 75;     
const int graphHeight = 15;       
const int graphLeft = 2;
const int graphRight = 158;
float lastHeartRate = 0;         
bool flag = false;
bool isClearedFlag = false;

void tft_Setup();
void buzzer_Setup();
void max30100_Setup();
void sys_Setup();
void drawAxis();
void drawGraph();
void clearGraph();

void onBeatDetected()
{
  if (millis() - tsLastReport > REPORTING_PERIOD_MS && pox.getSpO2() != 0) {
    unsigned long now = millis();
    float dt = (now - lastUpdate) / 1000.0f;
    oxi = pox.getSpO2();
    heartRate = bpmKalman.getAngle(pox.getHeartRate(), 0.0, dt);
    if(heartRate >= 110 || heartRate <= 50)
    {
      ledcWrite(0, 128);
    }
    else
    {
      ledcWrite(0, 0);
    }
    tsLastReport = millis();
    flag = true;
  }
}

void setup() 
{
  sys_Setup();
}

void loop() 
{
  pox.update();
  drawAxis();
  if(flag)
  {
    drawGraph();
    flag = false;
  }
  else if(millis() - tsLastReport > 1500)
  {
    clearGraph();
    graphX = 2;
  }
}

void tft_Setup()
{
  tft.setRotation(1);       
  tft.init();                
  tft.fillScreen(TFT_BLACK);  
  tft.drawRect(0, 0, 160, 128, TFT_BLACK);
}

void buzzer_Setup()
{
  ledcSetup(0, 2000, 8);
  ledcAttachPin(BUZZER_PIN, 0);
}

void max30100_Setup()
{
  bpmKalman.setAngle(75.0);
  Serial.print("Initializing pulse oximeter..");

  if (!pox.begin()) {
    Serial.println("FAILED");
    for(;;);
  } else {
    Serial.println("SUCCESS");
  }
  pox.setIRLedCurrent(MAX30100_LED_CURR_14_2MA);
  
  pox.setOnBeatDetectedCallback(onBeatDetected);
}

void sys_Setup()
{
  Serial.begin(115200);
  max30100_Setup();
  buzzer_Setup();
  tft_Setup();
}

void drawAxis()
{
  if(millis() - lastTFTRefesh > REFESHING_PERIOD_MS){
    tft.drawLine(1, 80, 158, 80, TFT_RED);
    tft.drawLine(1, 10, 1, 80, TFT_RED);
    tft.drawLine(158, 10, 158, 80,TFT_RED);
    tft.drawLine(1, 10, 158, 10,TFT_RED);
    tft.setTextColor(TFT_BLUE, TFT_BLACK);
    tft.setTextSize(2);
    tft.drawString("Heart", 10, 85);
    tft.drawString("Oxi", 110, 85);
    tft.drawLine(80, 80, 80, 128, TFT_RED);
    lastTFTRefesh = millis();
  }
}

void drawGraph()
{
    isClearedFlag = false;

    int constrainedHR = constrain(heartRate, 20, 160);
    int y = map(constrainedHR, 20, 160, graphBottom, graphHeight);

    int lastY = map(lastHeartRate, 20, 160, graphBottom, graphHeight);
    tft.drawLine(graphX , lastY, graphX + 2, y, TFT_BLUE);
    tft.setTextColor(TFT_BLUE, TFT_BLACK);
    tft.setTextSize(2);
    tft.drawString(String(heartRate), 10, 110);
    tft.drawString(String(oxi) + "%", 110, 110);
    lastHeartRate = heartRate;

    graphX += 2;
    if (graphX > graphRight) {
      graphX = graphLeft + 1;

      clearGraph();
    }
}

void clearGraph()
{
  if(isClearedFlag)
  {
    return;
  }
  ledcWrite(0, 0);
  tft.fillScreen(TFT_BLACK);  
  tft.drawRect(0, 0, 160, 128, TFT_BLACK);
  isClearedFlag = true;
}
#else
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <math.h>
#include "Kalman.h"

Kalman bpmKalman;
TFT_eSPI tft = TFT_eSPI();  // Khởi tạo đối tượng TFT

#include "MAX30100_PulseOximeter.h"

#define REPORTING_PERIOD_MS     1
#define REFESHING_PERIOD_MS     10
#define BUZZER_PIN              27
PulseOximeter pox;

uint32_t tsLastReport = 0;
uint32_t lastTFTRefesh = 0;
unsigned long lastUpdate = 0;
float heartRate;
uint8_t oxi;
int graphX = 2;     
const int graphBottom = 75; 
const int graphHeight = 15;        
const int graphLeft = 2;
const int graphRight = 158;
float lastHeartRate = 0;            
bool flag = false;
bool isClearedFlag = false;

void drawAxis();
void drawGraph();
void clearGraph();

void onBeatDetected(){
  if (millis() - tsLastReport > REPORTING_PERIOD_MS && pox.getSpO2() != 0) {
    unsigned long now = millis();
    float dt = (now - lastUpdate) / 1000.0f;
    oxi = pox.getSpO2();
    heartRate = bpmKalman.getAngle(pox.getHeartRate(), 0.0, dt);
    if(heartRate >= 110 || heartRate <= 50)
    {
      ledcWrite(0, 128);
    }
    else
    {
      ledcWrite(0, 0);
    }
    tsLastReport = millis();
    flag = true;
  }
}

void setup() {
  Serial.begin(115200);
  tft.setRotation(1);       
  tft.init();                
  tft.fillScreen(TFT_BLACK);  
  tft.drawRect(0, 0, 160, 128, TFT_BLACK);
  bpmKalman.setAngle(75.0);
  Serial.print("Initializing pulse oximeter..");

  if (!pox.begin()) {
      Serial.println("FAILED");
      for(;;);
  } else {
      Serial.println("SUCCESS");
  }
  pox.setIRLedCurrent(MAX30100_LED_CURR_24MA);
  
  pox.setOnBeatDetectedCallback(onBeatDetected);
  ledcSetup(0, 2000, 8);
  ledcAttachPin(BUZZER_PIN, 0);
}

void loop() {
  pox.update();
  drawAxis();
  if(flag)
  {
    drawGraph();
    flag = false;
  }
  else if(millis() - tsLastReport > 1500)
  {
    clearGraph();
    graphX = 2;
  }
}

void drawAxis(){
  if(millis() - lastTFTRefesh > REFESHING_PERIOD_MS){
    tft.drawLine(1, 80, 158, 80, TFT_RED);
    tft.drawLine(1, 10, 1, 80, TFT_RED);
    tft.drawLine(158, 10, 158, 80,TFT_RED);
    tft.drawLine(1, 10, 158, 10,TFT_RED);
    tft.setTextColor(TFT_BLUE, TFT_BLACK);
    tft.setTextSize(2);
    tft.drawString("Heart", 10, 85);
    tft.drawString("Oxi", 110, 85);
    tft.drawLine(80, 80, 80, 128, TFT_RED);
    lastTFTRefesh = millis();
  }
}

void drawGraph(){

    isClearedFlag = false;

    int constrainedHR = constrain(heartRate, 20, 160);
    int y = map(constrainedHR, 20, 160, graphBottom, graphHeight);

    int lastY = map(lastHeartRate, 20, 160, graphBottom, graphHeight);
    tft.drawLine(graphX , lastY, graphX + 2, y, TFT_BLUE);
    tft.setTextColor(TFT_BLUE, TFT_BLACK);
    tft.setTextSize(2);
    tft.drawString(String(heartRate), 10, 110);
    tft.drawString(String(oxi) + "%", 110, 110);
    lastHeartRate = heartRate;

    graphX += 2;
    if (graphX > graphRight) {
      graphX = graphLeft + 1;

      clearGraph();
    }
}

void clearGraph(){
  if(isClearedFlag)
  {
    return;
  }
  ledcWrite(0, 0);
  tft.fillScreen(TFT_BLACK);  
  tft.drawRect(0, 0, 160, 128, TFT_BLACK);
  isClearedFlag = true;
}
#endif


