#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "fsmp_board/fsmpLed.h"
#include "fsmp_board/fsmpFan.h"
#include "fsmp_board/fsmpBeeper.h"
#include "fsmp_board/fsmpLight.h"
#include "fsmp_board/fsmpTempHum.h"


static const int SERVER_PORT = 8080;
fsmpLeds leds;
fsmpFan fan;
fsmpBeeper beeper;
fsmpLight lightSensor;
fsmpTempHum tempHumSensor;

bool ledState = false;
bool fanState = false;
bool beeperState = false;

void send_sensor_data(int client_fd)
{
    double temperature = tempHumSensor.temperature();
    double humidity = tempHumSensor.humidity();
    double light = lightSensor.getValue();

    char json[512] = {0};

    snprintf(json, sizeof(json),
             "{\"temperature\":%.2f,"
             "\"humidity\":%.2f,"
             "\"light\":%.2f,"
             "\"led\":%s,"
             "\"fan\":%s,"
             "\"beeper\":%s}\n",
             temperature,
             humidity,
             light,
             ledState ? "true" : "false",
             fanState ? "true" : "false",
             beeperState ? "true" : "false");

    write(client_fd, json, strlen(json));
    printf("send: %s", json);
}

void send_reply(int client_fd, const char *device, int value)
{
    char json[256] = {0};

    snprintf(json, sizeof(json),
             "{\"type\":\"reply\",\"ok\":true,\"device\":\"%s\",\"value\":%d}\n",
             device, value);

    write(client_fd, json, strlen(json));
    printf("send: %s", json);
}

void handle_message(int client_fd, const char *msg)
{
    printf("recv: %s\n", msg);

    if (strstr(msg, "\"cmd\":\"read\"") != NULL) {
        send_sensor_data(client_fd);
        return;
    }

    if (strstr(msg, "\"cmd\":\"control\"") != NULL) {
        if (strstr(msg, "\"device\":\"led\"") != NULL) {
            if (strstr(msg, "\"value\":1") != NULL) {
                leds.on(fsmpLeds::LED1);
                ledState = true;
                send_reply(client_fd, "led", 1);
            } else {
                leds.off(fsmpLeds::LED1);
                ledState = false;
                send_reply(client_fd, "led", 0);
            }
            return;
        }
        if (strstr(msg, "\"device\":\"fan\"") != NULL) {
            if (strstr(msg, "\"value\":1") != NULL) {
                fan.setSpeed(180);
                fan.start();
                fanState = true;
                send_reply(client_fd, "fan", 1);
            } else {
                fan.stop();
                fanState = false;
                send_reply(client_fd, "fan", 0);
            }
            return;
        }

        if (strstr(msg, "\"device\":\"beeper\"") != NULL) {
            if (strstr(msg, "\"value\":1") != NULL) {
                beeper.setRate(1000);
                beeper.start();
                beeperState = true;
                send_reply(client_fd, "beeper", 1);
            } else {
                beeper.stop();
                beeperState = false;
                send_reply(client_fd, "beeper", 0);
            }
            return;
        }
    }

    const char *error = "{\"type\":\"error\",\"message\":\"unknown command\"}\n";
    write(client_fd, error, strlen(error));
    printf("send: %s", error);
}

int main()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return -1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    int ret = bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret < 0) {
        perror("bind");
        close(server_fd);
        return -1;
    }

    ret = listen(server_fd, 5);
    if (ret < 0) {
        perror("listen");
        close(server_fd);
        return -1;
    }

    printf("board server started, port: %d\n", SERVER_PORT);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        printf("client connected: %s:%d\n",
               inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port));

        while (1) {
            char buf[1024] = {0};
            int len = read(client_fd, buf, sizeof(buf) - 1);

            if (len <= 0) {
                printf("client disconnected\n");
                close(client_fd);
                break;
            }

            buf[len] = '\0';
            handle_message(client_fd, buf);
        }
    }

    close(server_fd);
    return 0;
}
