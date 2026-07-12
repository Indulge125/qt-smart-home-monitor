#ifndef ENV_H
#define ENV_H

#include <linux/input.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/gpio.h>
#include <sys/ioctl.h>

class fsmpEvents {

public:
    explicit fsmpEvents() {
        // 初始化按键文件描述符为-1
        key_1_fd = -1;
        key_2_fd = -1;
        key_3_fd = -1;

        // 打开GPIO设备并初始化按键
        gpio_init();
    }

    ~fsmpEvents() {
        close_all();
    }

    // 获取按键状态
    // 返回值：1-按键1按下，2-按键2按下，3-按键3按下，0-无按键
    int get_key_pressed() {
        // 检查按键1（边缘触发）
        if (key_1_fd >= 0) {
            struct gpioevent_data event_data;
            ssize_t ret = read(key_1_fd, &event_data, sizeof(event_data));
            if (ret > 0 && event_data.id == 2) {  // 下降沿=按下
                return 1;
            }
        }

        // 检查按键2（电平触发）
        if (key_2_fd >= 0) {
            struct gpiohandle_data data;
            int ret = ioctl(key_2_fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data);
            if (ret == 0 && data.values[0] == 0) {  // 低电平=按下
                return 2;
            }
        }

        // 检查按键3（边缘触发）
        if (key_3_fd >= 0) {
            struct gpioevent_data event_data;
            ssize_t ret = read(key_3_fd, &event_data, sizeof(event_data));
            if (ret > 0 && event_data.id == 2) {  // 下降沿=按下
                return 3;
            }
        }

        return 0;  // 无按键按下
    }

    // 等待按键按下（阻塞）
    int wait_for_key() {
        fd_set fds;
        int max_fd = -1;

        while (1) {
            FD_ZERO(&fds);

            if (key_1_fd >= 0) {
                FD_SET(key_1_fd, &fds);
                if (key_1_fd > max_fd) max_fd = key_1_fd;
            }
            if (key_3_fd >= 0) {
                FD_SET(key_3_fd, &fds);
                if (key_3_fd > max_fd) max_fd = key_3_fd;
            }

            if (max_fd < 0) return -1;

            // 等待事件
            int ret = select(max_fd + 1, &fds, NULL, NULL, NULL);
            if (ret > 0) {
                // 检查按键1
                if (key_1_fd >= 0 && FD_ISSET(key_1_fd, &fds)) {
                    struct gpioevent_data event_data;
                    read(key_1_fd, &event_data, sizeof(event_data));
                    if (event_data.id == 2) return 1;
                }

                // 检查按键3
                if (key_3_fd >= 0 && FD_ISSET(key_3_fd, &fds)) {
                    struct gpioevent_data event_data;
                    read(key_3_fd, &event_data, sizeof(event_data));
                    if (event_data.id == 2) return 3;
                }
            }

            // 轮询按键2
            if (key_2_fd >= 0) {
                struct gpiohandle_data data;
                ioctl(key_2_fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data);
                if (data.values[0] == 0) return 2;
            }
        }
    }

private:
    bool gpio_init() {
        // 打开GPIO chip5
        int fd = open("/dev/gpiochip5", O_RDONLY | O_NONBLOCK);
        if (fd < 0) {
            fprintf(stderr, "Failed to open /dev/gpiochip5\n");
            return false;
        }

        int ret;
        struct gpioevent_request event_req;
        struct gpiohandle_request req;

        // ========== 初始化按键1 (line 9) ==========
        memset(&event_req, 0, sizeof(event_req));
        event_req.lineoffset = 9;
        event_req.handleflags = GPIOHANDLE_REQUEST_INPUT;
        event_req.eventflags = GPIOEVENT_REQUEST_FALLING_EDGE;  // 下降沿触发
        ret = ioctl(fd, GPIO_GET_LINEEVENT_IOCTL, &event_req);
        if (ret == -1) {
            fprintf(stderr, "Failed to init key1 (line 9)\n");
        } else {
            key_1_fd = event_req.fd;
            // 设置非阻塞
            int flags = fcntl(key_1_fd, F_GETFL, 0);
            flags |= O_NONBLOCK;
            fcntl(key_1_fd, F_SETFL, flags);
            printf("Key1 initialized\n");
        }

        // ========== 初始化按键2 (line 7) ==========
        memset(&req, 0, sizeof(req));
        req.lineoffsets[0] = 7;
        req.flags = GPIOHANDLE_REQUEST_INPUT;
        req.lines = 1;
        ret = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &req);
        if (ret == -1) {
            fprintf(stderr, "Failed to init key2 (line 7)\n");
        } else {
            key_2_fd = req.fd;
            printf("Key2 initialized\n");
        }

        // ========== 初始化按键3 (line 8) ==========
        memset(&event_req, 0, sizeof(event_req));
        event_req.lineoffset = 8;
        event_req.handleflags = GPIOHANDLE_REQUEST_INPUT;
        event_req.eventflags = GPIOEVENT_REQUEST_FALLING_EDGE;  // 下降沿触发
        ret = ioctl(fd, GPIO_GET_LINEEVENT_IOCTL, &event_req);
        if (ret == -1) {
            fprintf(stderr, "Failed to init key3 (line 8)\n");
        } else {
            key_3_fd = event_req.fd;
            // 设置非阻塞
            int flags = fcntl(key_3_fd, F_GETFL, 0);
            flags |= O_NONBLOCK;
            fcntl(key_3_fd, F_SETFL, flags);
            printf("Key3 initialized\n");
        }

        close(fd);  // 关闭chip文件描述符，保留event fd
        return true;
    }

    void close_all() {
        if (key_1_fd >= 0) close(key_1_fd);
        if (key_2_fd >= 0) close(key_2_fd);
        if (key_3_fd >= 0) close(key_3_fd);
        printf("All GPIO closed\n");
    }

private:
    int key_1_fd;
    int key_2_fd;
    int key_3_fd;
};

#endif // ENV_H
