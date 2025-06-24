#ifndef __K210_CHECK_H__
#define __K210_CHECK_H__

#include "main.h"


typedef uint8_t (*func_cb)(void);

typedef enum
{
    CHECK_OK = 0,
    CHECK_USER_OUT,
    CHECK_TIME_OUT,

} check_error_t;


void k210_iic_init(void);




#endif /*__K210_CHECK_H__ */
