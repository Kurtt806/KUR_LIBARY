#include "KUR.h"

// WebSocketsServer webSocket = WebSocketsServer(81);
// Khởi tạo các biến và cài đặt ban đầu
Ticker time_wm;
Ticker time_ws;
WiFiMulti wm;
AsyncWebServer sv(80);
AsyncWebSocket ws("/ws");

const char *ssidArray[5] = {"SSID1", "SSID2", "SSID3", "SSID4", "NGOC LAU"};
const char *passdArray[5] = {"PASS1", "PASS2", "PASS3", "PASS4", "44448888"};

const char *http_username = "admin";
const char *http_password = "admin";
// Json Variable to Hold Sensor Readings
JSONVar value;
// Variables to save values from HTML form
String message = "";
String sliderValue1 = "0";
bool newRequest = false;
// Get Sensor Readings and return JSON object
int dutyCycle1;
// setting PWM properties
const int freq = 5000;
const int ledChannel1 = 0;
const int resolution = 8;

/*==================================================================================*/
/*==================================================================================*/
/*==================================================================================*/
// Get Slider Values
String getSliderValues()
{
  value["sliderValue1"] = String(sliderValue1);
  String jsonString = JSON.stringify(value);
  return jsonString;
}

/*==================================================================================*/
/*==================================================================================*/
/*==================================================================================*/
/**
 * Hàm t_wm() là một hàm callback được sử dụng để kiểm tra kết nối Wi-Fi và thực hiện
 * các hành động sau khi kết nối được thiết lập hoặc mất kết nối.
 *
 * Hàm này thực hiện các bước sau:
 * 1. Kiểm tra trạng thái kết nối Wi-Fi bằng cách sử dụng wifiMulti (wm).
 * 2. Nếu kết nối được thiết lập, hàm sẽ in ra thông tin về kết nối Wi-Fi (SSID và RSSI)
 *    và tắt đèn LED (nếu có) trên chân 2 (digitalWrite(2, LOW)).
 * 3. Nếu kết nối bị mất, hàm sẽ in ra "WiFi not connected!" để thông báo rằng kết nối
 *    đã bị mất.
 *
 * Hàm này thường được gọi bởi một timer để kiểm tra trạng thái kết nối Wi-Fi theo khoảng
 * thời gian định sẵn. Nó có thể được sử dụng để theo dõi trạng thái kết nối và thực hiện
 * các hành động phản hồi sau khi kết nối hoặc mất kết nối.
 */
void t_wm()
{
  if (wm.run() == WL_CONNECTED)
  {
    // Kết nối Wi-Fi thành công
    Serial.print("WiFi connected: ");
    Serial.print(WiFi.SSID());
    Serial.print(" ");
    Serial.println(WiFi.RSSI());

    // Tắt đèn LED (nếu có) trên chân 2
    digitalWrite(2, LOW);
  }
  else
  {
    // Kết nối Wi-Fi bị mất
    Serial.println("WiFi not connected!");
  }
}
/*==================================================================================*/
/*==================================================================================*/
/*==================================================================================*/
void initGPIO()
{
  // configure LED PWM functionalitites
  pinMode(LED_PIN_STATE, OUTPUT);
 
  ledcSetup(ledChannel1, freq, resolution);

  // attach the channel to the GPIO to be controlled
  ledcAttachPin(LED_PIN_STATE, ledChannel1);
}
/*==================================================================================*/
/*==================================================================================*/
/*==================================================================================*/
/**
 * Hàm initSPIFFS() được sử dụng để khởi tạo hệ thống tệp hồ sơ (SPIFFS) trên thiết bị.
 *
 * Hàm này thực hiện các bước sau:
 * 1. Kiểm tra xem SPIFFS có thể được bắt đầu không (SPIFFS.begin(true)).
 * 2. Nếu SPIFFS không thể được bắt đầu, hàm in ra "SPIFFS -> FALSE!" để thông báo lỗi.
 * 3. Nếu SPIFFS được bắt đầu thành công, hàm in ra "SPIFFS -> OK!" để xác nhận.
 *
 * Hệ thống tệp hồ sơ SPIFFS thường được sử dụng để lưu trữ và quản lý dữ liệu tệp trên
 * thiết bị. Nó giúp bạn đọc và ghi dữ liệu vào bộ nhớ flash trên thiết bị một cách dễ dàng.
 */
void initSPIFFS()
{
  if (!SPIFFS.begin(true))
  {
    // Không thể khởi tạo SPIFFS, in ra thông báo lỗi
    Serial.println("SPIFFS -> FALSE!");
  }
  else
  {
    // SPIFFS được khởi tạo thành công, in ra thông báo OK
    Serial.println("SPIFFS -> OK!");
  }
}

