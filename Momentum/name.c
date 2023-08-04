#include "usb_names.h"

#define MIDI_NAME   {'M','o','m','e','n','t','u','m'}
#define MIDI_NAME_LEN  8

#define MANUFACTURER_NAME  {'E','l','e','c','t','r','o','t','e','c','h','n','i','q','u','e'}
#define MANUFACTURER_NAME_LEN 16

struct usb_string_descriptor_struct usb_string_product_name = {
  2 + MIDI_NAME_LEN * 2,
  3,
  MIDI_NAME
};

struct usb_string_descriptor_struct usb_string_manufacturer_name = {
  2 + MANUFACTURER_NAME_LEN * 2,
  3,
  MANUFACTURER_NAME
};
