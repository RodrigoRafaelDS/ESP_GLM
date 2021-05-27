#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <SPI.h>
#include <FS.h>
#include "SD.h"
#include <WiFi.h>

// //Provide the token generation process info.
// #include "addons/TokenHelper.h"
// //Provide the RTDB payload printing info and other helper functions.
// #include "addons/RTDBHelper.h"

/* 1. Define the WiFi credentials */
#define WIFI_SSID "Sfox_Rodrigo"
#define WIFI_PASSWORD "SFOX@123"

/** 2. Define the Service Account credentials (required for token generation)
 * 
 * This information can be taken from the service account JSON file.
 * 
 * To download service account file, from the Firebase console, goto project settings, 
 * select "Service accounts" tab and click at "Generate new private key" button
*/

const char *FIREBASE_FCM_SERVER_KEY = "AAAAG12ImPk:APA91bEV2sn75FQcKhTBPSalwWM9mZQpMW3HBzIvpkWq8qUV_vcMzQXW8HASbdJng50As1Du2gwpY0TQc_DhljnplriDm6Hsa-vublPrgazVcDISHa6b09v_qUVopXNoiG-3hJYvsMH5";
// const char *DEVICE_REGISTRATION_ID_TOKEN ="cvK-rg0OS6unVMcDVjCovE:APA91bG8ZYUhVHx2-cnqzmBPOWA3vb0M9DGcPMPkz3hPsmjTmaiu00hw_0d0f9aYB4mFmcOFGW0NFNLnBWx8VlY97fdxrJZt-ydMxNRv2YWf5KsYBP3qsSB7GHoiMbuU9Cm8M2HCh08P";

// const char *host = "https://esp32-fe16a-default-rtdb.firebaseio.com/";
// const char *auth2 = "9rFkkxT7bXr4zA8uwy9gJfk6ytMRvnwUHLNhy9s2";

// #define API_KEY "AIzaSyApp7aNhQeQ8h4zoN6BTENx94yOgMuE7Ag"

// #define FIREBASE_PROJECT_ID "glm-easy"
// #define FIREBASE_CLIENT_EMAIL "https://firebase-adminsdk-r8lw9@glm-easy.iam.gserviceaccount.com";
// const char PRIVATE_KEY[] PROGMEM ="-----BEGIN PRIVATE KEY-----\nMIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQCUoaMzGP2/KfR9\nuetiFlZytXfvzgA1q+jqiw2HDf34GGHBAz+2SOYQeMb8Cb2B7ZANiHnvlAq2zHl/\nzByJWxI79z4eE70aM0dQptjmK9QkJloXcvg/IcFXACPGnmdWjnkeJrTnf4YTi6aq\nY4wiUspjHMr2I0KzPIkzFWoY+8jj/LVN63qn3EoEsAdoil7eCB9xgIADBOO+Pko/\ntJ98Q/IonhzAY1xvM5LgwZU4GO2sh2TOf3fA2BL32huxajyvN4v/Up1LPw6wMYvI\nTN1pKtWIIW/1L0+219d3eZ6Rv3jtcbYb/rn09gW1HPqHFV1J9rMDErCttp+MoJ5n\niOUuwGJFAgMBAAECggEAA4S3OuY6wFC7Vrj7YOByF9fcSMZen00JGXkRm+kLFXMD\nG8rHS+0tc3LFR+gb67x2h+t7WRGIsDHmjZqEvmrafQyDeu2XfcxJW7GaoHA/K4/n\nQapy3l1R/1lwf14pyHtc7W3+Vrb55iW/9atsZzYAg0u5ZKdwoV+SFXrkGEboxx5s\ne0ljfck8bRF/4JCDpysN4liM40s+0KIQWUmUsxeqOH48hbuMpyHXpeeUL+TFBbnP\niiNVvChzTwjJyixLJW9OkSRnQMWrs2tRUI0viVe5/rwkVXkbvNf6y5WS+4M7gWp5\nAS/IngSzWmkTeQbu+l2IYIiUOYyErL3t8pTf6Jer7QKBgQDOvRv24ztqOo/P37sz\nwo++ngna2ihEMlrmkSl/pKUVSm3Xy0s9IWQAPH61U0fL/jdxI+bgAjKl4X1Jq5x/\naWIw1jW6ryNP2KWnPFPYybGtLa3BA5S21CurFVMPmho/4/scnINubhowFMegGu/b\n7uLnoea6yvWeO/O06HJ6I6YKWwKBgQC4DAe4tUtUb5zG89ZtEMrxFGgV44Bt7knd\nb5YdyLWVMYsW7JFXuYPeUzPoyuJW9tAwRHDx2+v5bssLa5o8YIYNCSNro9fyRyhe\nSAmBOBDyOBtTD/3/siwu9qqsOelvlQBQwDOMC9bSo0QL05uvrMXrDQ5D4ee/ANeA\n5LLNR9Sn3wKBgFN+hKC9PO22IQavMQfCeBNf+FLu8Hw6agPcdIockJ98o8gXTpet\nuJlv1SyeKcz1KqcC/OdTe+kKnHL4YAgFI6bdnG189iH1OVHFR4eyTZPHcvkU3SbJ\noalCv3z5m8hZ5ryzaVFcHi909PpwZpbIb9Nky6588Z2qUM6rYjB53UEpAoGBAKXb\nkYnPN1ecTbitPWuEWp7vOO2g27g1qbM4QBwPKDSPNT0va7uWn1PXv0vmqhqcyfj1\ndKNBAWMDiU99ZL1MbwGIXrc+K/lWTvbIDGi60eq2zUkJfGjVdCPOfvg+KDQ6HtKm\nos1uTeX84/GUfRTPqnysg1KBXosugwbREqEUPnwZAoGADAhz4oIUcsxSqCkKp4jZ\nF6N6f3tBfAtqfNtMtuZ+vQOiJlTeu7nLvUJJqd89Z2gD1pYn+yDV9u7W5884dRVc\nV9vNI5PVLUvH0/djs+geD3twXZyE9jVcea80U8UP6F+oIXk1MtxY0K3kHW3UmcEd\n4Abg2KEuAjZE/WtblHU8of4=\n-----END PRIVATE KEY-----\n";

