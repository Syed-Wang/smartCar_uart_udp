#include "udp_send_task.h"
#include "cJSON.h"
#include "cmsis_os2.h"
#include "lwip/api_shell.h"
#include "lwip/netifapi.h"
#include "lwip/sockets.h"
#include "ohos_init.h"
#include "sys_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct sockaddr_in client; // 客户端
#define TASK_DELAY_TIME (50 * 1000)

void udp_send_task(void)
{
    int ret = 0;
    while (1) {
        cJSON* json_root = cJSON_CreateObject();
        cJSON* json_speed = cJSON_CreateObject();
        if (json_root) {
            cJSON_AddItemToObject(json_root, "carStatus", cJSON_CreateString(get_CurrentCarStatus(systemValue)));
            cJSON_AddItemToObject(json_root, "carSpeed", json_speed);
            cJSON_AddItemToObject(json_speed, "left", cJSON_CreateNumber(systemValue.left_motor_speed));
            cJSON_AddItemToObject(json_speed, "right", cJSON_CreateNumber(systemValue.right_motor_speed));
            cJSON_AddItemToObject(json_root, "carPower", cJSON_CreateNumber(systemValue.battery_voltage));
            cJSON_AddItemToObject(json_root, "distance", cJSON_CreateNumber(systemValue.distance));
        }
        char* payload = cJSON_PrintUnformatted(json_root);

        ret = sendto(systemValue.udp_socket_fd, payload, strlen(payload), 0,
            (struct sockaddr*)&client, sizeof(client));
        cJSON_Delete(json_root);
        free(payload);
        usleep(TASK_DELAY_TIME); // 50ms
    }
    closesocket(systemValue.udp_socket_fd);
}
