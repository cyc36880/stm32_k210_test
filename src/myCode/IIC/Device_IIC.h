/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : zishen
 * @Date         : 2024-09-21 12:24:15
 * @LastEditTime : 2025-06-24 22:35:27
 * @Description  : 
 * Copyright (c) 2024 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */

#ifndef _DEVICE_IIC_H_
#define _DEVICE_IIC_H_

/**
 * 单个总线上，默认只支持挂载一个设备
 * 对于多个设备的操作，需要自己实现
 * 
*/

#include <stdint.h>
#include <stdlib.h>

#define IIC_W 0
#define IIC_R 1 

#define IIC_ACK  0
#define IIC_NACK 1

#define IIC_DEVICE_ONLINE 1


typedef struct _IIC_DEVICE       IIC_DEVICE;
typedef struct __SOFT_IIC_DRIVE  _SOFT_IIC_DRIVE;


// iic 设备列表
typedef struct _IIC_DEVICE_LIST
{
    IIC_DEVICE *iic_device_header;
    IIC_DEVICE *iic_device_tail;
}IIC_DEVICE_LIST;


struct __SOFT_IIC_DRIVE
{
    // ==== 以下由用户提供
    void (*Drive_init)(_SOFT_IIC_DRIVE *self); // 端口初始化

    void (*_SDA_input)(_SOFT_IIC_DRIVE *self);
    void (*_SDA_output)(_SOFT_IIC_DRIVE *self);
    void (*_Delay)(_SOFT_IIC_DRIVE *self);

    void (*_SCL_write)(_SOFT_IIC_DRIVE *self, uint8_t);
    void (*_SDA_write)(_SOFT_IIC_DRIVE *self, uint8_t);
    uint8_t (*_SDA_read)(_SOFT_IIC_DRIVE *self);

    uint8_t delay_time_us; //通信延时
    uint8_t waitack_delayUnits; //等待应答时间 多少个delay_time_us单位

    uint8_t user_data;
    // =============================

    void (*start)(struct __SOFT_IIC_DRIVE *self);
    void (*stop)(struct __SOFT_IIC_DRIVE *self);
    void (*ack)(struct __SOFT_IIC_DRIVE *self);
    void (*nack)(struct __SOFT_IIC_DRIVE *self);
    uint8_t (*wait_ack)(struct __SOFT_IIC_DRIVE *self);
    void (*send_byte)(struct __SOFT_IIC_DRIVE *self, uint8_t);
    uint8_t (*read_byte)(struct __SOFT_IIC_DRIVE *self, uint8_t);

    void (*set_default)(struct __SOFT_IIC_DRIVE *self);
    void (*set_DelayTime_us)(struct __SOFT_IIC_DRIVE *self, uint8_t);
    void (*set_waitack_delayUnits)(struct __SOFT_IIC_DRIVE *self, uint8_t);
};


struct _IIC_DEVICE
{
    uint8_t device_address; // 七位设备地址 不要直接赋值，需要调用set_addr函数
    _SOFT_IIC_DRIVE *iic_drive; // 软件模拟驱动
    // ==== 以下由用户提供
   
    void (*Device_init)(void); // 设备初始化
    void (*Device_Destructors)(struct _IIC_DEVICE *self); //析构函数
    void *user_data;
    // =============================

    struct _IIC_DEVICE *next;
    void    (*set_addr)(struct _IIC_DEVICE *self, uint8_t); // 设置设备地址
    uint8_t (*scan_LowAddr)(struct _IIC_DEVICE *self); // 扫描得到一个低地址设备的addr
    uint8_t (*update_LowAddr)(struct _IIC_DEVICE *self); // 更新一个低地址设备的addr
    uint8_t (*update_AddrFromDevList)(struct _IIC_DEVICE *self, const uint8_t *addr_list, uint8_t len);// 根据设备列表更新设备地址
    uint8_t (*isOnline)(struct _IIC_DEVICE *self);

    uint8_t (*writeBytes)(struct _IIC_DEVICE *self, uint8_t *data, uint16_t len);
    uint8_t (*readBytes)(struct _IIC_DEVICE *self, uint8_t *data, uint16_t len);
    uint8_t (*write_double)(IIC_DEVICE *self, uint8_t *reg_dat, uint8_t reg_len, uint8_t *dat, uint16_t data_len);
    uint8_t (*read_double)(IIC_DEVICE *self, uint8_t *reg_dat, uint8_t reg_len, uint8_t *dat, uint16_t data_len);
    uint8_t (*writeReg)(struct _IIC_DEVICE *self, uint8_t reg, uint8_t *data, uint16_t len);
    uint8_t (*readReg)(struct _IIC_DEVICE *self, uint8_t reg, uint8_t *data, uint16_t len);

    void    (*set_default)(struct _IIC_DEVICE *self);
};


uint8_t IIC_Get_DeviceList_Num(IIC_DEVICE_LIST *IIC_Device_List);
IIC_DEVICE *IIC_Get_DeviceInList(IIC_DEVICE_LIST *IIC_Device_List, uint8_t id);

void IIC_Drive_Add(IIC_DEVICE_LIST *IIC_Device_List, IIC_DEVICE *iic_device);


#endif


