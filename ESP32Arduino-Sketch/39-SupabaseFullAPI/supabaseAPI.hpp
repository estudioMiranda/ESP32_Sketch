
// Supabase
String url = "https://mgqasbkqqquvixgxsfgo.supabase.co/rest/v1";
String apikey = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6Im1ncWFzYmtxcXF1dml4Z3hzZmdvIiwicm9sZSI6ImFub24iLCJpYXQiOjE2NjM5NTA0NjUsImV4cCI6MTk3OTUyNjQ2NX0.IOpbZhjdAJzMzpuhbT-lbkYmlprOHEbQMNenJHxwElk";

void processResponse(int httpCode, HTTPClient& http)
{
	if (httpCode > 0) {
		Serial.printf("Código de respuesta: %d\t", httpCode);

		if (httpCode == HTTP_CODE_OK) {
		  String payload = http.getString();
			Serial.println(payload);
		}
	}
	else {
		Serial.printf("Ha ocurrido un error en la solicitud: %s\n", http.errorToString(httpCode).c_str());
	}
	http.end();
}

void GetAll() { // validado
	HTTPClient http;
	http.begin( url + "/LED");
  http.addHeader("apikey", apikey);
	int httpCode = http.GET();
	processResponse(httpCode, http);
}

void GetById(int id) {
	HTTPClient http;
	http.begin(url + "/LED" + "?id=eq." + String(id));
  http.addHeader("apikey", apikey);
	int httpCode = http.GET();
	processResponse(httpCode, http);
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
	http.begin(url + "/DHT");
  http.addHeader("apikey", apikey);
	http.addHeader("Content-Type", "application/json");
	
	int httpCode = http.POST(datosSensor);
	processResponse(httpCode, http);
}

void UpdateById(int id, String datosSensor) {  // Validado
	HTTPClient http;
	http.begin(url + "/DHT" + "?id=eq." + String(id));
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
