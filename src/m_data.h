#ifndef _M_DATA_H_
#define _M_DATA_H_

struct m_data
{
    unsigned char type;
    char from_id[6];
    char to_id[6];
    void *data;
}

#endif