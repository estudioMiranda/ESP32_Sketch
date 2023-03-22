
// Supabase
String url = "https://vommeepgegbaadwlmcns.supabase.co/rest/v1";
String apikey = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6InZvbW1lZXBnZWdiYWFkd2xtY25zIiwicm9sZSI6ImFub24iLCJpYXQiOjE2NTc1NTU0NDUsImV4cCI6MTk3MzEzMTQ0NX0.COSffX_5xvZOJrqt7ddbwGV32KggfYK-TeZRTUyeC_c";

void processResponse(int httpCode, HTTPClient& http)
{
	if (httpCode > 0) {
		Serial.printf("Código de respuesta: %d\t", httpCode);
    Serial.println();

		if (httpCode == HTTP_CODE_OK) {
		  String payload = http.getString();
			  // Serial.println(payload);
      char documentoJson[500];
        payload.replace(" ", "");
        payload.replace("\n", "");
        payload.trim();
        payload.remove(0,1);
        payload.remove(payload.length()-1, 1);
        payload.toCharArray(documentoJson, 500);

        Serial.println(payload);
        
        StaticJsonDocument<200> doc;
        deserializeJson( doc, documentoJson);
		}
	}
	else {
		Serial.printf("Ha ocurrido un error en la solicitud: %s\n", http.errorToString(httpCode).c_str());
	}
	http.end();
}

void GetAll() { // validado
	HTTPClient http;
	http.begin( url + "/esp32");
  http.addHeader("apikey", apikey);
	int httpCode = http.GET();
	processResponse(httpCode, http);
}

String GetById(int id) {
	HTTPClient http;
	http.begin(url + "/esp32" + "?id=eq." + String(id));
  http.addHeader("apikey", apikey);
	int httpCode = http.GET();
	if (httpCode > 0) {
    Serial.printf("Código de respuesta: %d\t", httpCode);
    Serial.println();
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      return payload;
      Serial.println(payload);
    }
  }
  else {
    Serial.printf("Ha ocurrido un error en la solicitud: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}

void GetQuery(String filter) {
	HTTPClient http;
	http.begin(url + "/DHT?select=" + filter);
  http.addHeader("apikey", apikey);
	int httpCode = http.GET();
	processResponse(httpCode, http);
}

void Add(String datosSensor)
{ 
	HTTPClient http;
	http.begin(url + "/esp32");
  http.addHeader("apikey", apikey);
	http.addHeader("Content-Type", "application/json");
	
	int httpCode = http.POST(datosSensor);
	processResponse(httpCode, http);
}

void UpdateById(int id, String datosSensor) {  // Validado
	HTTPClient http;
	http.begin(url + "/esp32" + "?id=eq." + String(id));
  http.addHeader("apikey", apikey);
	http.addHeader("Content-Type", "application/json");
  	
	int httpCode = http.PATCH(datosSensor);
	processResponse(httpCode, http);
}

void DeleteById(int id) {   // Validado
	HTTPClient http;
	http.begin(url + "/DHT" + "?id=eq." + String(id));
  http.addHeader("apikey", apikey);
	int httpCode = http.sendRequest("DELETE");
	processResponse(httpCode, http);
}
