/*
  ============================================================
        ESP32 TELEGRAM REMOTE LIGHT CONTROL
        C++ / Arduino Framework
  ============================================================

  Features:
  - Wi-Fi connection
  - Telegram Bot
  - /start command
  - ON / OFF buttons
  - Admin User ID protection
  - GPIO output for LOW-VOLTAGE relay module
  - Automatic Wi-Fi reconnect

  IMPORTANT:
  ESP32 GPIO is 3.3V logic.
  Do NOT connect a mains circuit directly to an ESP32 GPIO.
*/

// ============================================================
//                    CONFIGURATION
// ============================================================

const char* WIFI_SSID     = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

const char* BOT_TOKEN     = "YOUR_TELEGRAM_BOT_TOKEN";

// Put your Telegram numeric User ID here
const char* ADMIN_USER_ID = "YOUR_TELEGRAM_USER_ID";

// GPIO connected to your LOW-VOLTAGE relay module
const int RELAY_PIN = 23;

// Change these if your relay module works opposite
const int RELAY_ON  = HIGH;
const int RELAY_OFF = LOW;

// ============================================================

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

unsigned long lastTelegramCheck = 0;

// ============================================================
//                    WIFI CONNECT
// ============================================================

void connectWiFi() {

  Serial.println();
  Serial.println("Connecting to Wi-Fi...");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi connected.");

  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());
}

// ============================================================
//                    ACCESS CHECK
// ============================================================

bool isAdmin(String userId) {

  return userId == String(ADMIN_USER_ID);
}

// ============================================================
//                    TELEGRAM MENU
// ============================================================

void sendMainMenu(String chatId) {

  String keyboard = "[["
                    "{\"text\":\"🟢 LIGHTS ON\",\"callback_data\":\"LIGHT_ON\"},"
                    "{\"text\":\"🔴 LIGHTS OFF\",\"callback_data\":\"LIGHT_OFF\"}"
                    "]]";

  bot.sendMessageWithInlineKeyboard(
    chatId,
    "🏠 *Home Control*\n\nChoose an action:",
    "Markdown",
    keyboard
  );
}

// ============================================================
//                    SET RELAY
// ============================================================

void setRelay(bool state) {

  if (state) {

    digitalWrite(RELAY_PIN, RELAY_ON);

    Serial.println("Relay: ON");

  } else {

    digitalWrite(RELAY_PIN, RELAY_OFF);

    Serial.println("Relay: OFF");
  }
}

// ============================================================
//                    TELEGRAM HANDLER
// ============================================================

void handleNewMessages(int count) {

  for (int i = 0; i < count; i++) {

    String chatId = bot.messages[i].chat_id;
    String userId = bot.messages[i].from_id;
    String text   = bot.messages[i].text;

    Serial.println();
    Serial.println("Telegram message received");
    Serial.print("User ID: ");
    Serial.println(userId);
    Serial.print("Message: ");
    Serial.println(text);

    // --------------------------------------------------------
    // ADMIN SECURITY
    // --------------------------------------------------------

    if (!isAdmin(userId)) {

      bot.sendMessage(
        chatId,
        "⛔ Access denied.",
        ""
      );

      continue;
    }

    // --------------------------------------------------------
    // /start
    // --------------------------------------------------------

    if (text == "/start") {

      bot.sendMessage(
        chatId,
        "🤖 *ESP32 Home Control*\n\n"
        "Telegram remote control is ready.",
        "Markdown"
      );

      sendMainMenu(chatId);
    }

    // --------------------------------------------------------
    // /on
    // --------------------------------------------------------

    else if (text == "/on") {

      setRelay(true);

      bot.sendMessage(
        chatId,
        "🟢 Lights control: ON",
        ""
      );
    }

    // --------------------------------------------------------
    // /off
    // --------------------------------------------------------

    else if (text == "/off") {

      setRelay(false);

      bot.sendMessage(
        chatId,
        "🔴 Lights control: OFF",
        ""
      );
    }

    // --------------------------------------------------------
    // BUTTON CALLBACK
    // --------------------------------------------------------

    else if (bot.messages[i].type == "callback_query") {

      String callbackData = bot.messages[i].text;

      if (callbackData == "LIGHT_ON") {

        setRelay(true);

        bot.answerCallbackQuery(
          bot.messages[i].query_id,
          "Lights ON"
        );

        bot.sendMessage(
          chatId,
          "🟢 Lights are ON.",
          ""
        );
      }

      else if (callbackData == "LIGHT_OFF") {

        setRelay(false);

        bot.answerCallbackQuery(
          bot.messages[i].query_id,
          "Lights OFF"
        );

        bot.sendMessage(
          chatId,
          "🔴 Lights are OFF.",
          ""
        );
      }
    }

    // --------------------------------------------------------
    // UNKNOWN COMMAND
    // --------------------------------------------------------

    else {

      bot.sendMessage(
        chatId,
        "Unknown command.\n\nUse /start",
        ""
      );
    }
  }
}

// ============================================================
//                         SETUP
// ============================================================

void setup() {

  Serial.begin(115200);

  // Relay GPIO
  pinMode(RELAY_PIN, OUTPUT);

  // Start in OFF state
  digitalWrite(RELAY_PIN, RELAY_OFF);

  // Wi-Fi
  connectWiFi();

  // Telegram HTTPS certificate checking
  // For a simple ESP32 project.
  client.setInsecure();

  Serial.println();
  Serial.println("=================================");
  Serial.println(" ESP32 TELEGRAM HOME CONTROL");
  Serial.println("=================================");
  Serial.println("System ready.");
}

// ============================================================
//                          LOOP
// ============================================================

void loop() {

  // ----------------------------------------------------------
  // Wi-Fi reconnect
  // ----------------------------------------------------------

  if (WiFi.status() != WL_CONNECTED) {

    Serial.println("Wi-Fi disconnected.");
    connectWiFi();
  }

  // ----------------------------------------------------------
  // Check Telegram
  // ----------------------------------------------------------

  if (millis() - lastTelegramCheck > 1000) {

    int newMessages = bot.getUpdates(bot.last_message_received + 1);

    while (newMessages) {

      handleNewMessages(newMessages);

      newMessages = bot.getUpdates(
        bot.last_message_received + 1
      );
    }

    lastTelegramCheck = millis();
  }
}