/* 3. Define the ID token for client or device to send the message */
#define DEVICE_REGISTRATION_ID_TOKEN "f3TZE2XUQniKFENjAO8XGj:APA91bGipMAVpkQGKs42y_0Lu7J4qMVLBOG5qL_XjkwuCJid1K7t5Az_JZadZvvCpNMtp9xxkHhSTP_mX3mn56OdlE0Vt0q5qF_v0q53I61UfGMoIxZew9GRcns5GggGEanbMPYYQfHF";
// const char *host "https://esp32-fe16a-default-rtdb.firebaseio.com/";
// const char *auth = "9rFkkxT7bXr4zA8uwy9gJfk6ytMRvnwUHLNhy9s2";
/* 4. Define the Firebase Data object */
FirebaseData fbdo;

/* 5. Define the FirebaseAuth data for authentication data */
FirebaseAuth auth;

/* 6. Define the FirebaseConfig data for config data */
FirebaseConfig config;

unsigned long lastTime = 0;

int count = 0;

void sendMessage();

void setup()
{

    Serial.begin(115200);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

    /* Assign the sevice account credentials and private key (required) */

    /* Assign the callback function for the long running token generation task */

    // config.api_key=API_KEY;
    // auth.user.email = USER_EMAIL;
    // auth.user.password = USER_PASSWORD;
    // config.host=host;
    // auth.token.uid=auth2;

    // // fbdo.setResponseSize(4096);

    // config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

    // /** Assign the maximum retry of token generation */
    // // config.max_token_generation_retry = 5;

    // /* Initialize the library with the Firebase authen and config */
    // Firebase.begin(host,auth2);

    Firebase.FCM.setServerKey(FIREBASE_FCM_SERVER_KEY);
    Firebase.reconnectWiFi(true);
    sendMessage();
}

void loop()
{

    //   if (Firebase.ready() && (millis() - lastTime > 10 * 1000))
    //   {
    //     lastTime = millis();

    sendMessage();
    //   }
}

void sendMessage()
{

    Serial.print("Send Firebase Cloud Messaging... ");

    //Read more details about legacy HTTP API here https://firebase.google.com/docs/cloud-messaging/http-server-ref
    FCM_Legacy_HTTP_Message msg;

    // msg.targets.to = DEVICE_REGISTRATION_ID_TOKEN;
    msg.targets.to = "/topics/testEPS32_GLM";
    msg.options.time_to_live = "1000";
    msg.options.priority = "high";

    FirebaseJson json;
    String payload;
    
    msg.payloads.notification.title = "Notification title";
    msg.payloads.notification.body = "Notification body";

    // msg.payloads.notification.icon = "myicon";
    // msg.payloads.notification.click_action = "OPEN_ACTIVITY_1";

    //all data key-values should be string
    json.add("itemID", count);
    json.add("btID", "02");
    json.add("msg","testando 123");
    // json.add("timestamp", "1609815454");
    json.toString(payload);
    msg.payloads.data = payload.c_str();

    if (Firebase.FCM.send(&fbdo, &msg))
    { //send message to recipient
        Serial.printf("ok\n%s\n\n", Firebase.FCM.payload(&fbdo).c_str());
        Serial.println(msg.payloads.data);
    }
    else
        Serial.println(fbdo.errorReason());

    count++;
    delay(5000);
}