
#include "RCOutput.h"

using namespace Empty;

void RCOutput::init() {}

void RCOutput::set_freq(uint32_t chmask, uint16_t freq_hz) {}

uint16_t RCOutput::get_freq(uint8_t ch) {
    return 50;
}

void RCOutput::enable_ch(uint8_t ch)
{}

void RCOutput::disable_ch(uint8_t ch)
{}

void RCOutput::write(uint8_t ch, uint16_t period_us)
{
    if (ch < 16) {
        _pwm_output[ch] = period_us;
    }
}

uint16_t RCOutput::read(uint8_t ch) {
    if (ch >= 16) {
        return 900;
    }
    return _pwm_output[ch];
}

void RCOutput::read(uint16_t* period_us, uint8_t len)
{}