/*==================================================================================*/
/*==================================================================================*/
/*==================================================================================*/
/**
 * Hàm initWiFi() được sử dụng để khởi tạo và thiết lập kết nối Wi-Fi trên thiết bị.
 *
 * Hàm này thực hiện các bước sau:
 * 1. Tắt chế độ Wi-Fi trước đó (nếu có).
 * 2. Thiết lập chế độ Wi-Fi trong chế độ Station (WIFI_STA).
 * 3. Thêm danh sách các mạng Wi-Fi (SSID và mật khẩu) vào mảng wifiMulti (wm).
 * 4. Quét các mạng Wi-Fi có sẵn và hiển thị thông tin về chúng.
 * 5. Kết nối đến mạng Wi-Fi có tín hiệu mạnh nhất trong danh sách.
 * 6. Bắt đầu một timer (time_wm) để quản lý kết nối Wi-Fi (t_wm).
 *
 * Lưu ý rằng hàm này giúp thiết lập kết nối Wi-Fi và quản lý nó, nhưng không thực hiện việc
 * xử lý kết nối hoặc xử lý sự kiện khi kết nối thay đổi. Các công việc cụ thể liên quan
 * đến kết nối Wi-Fi hoặc xử lý sự kiện sau khi kết nối nên được thêm vào trong các hàm khác.
 */
void initWiFi()
{
  // Tắt chế độ Wi-Fi trước đó (nếu có)
  WiFi.mode(WIFI_OFF);
  delay(300);

  // Thiết lập chế độ Wi-Fi trong chế độ Station (WIFI_STA)
  WiFi.mode(WIFI_STA);

  // Thêm danh sách các mạng Wi-Fi (SSID và mật khẩu) vào mảng wifiMulti (wm)
  for (int i = 0; i < 5; i++)
  {
    wm.addAP(ssidArray[i], passdArray[i]);
  }

  // Quét các mạng Wi-Fi có sẵn và hiển thị thông tin về chúng
  Serial.println("Scanning networks");
  int n = WiFi.scanNetworks();
  Serial.println("Scan done");

  if (n == 0)
  {
    Serial.println("No networks found");
  }
  else
  {
    Serial.print(n);
    Serial.println(" networks found");

    for (int i = 0; i < n; ++i)
    {
      // Hiển thị thông tin về mạng Wi-Fi
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
      delay(10);
    }
  }

  // Kết nối đến mạng Wi-Fi có tín hiệu mạnh nhất trong danh sách
  Serial.println("Connecting Wi-Fi...");
  if (wm.run() == WL_CONNECTED)
  {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }

  // Bắt đầu một timer (time_wm) để quản lý kết nối Wi-Fi (t_wm)
  time_wm.attach(1.0, t_wm);
}

/*==================================================================================*/
/*==================================================================================*/
/*==================================================================================*/
/*
 * Hàm notifyClients(state) gửi thông điệp đến tất cả các clients kết nối thông qua WebSocket.
 *
 * @param state Một chuỗi (String) chứa thông điệp hoặc trạng thái cần gửi đến các clients.
 *
 * Hàm này sử dụng WebSocket (ws) để gửi thông điệp (state) đến tất cả các clients
 * đã kết nối với máy chủ WebSocket. Thông điệp này có thể được sử dụng để thông báo
 * cho các clients về các sự kiện hoặc trạng thái mới trong ứng dụng. Ví dụ, bạn có thể
 * sử dụng hàm này để gửi thông báo đến các clients khi có một thay đổi trong dữ liệu
 * hoặc trạng thái của máy chủ.
 *
 * Ví dụ:
 * notifyClients("Máy chủ đã được khởi động lại.");
 * notifyClients("Dữ liệu mới đã được cập nhật.");
 *
 * Lưu ý rằng hàm này chỉ hoạt động đúng khi máy chủ WebSocket đã được thiết lập
 * và các clients đã kết nối đến nó. Hàm này không trả về giá trị và không nhận tham số
 * khác ngoài thông điệp (state) cần gửi.
 */
void notifyClients(String state)
{
  ws.textAll(state);
}

