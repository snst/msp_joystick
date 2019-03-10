// Written by Stefan Schmidt 2019
#include "Joystick.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

Joystick::Joystick()
{
    init();
}

Joystick::Joystick(const char *device)
{
    init();
    open_device(device);
}

Joystick::~Joystick()
{
    close(dev_);
}

void Joystick::init()
{
    dev_ = -1;
    print_count_ = 0;
    memset(axis_val_, 0, sizeof(axis_val_));
    memset(button_val_, 0, sizeof(button_val_));
    memset(axis_reverse_, 0, sizeof(axis_reverse_));
    for (int i = 0; i < (sizeof(axis_scaled_val_) / sizeof(axis_scaled_val_[0])); i++)
    {
        axis_scaled_val_[i] = JOYSTICK_MIDDLE;
    }
    axis_reverse_[eThrottle] = true;
    axis_reverse_[ePitch] = true;
}


bool Joystick::open_device(const char *device)
{
    bool ret = false;
    dev_ = open(device, O_RDONLY);

    if (-1 == dev_)
    {
        printf("Failed to open joystick: %s\n", device);
        axis_count_ = 0;
        button_count_ = 0;
    }
    else
    {
        axis_count_ = get_axis_count();
        button_count_ = get_button_count();
        ret = true;
    }
    return true;
}

void Joystick::close_device()
{
    close(dev_);
    dev_ = -1;
}


void Joystick::map_axis(struct js_event *event)
{
    float div = 32000.0 / 500.0;
    if (axis_reverse_[event->number])
    {
        div *= -1;
    }
    axis_scaled_val_[event->number] = ((float)event->value / div) + JOYSTICK_MIDDLE;
}


int32_t Joystick::get_axis(axis_t i)
{
    if (MAX_AXIS > (uint32_t)i)
    {
        return axis_scaled_val_[i];
    }
}


int32_t Joystick::get_button(uint32_t i)
{
    int32_t ret = 1000;
    if (MAX_BUTTONS > (uint32_t)i)
    {
        ret = button_val_[i] ? 1700 : 1300;
    }
    return ret;
}


bool Joystick::update()
{
    bool ret = false;
    struct js_event event;

    if (sizeof(event) == ::read(dev_, &event, sizeof(event)))
    {
        switch (event.type)
        {
        case JS_EVENT_BUTTON:
            if (MAX_BUTTONS > event.number)
            {
                button_val_[event.number] = event.value != 0;
            }
            break;
        case JS_EVENT_AXIS:
            if (2 == event.type)
            {
                if (MAX_AXIS > event.number)
                {
                    axis_val_[event.number] = event.value;
                    map_axis(&event);
                }
            }
            break;
        default:
            break;
        }
        ret = true;
    }
    return ret;
}


uint32_t Joystick::get_axis_count()
{
    uint8_t n;
    if (-1 == ioctl(dev_, JSIOCGAXES, &n))
    {
        n = 0;
    }
    return n;
}


uint32_t Joystick::get_button_count()
{
    uint8_t n;
    if (-1 == ioctl(dev_, JSIOCGBUTTONS, &n))
    {
        n = 0;
    }
    return n;
}


void Joystick::print_state()
{
    printf("state #%8d: ", print_count_++);
    /*
    for (uint32_t i = 0; i < axis_count_; i++)
    {
        printf("a[%d]=%6d ", i, axis_val_[i]);
    }

    for (uint32_t i = 0; i < button_count_; i++)
    {
        printf("b[%d]=%d ", i, button_val_[i]);
    }
    printf("\n");*/

    printf("throttle=%4d, yaw=%4d, pitch=%4d, roll=%4d, aux=%4d, btn0=%4d, btn1=%4d, btn2=%4d, btn3=%4d, btn4=%4d\n", 
        get_axis(eThrottle), get_axis(eYaw), get_axis(ePitch), get_axis(eRoll), get_axis(eAux),
        get_button(0), get_button(1), get_button(2), get_button(3), get_button(4));
}