#include "SoundBuffer.h"

SoundBuffer::SoundBuffer(uint16_t size)
{
    m_size = size;
    m_buffer.resize(size);

}

bool SoundBuffer::write(uint16_t pos, char value)
{
    if(pos < m_size){
        m_buffer[pos] = value;
        return true;
    }
    else
        return false;
}

char SoundBuffer::read(uint16_t pos)
{
    return m_buffer.at(pos);
}
