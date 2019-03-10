// Written by Stefan Schmidt 2019
#include "Msp.h"

Msp::Msp()
: out_ptr_(out_buffer_)
{
}


Msp::~Msp()
{
}


void Msp::writeU8(uint8_t val)
{
    *out_ptr_++ = val;
}


void Msp::writeU16(uint16_t val)
{
    writeU8(val >> 0);
    writeU8(val >> 8);
}


void Msp::writeU32(uint32_t val)
{
    writeU8(val >> 0);
    writeU8(val >> 8);
    writeU8(val >> 16);
    writeU8(val >> 24);
}


void Msp::prepare_out_msg(uint8_t command)
{
    uint8_t *o = out_buffer_;
    *o++ = '$';
    *o++ = 'M';
    *o++ = '<';
    *o++ = 0; // size
    *o++ = command;
    out_ptr_ = o;
}


void Msp::complete_out_msg()
{
    size_t len = (out_ptr_ - out_buffer_) - 5;
    out_buffer_[3] = (uint8_t)len;
    uint8_t crc = 0;
    for (size_t i = 3; i < 3 + len + 2; i++)
    {
        crc = crc ^ out_buffer_[i];
    }
    *out_ptr_++ = crc; // 229
}


size_t Msp::get_packet_len()
{
    return out_ptr_ - out_buffer_;
}


uint8_t *Msp::get_packet_data()
{
    return out_buffer_;
}


void Msp::pkt_MSP_SET_RAW_RC(uint16_t roll, uint16_t pitch, uint16_t throttle, uint16_t yaw, uint16_t aux1, uint16_t aux2, uint16_t aux3, uint16_t aux4, uint16_t aux5, uint16_t aux6)
{
    prepare_out_msg(MSP_SET_RAW_RC);
    writeU16(roll);
    writeU16(pitch);
    writeU16(throttle);
    writeU16(yaw);
    writeU16(aux1);
    writeU16(aux2);
    writeU16(aux3);
    writeU16(aux4);
    writeU16(aux5);
    writeU16(aux6);
    writeU16(0);
    writeU16(0);
    writeU16(0);
    writeU16(0);
    writeU16(0);
    writeU16(0);
    writeU16(0);
    writeU16(0);
    complete_out_msg();
}