/*==================================================================================*/
/*==================================================================================*/
/*==================================================================================*/
/**
 * Hàm handleWebSocketMessage(arg, data, len) được sử dụng để xử lý tin nhắn WebSocket
 * nhận được từ một client kết nối thông qua WebSocket.
 *
 * Hàm này thực hiện các bước sau:
 * 1. Kiểm tra thông tin về khung WebSocket (AwsFrameInfo) để đảm bảo đó là khung tin nhắn cuối cùng,
 *    có độ dài phù hợp và có opcode là WS_TEXT (tin nhắn văn bản).
 * 2. Trích xuất dữ liệu tin nhắn từ mảng uint8_t *data và chuyển đổi thành chuỗi (message).
 * 3. Phân tích tin nhắn để trích xuất thông tin về số bước (steps) và hướng (direction).
 * 4. In ra thông tin số bước và hướng vào Serial để theo dõi và ghi log.
 * 5. Gửi thông báo (notifyClients()) đến tất cả các clients kết nối qua WebSocket với thông tin hướng.
 * 6. Tạo một thông điệp (sensorReadings) bằng cách gọi hàm getSensorReadings() để lấy dữ liệu cảm biến.
 * 7. In ra thông tin dữ liệu cảm biến vào Serial để theo dõi và ghi log.
 * 8. Gửi thông báo (notifyClients()) đến tất cả các clients kết nối qua WebSocket với dữ liệu cảm biến.
 *
 * Hàm này thường được sử dụng trong một máy chủ WebSocket để xử lý các tin nhắn từ clients.
 * Nó kiểm tra và xử lý tin nhắn đến và gửi dữ liệu trả lời đến các clients thông qua WebSocket.
 */
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
  {
    data[len] = 0;
    message = (char *)data;
    if (message.indexOf("1s") >= 0)
    {
      sliderValue1 = message.substring(2);
      dutyCycle1 = map(sliderValue1.toInt(), 0, 100, 0, 255);
      Serial.println(dutyCycle1);
      Serial.print(getSliderValues());
      notifyClients(getSliderValues());
    }
    if (strcmp((char *)data, "getValues") == 0)
    {
      notifyClients(getSliderValues());
    }
  }
}

/*==================================================================================*/
/*==================================================================================*/
/*==================================================================================*/
/**
 * Hàm onEvent() là một hàm callback được gọi khi có sự kiện xảy ra trong máy chủ WebSocket.
 *
 * Hàm này thực hiện xử lý sự kiện WebSocket dựa trên loại sự kiện (type) nhận được.
 * Các loại sự kiện bao gồm kết nối (connect), ngắt kết nối (disconnect), dữ liệu nhận được (data),
 * ping (pong), và lỗi (error).
 *
 * - WS_EVT_CONNECT: Được gọi khi một client kết nối thành công với máy chủ WebSocket.
 * - WS_EVT_DISCONNECT: Được gọi khi một client ngắt kết nối từ máy chủ WebSocket.
 * - WS_EVT_DATA: Được gọi khi máy chủ WebSocket nhận được dữ liệu từ một client. Hàm này
 *   gọi hàm handleWebSocketMessage() để xử lý dữ liệu.
 * - WS_EVT_PONG: Được gọi khi máy chủ WebSocket nhận được một pong từ một client sau khi
 *   gửi ping. Thường được sử dụng để kiểm tra trạng thái kết nối.
 * - WS_EVT_ERROR: Được gọi khi có lỗi xảy ra trong quá trình hoạt động của máy chủ WebSocket.
 *
 * Hàm này thường được sử dụng để theo dõi và xử lý các sự kiện từ các clients kết nối qua WebSocket.
 */
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  switch (type)
  {
  case WS_EVT_CONNECT:
    // Xử lý sự kiện kết nối của client
    Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
    break;
  case WS_EVT_DISCONNECT:
    // Xử lý sự kiện ngắt kết nối của client
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
    break;
  case WS_EVT_DATA:
    // Xử lý sự kiện nhận dữ liệu từ client
    handleWebSocketMessage(arg, data, len);
    break;
  case WS_EVT_PONG:
    // Xử lý sự kiện nhận pong từ client
    // Thường được sử dụng để kiểm tra trạng thái kết nối
    break;
  case WS_EVT_ERROR:
    // Xử lý sự kiện lỗi trong máy chủ WebSocket
    break;
  }
}

// Initialize WebSocket
void initWebSocket()
{
  ws.onEvent(onEvent);
  sv.addHandler(&ws);
}

// Start WebSocket
void runWebSocket()
{
  // Web Server Root URL
  sv.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
        { request->send(SPIFFS, "/index.html", "text/html"); });

  sv.serveStatic("/", SPIFFS, "/");
}

/*--------------------------------------------*/
KUR::KUR()
{
}
KUR::~KUR()
{
}

/*--------------------------------------------*/
void KUR::START_INIT(int EN_SERIAL)
{
  // Thực hiện một công việc nào đó
  EN_SERIAL != 0 ? Serial.begin(EN_SERIAL) : (void)0;
  initGPIO();
  initSPIFFS();
  initWiFi();
  initWebSocket();
  runWebSocket();

  sv.begin();
}

/*--------------------------------------------*/
void KUR::START_OTA()
{
  // Thực hiện một công việc nào đó
  AsyncElegantOTA.begin(&sv);
  sv.begin();
  Serial.println("server OTA opened");
}

/*--------------------------------------------*/
void KUR::RUN()
{
  ledcWrite(ledChannel1, dutyCycle1);
  ws.cleanupClients();
}