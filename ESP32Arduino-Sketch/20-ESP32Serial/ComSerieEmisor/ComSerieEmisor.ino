#define RXD2 16
#define TXD2 17

void setup()
{
  Serial.begin(115200);
  Serial.begin(9600, SERIAL_8N1,RXD2,TXD2);
}
void loop()
{
  Serial.println("HOLA MUNDO");
  delay(2000);
}
