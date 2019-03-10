// Written by Stefan Schmidt 2019

// use jstest-gtk
// jscal -t /dev/input/js0
// sudo jscal-store /dev/input/js0
// jscal-restore
// /var/lib/joystick/joystick.state

#include <stdio.h>
#include "Joystick.h"
#include "Msp.h"
#include <thread>
#include <iostream>
#include <unistd.h>
#include <boost/asio/io_service.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/exception/all.hpp>
#include <exception>

#define JOYSTICK_DEVICE "/dev/input/js0"
#define RC_UPDATE_SLEEP_MS 20
#define NAZE_IP "127.0.0.1"
#define NAZE_PORT 5761


void joystick_thread(Joystick *js, bool debug)
{
    printf("axis: %i, buttons: %i\n", js->get_axis_count(), js->get_button_count());
    while (js->update())
    {
        if (debug) 
        {
            js->print_state();
        }
    }
}

int main(int argc, char *argv[])
{
    bool debug = (argc != 1);
    Joystick js(JOYSTICK_DEVICE);
    std::thread t1(joystick_thread, &js, debug);

    boost::asio::io_service ios;
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(NAZE_IP), NAZE_PORT);
    boost::asio::ip::tcp::socket socket(ios);

    while (true)
    {
        try
        {
            if (!socket.is_open())
            {
                socket.connect(endpoint);
            }

            Msp msp;
            msp.pkt_MSP_SET_RAW_RC(
                js.get_axis(Joystick::eRoll),
                js.get_axis(Joystick::ePitch),
                js.get_axis(Joystick::eThrottle),
                js.get_axis(Joystick::eYaw),
                js.get_axis(Joystick::eAux),
                js.get_button(0),
                js.get_button(1),
                js.get_button(2),
                js.get_button(3),
                js.get_button(4));

            boost::system::error_code error;
            socket.write_some(boost::asio::buffer(msp.get_packet_data(), msp.get_packet_len()), error);
            if(error.value() != 0) {
                socket.close();
            }
            else 
            {
                js.print_state();
                std::this_thread::sleep_for(std::chrono::milliseconds(RC_UPDATE_SLEEP_MS));
            }
        }
        catch (boost::exception &e)
        {
            std::cerr << boost::diagnostic_information(e);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }

    socket.close();
    t1.join();

    return 0;
}