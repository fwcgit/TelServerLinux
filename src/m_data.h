#ifndef _M_DATA_H_
#define _M_DATA_H_

#ifdef __cplusplus
extern "C" {
#endif
struct m_data
{
    unsigned char type;
    char from_id[6];
    char to_id[6];
    void *data;
};
#ifdef __cplusplus
}
#endif
#endif