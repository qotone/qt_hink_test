#ifndef SOUNDBUFFER_H
#define SOUNDBUFFER_H

#include <QVector>
class SoundBuffer
{
public:
    SoundBuffer(uint16_t m_size = 0);
    bool write(uint16_t pos, char value);
    char read(uint16_t pos);
private:
    uint16_t m_size;
    QVector<char> m_buffer;
};

#endif // SOUNDBUFFER_H
