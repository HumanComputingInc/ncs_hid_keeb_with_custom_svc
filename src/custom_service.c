#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>

#define CUSTOM_BT_KEYMAP_UUID(num) BT_UUID_DECLARE_128(BT_UUID_128_ENCODE(num, 0x14d2, 0x421e, 0xa6a0, 0xdf5f66efb305))
#define BT_UUID_CUSTOM_SVC  CUSTOM_BT_KEYMAP_UUID(0x00000000)
#define BT_UUID_CUSTOM_CHRC CUSTOM_BT_KEYMAP_UUID(0x00000001)

ssize_t write_handler(
    struct bt_conn *conn,
    const struct bt_gatt_attr *attr,
    const void *bt_buf,
    uint16_t len,
    uint16_t offset,
    uint8_t flags
) {
    printk("Recieved write request\n");
    if (flags & BT_GATT_WRITE_FLAG_PREPARE) {
        printk("Got a prepfare call, not writting to the buffer");
        return 0;
    }
    if (offset + len > 4) {
        printk("Expected an int32, got %d bytes\n", offset + len);
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }
    uint32_t buf;
    memcpy(&buf, bt_buf, len);
    printk("Got the value %d written through the custom char\n", buf);
    return len;
}

uint32_t current_val = 1;
ssize_t read_handler(
    struct bt_conn *conn,
    const struct bt_gatt_attr *attr,
    const void *bt_buf,
    uint16_t len,
    uint16_t offset
) {
    printk("Recieved read request\n");

    uint32_t to_send = ++current_val;
    printk("Wrote the value %d to the characteristic\n", to_send);
    return bt_gatt_attr_read(conn, attr, bt_buf, len, offset, &to_send, sizeof(to_send));
}

BT_GATT_SERVICE_DEFINE(custom_svc,
    BT_GATT_PRIMARY_SERVICE(BT_UUID_CUSTOM_SVC),
    BT_GATT_CHARACTERISTIC(
        BT_UUID_CUSTOM_CHRC,
        BT_GATT_CHRC_WRITE | BT_GATT_CHRC_READ,
        (BT_GATT_PERM_WRITE | BT_GATT_PERM_READ),
        read_handler, write_handler, NULL
    ),
);