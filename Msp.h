// Written by Stefan Schmidt 2019
#ifndef _MSP_H_
#define _MSP_H

#include <stdint.h>
#include <stdlib.h>
#include "msp_protocol.h"

class Msp
{
  public:
    Msp();
    ~Msp();

    void pkt_MSP_SET_RAW_RC(uint16_t roll, uint16_t pitch, uint16_t throttle, uint16_t yaw, uint16_t aux1, uint16_t aux2, uint16_t aux3, uint16_t aux4, uint16_t aux5, uint16_t aux6);

    size_t get_packet_len();
    uint8_t* get_packet_data();

protected:

    void writeU8(uint8_t val);
    void writeU16(uint16_t val);
    void writeU32(uint32_t val);

    void prepare_out_msg(uint8_t command);
    void complete_out_msg();

    uint8_t out_buffer_[255];
    uint8_t* out_ptr_;
};

#endif