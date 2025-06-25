#include "stdlib.h"
#include "stdio.h"
#include "k210_check.h"
#include "IIC_Port.h"
#include "i2c.h"

static const char dev_addr = 0x24;

static const char k210_verify[] = {9,8,7,6,5,4,3,2,1,0,1,2,3,4,5,6,7,8,9};
static const uint8_t verify_reg = 0x01;
static const uint8_t sd_status_reg = 0x02;
static const uint8_t wifi_status_reg = 0x03;
static const uint8_t factory_text_reg = 182;

void k210_iic_init(void)
{
    IIC_DEVICE *iic_dev = get_IIC_Port_DevHandle(1);
    iic_dev->set_default(iic_dev);

    iic_dev->set_addr(iic_dev, dev_addr);
}

check_error_t wait_k210_online(uint32_t time_out)
{
    // IIC_DEVICE *iic_dev = get_IIC_Port_DevHandle(1);
    // uint32_t last_time = HAL_GetTick();

    uint8_t ret_val = 0;
    HAL_StatusTypeDef status = HAL_I2C_Master_Receive(&hi2c1, dev_addr<<1, &ret_val, 1, time_out);
    if (status != HAL_OK)
    {
        HAL_I2C_DeInit(&hi2c1);
        HAL_I2C_MspDeInit(&hi2c1);
        MX_I2C1_Init();
    }
    return status==HAL_OK ? CHECK_OK : CHECK_TIME_OUT;
}

check_error_t check_k210_verify(void)
{
    // IIC_DEVICE *iic_dev = get_IIC_Port_DevHandle(1);
    const uint8_t verify_len = sizeof(k210_verify);
    uint8_t verify_data[verify_len];

    // uint8_t success = 1;
    // iic_dev->readReg(iic_dev, verify_reg, verify_data, verify_len);
    // for (uint8_t i=0; i<verify_len; i++)
    // {
    //     if (verify_data[i] != k210_verify[i])
    //     {
    //         success=0;
    //     }
    // }
    // if (success)
    // {
    //     return CHECK_OK;
    // }
    // return CHECK_TIME_OUT;
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c1, dev_addr<<1, verify_reg, I2C_MEMADD_SIZE_8BIT, verify_data, verify_len, 0xf);
    if (status != HAL_OK)
    {
        HAL_I2C_DeInit(&hi2c1);
        HAL_I2C_MspDeInit(&hi2c1);
        MX_I2C1_Init();
    }
    switch (status)
    {
        case HAL_OK:
            for (uint8_t i=0; i<verify_len; i++)
            {
                if (verify_data[i] != k210_verify[i])
                {
                    return CHECK_FAIL;
                }
            }
            return CHECK_OK;
        case HAL_ERROR:
            return CHECK_ERROR;
        default:
            return CHECK_TIME_OUT;
    }
}


check_error_t get_k210_sd_status(void)
{
    // IIC_DEVICE *iic_dev = get_IIC_Port_DevHandle(1);
    uint8_t sd_status;
    // uint8_t ret_status = iic_dev->readReg(iic_dev, sd_status_reg, &sd_status, 1);

    // if (0 != ret_status)
    // {
    //     return CHECK_ERROR;
    // }
    // return sd_status==1 ? CHECK_OK : CHECK_FAIL;
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c1, dev_addr<<1, sd_status_reg, I2C_MEMADD_SIZE_8BIT, &sd_status, 1, 0xf);
    if (status != HAL_OK)
    {
        HAL_I2C_DeInit(&hi2c1);
        HAL_I2C_MspDeInit(&hi2c1);
        MX_I2C1_Init();
    }
    switch (status)
    {
        case HAL_OK:
            return sd_status==1 ? CHECK_OK : CHECK_FAIL;
        case HAL_ERROR:
            return CHECK_ERROR;
        default:
            return CHECK_TIME_OUT;
    }
}


check_error_t get_k210_wifi_status(void)
{
    // IIC_DEVICE *iic_dev = get_IIC_Port_DevHandle(1);
    uint8_t wifi_status;
    // uint8_t ret_status = iic_dev->readReg(iic_dev, wifi_status_reg, &wifi_status, 1);

    // if (0 != ret_status)
    // {
    //     return CHECK_ERROR;
    // }
    // return wifi_status==1 ? CHECK_OK : CHECK_FAIL;
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c1, dev_addr<<1, wifi_status_reg, I2C_MEMADD_SIZE_8BIT, &wifi_status, 1, 0xf);
    if (status != HAL_OK)
    {
        HAL_I2C_DeInit(&hi2c1);
        HAL_I2C_MspDeInit(&hi2c1);
        MX_I2C1_Init();
    }
    switch (status)
    {
        case HAL_OK:
            return wifi_status==1 ? CHECK_OK : CHECK_FAIL;
        case HAL_ERROR:
            return CHECK_ERROR;
        default:
            return CHECK_TIME_OUT;
    }
}

check_error_t set_k210_factory_text(void)
{
    uint8_t k210_factory_reg_val = 1;
    // IIC_DEVICE *iic_dev = get_IIC_Port_DevHandle(1);

    // uint8_t ret_val = iic_dev->writeReg(iic_dev, factory_text_reg, &k210_factory_reg_val, 1);
    // if (0 != ret_val)
    // {
    //     return CHECK_ERROR;
    // }
    // ret_val = iic_dev->readReg(iic_dev, factory_text_reg, &k210_factory_reg_val, 1);
    // if (0 != ret_val)
    // {
    //     return CHECK_ERROR;
    // }

    HAL_StatusTypeDef status =  HAL_I2C_Mem_Write(&hi2c1, dev_addr<<1, factory_text_reg, I2C_MEMADD_SIZE_8BIT, &k210_factory_reg_val, 1, 0xf);
    if (status != HAL_OK)
    {
        HAL_I2C_DeInit(&hi2c1);
        HAL_I2C_MspDeInit(&hi2c1);
        MX_I2C1_Init();
    }
    status = HAL_I2C_Mem_Read(&hi2c1, dev_addr<<1, factory_text_reg, I2C_MEMADD_SIZE_8BIT, &k210_factory_reg_val, 1, 0xf);
    if (status != HAL_OK)
    {
        HAL_I2C_DeInit(&hi2c1);
        HAL_I2C_MspDeInit(&hi2c1);
        MX_I2C1_Init();
    }
    switch (status)
    {
        case HAL_OK:
            return k210_factory_reg_val==1 ? CHECK_OK : CHECK_FAIL;
        case HAL_ERROR:
            return CHECK_ERROR;
        default:
            return CHECK_TIME_OUT;
    }
}



void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
    HAL_I2C_DeInit(&hi2c1);
    HAL_I2C_MspDeInit(&hi2c1);
    MX_I2C1_Init();
}
