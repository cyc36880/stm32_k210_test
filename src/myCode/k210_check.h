#ifndef __K210_CHECK_H__
#define __K210_CHECK_H__

#include "main.h"


typedef uint8_t (*func_cb)(void);

typedef enum
{
    CHECK_OK = 0,
    CHECK_USER_OUT,
    CHECK_TIME_OUT,
    CHECK_ERROR, // 发生错误
    CHECK_FAIL,  // 查询的结果是失败
} check_error_t;


void k210_iic_init(void);
check_error_t wait_k210_online(uint32_t time_out);
check_error_t check_k210_verify(void);
check_error_t get_k210_sd_status(void);
check_error_t get_k210_wifi_status(void);
check_error_t set_k210_factory_text(void);

#endif /*__K210_CHECK_H__ */
