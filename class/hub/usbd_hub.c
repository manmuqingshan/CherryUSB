#include "usbd_core.h"
#include "usbd_hub.h"

static struct usb_hub_descriptor hub_desc = {
    .bLength = 0x09,
    .bDescriptorType = HUB_DESCRIPTOR_TYPE_HUB,
    .bNbrPorts = 4,
    .wHubCharacteristics = HUB_CHAR_PORTIND | HUB_CHAR_TTTT_32_BITS,
    .bPwrOn2PwrGood = 0x32,
    .bHubContrCurrent = 0x64,
    .DeviceRemovable = 0x00,
    .PortPwrCtrlMask = 0xff
};

int hub_custom_request_handler(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    USBD_LOG_DBG("HUB Class Custom request: "
                 "bRequest 0x%02x\r\n",
                 setup->bRequest);

    if (((setup->bmRequestType & USB_REQUEST_TYPE_MASK) == USB_REQUEST_CLASS) &&
        ((setup->bmRequestType & USB_REQUEST_RECIPIENT_MASK) == USB_REQUEST_TO_DEVICE) &&
        (setup->bRequest == HUB_REQUEST_GET_DESCRIPTOR)) {
        uint8_t value = (uint8_t)(setup->wValue >> 8);
        uint8_t intf_num = (uint8_t)setup->wIndex;

        switch (value) {
            case HUB_DESCRIPTOR_TYPE_HUB:
                *data = (uint8_t *)&hub_desc;
                *len = hub_desc.bLength;
                break;
            default:
                return -1;
        }
        return 0;
    }

    else if (((setup->bmRequestType & USB_REQUEST_TYPE_MASK) == USB_REQUEST_CLASS) &&
             ((setup->bmRequestType & USB_REQUEST_RECIPIENT_MASK) == USB_REQUEST_TO_OTHER)) {
        uint8_t hub_port_feature = (uint8_t)(setup->wValue);
        uint8_t hub_port = (uint8_t)setup->wIndex;

        switch (setup->bRequest) {
            case HUB_REQUEST_GET_STATUS:
                break;
            case HUB_REQUEST_CLEAR_FEATURE:
                break;
            case HUB_REQUEST_SET_FEATURE:
                break;
            default:
                USBD_LOG_WRN("Unhandled HUB Class Custom bRequest 0x%02x\r\n", setup->bRequest);
                return -1;
        }
        return 0;
    }
    return -1;
}

void hub_notify_handler(uint8_t event, void *arg)
{
    switch (event) {
        case USBD_EVENT_RESET:

            break;
        default:
            break;
    }
}

void usbd_hub_add_interface(usbd_class_t *class, usbd_interface_t *intf)
{
    static usbd_class_t *last_class = NULL;

    if (last_class != class) {
        last_class = class;
        usbd_class_register(class);
    }

    intf->class_handler = NULL;
    intf->custom_handler = hub_custom_request_handler;
    intf->vendor_handler = NULL;
    intf->notify_handler = hub_notify_handler;
    usbd_class_add_interface(class, intf);
}