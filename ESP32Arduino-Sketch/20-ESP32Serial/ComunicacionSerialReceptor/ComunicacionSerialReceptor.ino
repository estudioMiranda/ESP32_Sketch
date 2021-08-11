

#define RXD2 16
#define TXD2 17

void setup() {
  
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
}

void loop() 
{ //Choose Serial1 or Serial2 as required
  Serial.print("Datos recibidos: ");
  Serial.println(Serial2.readString());
  delay(200);
}
