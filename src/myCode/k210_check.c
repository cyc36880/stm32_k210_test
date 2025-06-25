#include "stdlib.h"
#include "stdio.h"
#include "k210_check.h"
#include "IIC_Port.h"

static const char dev_addr = 0x34;

static const char k210_verify[] = {9,8,7,6,5,4,3,2,1,0,1,2,3,4,5,6,7,8,9};
static const char verify_reg = 0x03;
static const char sd_status_reg = 0x02;
static const char wifi_status_reg = 0x03;

void k210_iic_init(void)
{
    IIC_DEVICE *iic_dev = get_IIC_Port_DevHandle(1);
    iic_dev->set_default(iic_dev);

    iic_dev->set_addr(iic_dev, dev_addr);
}

static check_error_t wait_k210_online(uint32_t time_out, func_cb func)
{
    IIC_DEVICE *iic_dev = get_IIC_Port_DevHandle(1);
    uint32_t last_time = HAL_GetTick();

    while ( (HAL_GetTick() - last_time) < time_out )
    {
        if (func && func() != 0) 
        {
            return CHECK_USER_OUT;
        }
        if (IIC_DEVICE_ONLINE == iic_dev->isOnline(iic_dev) )
        {
            return CHECK_OK;
        }    
    }
    return CHECK_TIME_OUT;
}

static check_error_t check_k210_verify(uint32_t time_out, func_cb func)
{
    IIC_DEVICE *iic_dev = get_IIC_Port_DevHandle(1);
    const uint8_t verify_len = sizeof(k210_verify);
    uint8_t verify_data[verify_len];

    uint32_t last_time = HAL_GetTick();

    while ( (HAL_GetTick() - last_time) < time_out )
    {
        uint8_t success = 1;
        iic_dev->readReg(iic_dev, verify_reg, verify_data, verify_len);
        if (func && func() != 0)
        {
            return CHECK_USER_OUT;
        }
        for (uint8_t i=0; i<verify_len; i++)
        {
            if (verify_data[i] != k210_verify[i])
            {
                success=0;
            }
        }
        if (success)
        {
            return CHECK_OK;
        }
        HAL_Delay(50);
    }
    return CHECK_TIME_OUT;
}


static int get_k210_sd_status(void)
{
    IIC_DEVICE *iic_dev = get_IIC_Port_DevHandle(1);


}

check_error_t check_k210(uint32_t time_out)
{
    IIC_DEVICE *iic_dev = get_IIC_Port_DevHandle(1);




}
