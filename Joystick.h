// Written by Stefan Schmidt 2019
#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_

#include <stdint.h>
#include <linux/joystick.h>

#define MAX_AXIS 10
#define MAX_BUTTONS 10
#define JOYSTICK_MIDDLE 1500

class Joystick
{
public:
  enum axis_t
  {
    eYaw = 0,
    eThrottle = 1,
    eAux = 2,
    eRoll = 4,
    ePitch = 5
  };
  Joystick();
  Joystick(const char *device);
  ~Joystick();
  bool open_device(const char *device);
  void close_device(); 
  bool update();
  uint32_t get_axis_count();
  uint32_t get_button_count();
  void print_state();
  void map_axis(struct js_event *event);
  int32_t get_axis(axis_t i);
  int32_t get_button(uint32_t i);

protected:
  int dev_;
  int32_t axis_val_[MAX_AXIS];
  int32_t axis_scaled_val_[MAX_AXIS];
  bool axis_reverse_[MAX_AXIS];
  int32_t axis_count_;
  bool button_val_[MAX_BUTTONS];
  int32_t button_count_;
  uint32_t print_count_;
  void init();
};

#endif