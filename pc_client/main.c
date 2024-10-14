#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/rfcomm.h>


int main(void)
{

    int dev_id = hci_get_route(NULL);
    int hci_sock = hci_open_dev(dev_id);
    if (dev_id < 0 || hci_sock < 0) {
        perror("Socket");
        exit(-1);
    }

    printf("Scanning...\n");
    int max_rsp = UINT8_MAX;
    inquiry_info *ii = (inquiry_info *)malloc(sizeof(inquiry_info) * max_rsp);
    int len = 8;
    int flags = IREQ_CACHE_FLUSH;
    int num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
    if (num_rsp < 0) {
        perror("Inquiry");
    }

    char addr_str[19] = {0};
    char name[248] = {0};
    printf(" ID  |        Address       |         Name\n");
    printf("---------------------------------------------------\n");
    for (int i = 0; i < num_rsp; i++) {
        ba2str(&(ii + i)->bdaddr, addr_str);
        memset(name, 0, sizeof(name));
        if (hci_read_remote_name(hci_sock, &(ii + i)->bdaddr, sizeof(name), name, 0) < 0) {
            strcpy(name, "[Unknown]");
        }
        printf(" %-3d | %-20s | %-s\n", i, addr_str, name);
    }

    bdaddr_t bt_addr;
    int id;
    printf("Select device to connect to: ");
    while (1) {
        scanf("%d", &id);
        if ((id < num_rsp) && (id >= 0)) {
            bacpy(&bt_addr, &(ii + id)->bdaddr);
            break;
        } else {
            printf("There is no device with such ID! Try again: ");
        }
    }
    free(ii);
    close(hci_sock);

    ba2str(&bt_addr, addr_str);
    printf("Connecting to %s... ", addr_str);
    int sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    if (sock < 0) {
        perror("Sock RFCOMM");
        exit(-2);
    }

    struct sockaddr_rc addr = { 0 };
    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = (uint8_t) 1;
    bacpy(&addr.rc_bdaddr, &bt_addr);
    int status = connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    if (status == 0) {
        printf("Done\n");
    } else {
        perror("Connect");
        exit(-3);
    }

    char buffer[512] = {0};

    while (1) {
        read(sock, buffer, sizeof(buffer));
        printf("%s", buffer);
        memset(buffer, 0, sizeof(buffer));
        scanf("%s", buffer);
        if (!strcmp(buffer, "quit")) break;
        write(sock, buffer, strlen(buffer));
    }

    printf("Connection closed!\n");
    close(sock);

    return 0;
}