#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

// Need to connec to the wifi somehow, set it up here
WiFiClient wifiClient;
String esid  = "Kavin";
String epass = "1123581321";

// splunk settings and http collector token
String collectorToken = "fc87cd31-cb8a-4870-9efd-36ab953eea71";
String splunkindexer = "192.168.43.229";
String eventData="";

//you need a different client per board
String clientName ="01";

  
void setup()
{  
  //serial because seeing text telling us its working is good
  Serial.begin(115200); // Reduce this if your Arduino has trouble talking so fast
  Serial.println("splunk hec");
  initWiFi();
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  //^^ did i get an IP "its working!!"
}

void loop()
{
  // build the event data, telemtry and metrics type of data goes below
  String msgString ="this event is from arduino";
  eventData="\"clientname\": \""+clientName + "\",\"message_recieved\": \""+String(msgString)+"\"";
  Serial.println(eventData);
  //send off the data
  splunkpost(collectorToken,eventData,clientName,splunkindexer); 
  delay(10000); 
  // ^^ hard work, your deserve a nap

}

void initWiFi(){
  Serial.println();
  Serial.println("Wifi Startup");
  esid.trim();
  if ( esid.length() > 1 ) {
      // test esid 
      WiFi.disconnect();
      delay(100);
      WiFi.mode(WIFI_STA);
      Serial.print("Connecting to WiFi ");
      Serial.println(esid);
      WiFi.begin(esid.c_str(), epass.c_str());
      if ( testWifi() == 20 ) { 
          return;
      }
  }
}

String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}

int testWifi(void) {
  int c = 0;
  Serial.println("Wifi test...");  
  while ( c < 30 ) {
    if (WiFi.status() == WL_CONNECTED) { return(20); } 
    delay(500);
    Serial.print(".");    
    c++;
  }
  Serial.println("WiFi Connect timed out");
  return(10);
} 

void splunkpost(String collectorToken,String PostData, String Host, String splunkindexer)
{
  // recieved the token, post data clienthost and the splunk indexer  
  String payload = "{ \"event\": {" + PostData + "}}";
  
  //Build the request
  HTTPClient http;
  String splunkurl="http://"+ splunkindexer +":8088/services/collector/event";
  String tokenValue="Splunk " + collectorToken;
  
  // fire at will!! 
  http.begin(splunkurl);
  http.addHeader("Content-Type", "application/json");
  Serial.println(tokenValue);
  http.addHeader("Authorization", tokenValue);
  Serial.println(payload);
  String contentlength = String(payload.length());
  http.addHeader("Content-Length", contentlength );
  http.POST(payload);
  http.writeToStream(&Serial);
  http.end();
 
}
