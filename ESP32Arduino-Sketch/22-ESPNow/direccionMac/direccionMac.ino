
#include "WiFi.h"
 
void setup(){
  Serial.begin(115200);
  WiFi.mode(WIFI_MODE_STA);
  Serial.println();
  Serial.println();
  Serial.print("Dirección MAC: ");
  Serial.print(WiFi.macAddress());
}
 
void loop(){

}
