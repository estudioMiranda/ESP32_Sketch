// Bibliotecas
#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <ESP_Mail_Client.h>

// Conexión WiFi
#define WIFI_SSID "studiomiranda"
#define WIFI_PASSWORD "88888888"
// Conexión Correo
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465

// Correo que envía el mensaje
#define AUTHOR_EMAIL "esp32arduinoes@gmail.com"
// Necesitamos en crear doble verificación y crear contraseña de aplicación
// Contraseña generada por Google
#define AUTHOR_PASSWORD "puktgzfjlufqsfpu"    

// Correo que recibe el mensaje
#define RECIPIENT_EMAIL "444cuatro@gmail.com"

// instancia de la clase SMTPSession
SMTPSession smtp;

// Función que recibe el estado SMTP
void smtpCallback(SMTP_Status status);

void setup(){
  Serial.begin(115200);
  Serial.println();
  Serial.print("Connecting to AP");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(200);
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /** Enable the debug via Serial port
   * none debug or 0
   * basic debug or 1
  */
  smtp.debug(1);

  /* Set the callback function to get the sending results */
  smtp.callback(smtpCallback);

  /* Declare the session config data */
  ESP_Mail_Session session;

  /* Set the session config */
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = "";

  /* Declare the message class */
  SMTP_Message message;

  // Cabecera del mensaje
  message.sender.name = "ESP";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "ESP Test Email";
  message.addRecipient("Arduinoes", RECIPIENT_EMAIL);

  // Cuerpo del Mensaje 
  String htmlMsg = "<div style=\"color:#2f4468;\"><h1>¡Hola Mundo!</h1><p>- Enviado desde la tarjeta de desarrollo ESP32 </p></div>";
  message.html.content = htmlMsg.c_str();
  message.html.content = htmlMsg.c_str();
  message.text.charSet = "us-ascii";
  message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  /* Set the custom message header */
  //message.addHeader("Message-ID: <abcde.fghij@gmail.com>");

  /* Connect to server with the session config */
  if (!smtp.connect(&session))
    return;

  /* Start sending Email and close the session */
  if (!MailClient.sendMail(&smtp, &message))
    Serial.println("Error sending Email, " + smtp.errorReason());
}

void loop(){

}

/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status){
  /* Print the current status */
  Serial.println(status.info());

  /* Print the sending result */
  if (status.success()){
    Serial.println("----------------");
    ESP_MAIL_PRINTF("El mensaje ha sido enviado con éxito: %d\n", status.completedCount());
    ESP_MAIL_PRINTF("Ha ocurrido un error al enviar el mensaje: %d\n", status.failedCount());
    Serial.println("----------------\n");
    struct tm dt;

    for (size_t i = 0; i < smtp.sendingResult.size(); i++){
      /* Get the result item */
      SMTP_Result result = smtp.sendingResult.getItem(i);
      time_t ts = (time_t)result.timestamp;
      localtime_r(&ts, &dt);

      ESP_MAIL_PRINTF("Mensaje Número: %d\n", i + 1);
      ESP_MAIL_PRINTF("Estado: %s\n", result.completed ? "success" : "failed");
      ESP_MAIL_PRINTF("Fecha/Hora: %d/%d/%d %d:%d:%d\n", dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
      ESP_MAIL_PRINTF("Receptor: %s\n", result.recipients);
      ESP_MAIL_PRINTF("Asunto: %s\n", result.subject);
    }
    Serial.println("----------------\n");
  }
}
