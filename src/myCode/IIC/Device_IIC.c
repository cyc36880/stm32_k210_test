/*
 * @Author       : 蔡雅超 (ZIShen)
 * @LastEditors  : zishen
 * @Date         : 2024-09-21 12:24:15
 * @LastEditTime : 2025-06-24 22:35:17
 * @Description  : 
 * Copyright (c) 2024 Author 蔡雅超 email: 2672632650@qq.com, All Rights Reserved.
 */

#include "Device_IIC.h"


static void _IIC_Start(_SOFT_IIC_DRIVE *self)
{
//    xSemaphoreTake(port_iic_semaphore, portMAX_DELAY); // 信号量

    self->_SDA_output(self);
    
    self->_SDA_write(self, 1);
    self->_SCL_write(self, 1);
    self->_Delay(self);
    self->_SDA_write(self, 0);
    self->_Delay(self);
    self->_SCL_write(self, 0);
}

static void _IIC_Stop(_SOFT_IIC_DRIVE *self)
{
    self->_SDA_output(self);

    self->_SDA_write(self, 0);
    self->_SCL_write(self, 1);
    self->_Delay(self);
    // self->_SCL_write(self, 1);
    self->_SDA_write(self, 1);
    self->_Delay(self);
    
//    xSemaphoreGive(port_iic_semaphore); // 信号量
}

// 
static void _IIC_Ack(_SOFT_IIC_DRIVE *self)
{
    // self->_SDA_output(self);

    // self->_SDA_write(self, 0);
    // self->_Delay(self);
    // self->_SCL_write(self, 1);
    // self->_Delay(self);
    // self->_SCL_write(self, 0);
    // self->_Delay(self);

    self->_SCL_write(self, 0);
    self->_SDA_output(self);
    self->_SDA_write(self, 0);
    self->_Delay(self);
    self->_SCL_write(self, 1);
    self->_Delay(self);
    self->_SCL_write(self, 0);
}

static void _IIC_NAck(_SOFT_IIC_DRIVE *self)
{
    // self->_SDA_output(self);

    // self->_SDA_write(self, 1);
    // self->_Delay(self);
    // self->_SCL_write(self, 1);
    // self->_Delay(self);
    // self->_SCL_write(self, 0);
    // self->_Delay(self);
    self->_SCL_write(self, 0);
    self->_SDA_output(self);
    self->_SDA_write(self, 1);
    self->_Delay(self);
    self->_SCL_write(self, 1);
    self->_Delay(self);
    self->_SCL_write(self, 0);
}

static uint8_t _IIC_WaitAck(_SOFT_IIC_DRIVE *self)
{
    uint8_t timeout=0;
    uint8_t ack = IIC_NACK;

    self->_SDA_input(self);
    self->_SDA_write(self, 1);
    self->_Delay(self);
    self->_SCL_write(self, 1);
    self->_Delay(self);

    do {
        ack = self->_SDA_read(self);
        if(ack == IIC_ACK) break;
        self->_Delay(self);
    }while((++timeout) < self->waitack_delayUnits);

    self->_SCL_write(self, 0);
    self->_Delay(self);

    return ack;

}

static void _IIC_SendByte(_SOFT_IIC_DRIVE *self, uint8_t byte)
{
    // self->_SDA_output(self);
    // for(uint8_t i=0; i<8; i++)
    // {
    //     self->_SCL_write(self, 0);
    //     self->_Delay(self);

    //     self->_SDA_write(self, byte >> 7);
    //     byte <<= 1;

    //     self->_Delay(self);

    //     self->_SCL_write(self, 1);
    //     self->_Delay(self);
    // }
    // self->_SCL_write(self, 0);
    // self->_Delay(self);
    self->_SDA_output(self);
    self->_SCL_write(self, 0);

    for(uint8_t i=0; i<8; i++)
    {
        self->_SDA_write(self, byte >> 7);
        byte <<= 1;
        self->_Delay(self);
        self->_SCL_write(self, 1);
        self->_Delay(self);
        self->_SCL_write(self, 0);
        self->_Delay(self);
    }

}

static uint8_t _IIC_ReadByte(_SOFT_IIC_DRIVE *self, uint8_t ack)
{
    uint8_t byte = 0;

    self->_SDA_write(self, 1);
    self->_SDA_input(self);
    
    for(uint8_t i=0; i<8; i++)
    {
        byte <<= 1;
        self->_SCL_write(self, 1);
        self->_Delay(self);
        if(self->_SDA_read(self))
        {
            byte |= 0x01;
        }
        self->_SCL_write(self, 0);
        self->_Delay(self);
    }
    if(ack == IIC_ACK) self->ack(self);
    else self->nack(self);

    return byte;
    
   
    // uint8_t byte = 0;

    // self->_SDA_input(self);
    // for(uint8_t i=0; i<8; i++)
    // {
    //     self->_SCL_write(self, 0);
    //     self->_Delay(self);
    //     self->_SCL_write(self, 1);
    //     // self->_Delay(self);
    //     byte <<= 1;
    //     if(self->_SDA_read(self))
    //     {
    //         byte++;
    //     }
    //     self->_Delay(self);
    // }
    // if(ack == IIC_ACK) self->ack(self);
    // else self->nack(self);

    // return byte;
}

//  设置恢复到默认值
static void _IIC_set_default(_SOFT_IIC_DRIVE *self)
{
    self->delay_time_us = 100;
    self->waitack_delayUnits = 0;
}

static void _IIC_set_DelayTime_us(_SOFT_IIC_DRIVE *self, uint8_t us)
{
    self->delay_time_us = us;
}
static void _IIC_set_waitack_delayUnits(_SOFT_IIC_DRIVE *self, uint8_t units)
{
    self->waitack_delayUnits = units;
}

// ====================================================================

static void _Device_set_default(IIC_DEVICE *self)
{
    if(self->iic_drive->set_default)
        self->iic_drive->set_default(self->iic_drive);

    if(self->Device_Destructors) { // 析构回调
        self->Device_Destructors(self);
        self->Device_Destructors = NULL;
    }
    
    self->set_addr(self, 0xff);
}

static void _IIC_set_address(IIC_DEVICE *self, uint8_t address)
{
    self->device_address = address;
}


static uint8_t _IIC_writeBytes(IIC_DEVICE *self, uint8_t *data, uint16_t len)
{
    if(self->iic_drive == NULL) return 1;

    _SOFT_IIC_DRIVE *drv = self->iic_drive;

    for( ; ; )
    {
        drv->start(drv);
        drv->send_byte(drv, (self->device_address<<1) | IIC_W);
        if(drv->wait_ack(drv)) break;

        for(uint8_t i=0; i<len; i++)
        {
            drv->send_byte(drv, data[i]);
            if(drv->wait_ack(drv)) break;
        }
        drv->stop(drv);

        return 0;
    }

    drv->stop(drv);

    return 1;
}

static uint8_t _IIC_readBytes(IIC_DEVICE *self, uint8_t *data, uint16_t len)
{
    if(self->iic_drive == NULL) return 1;

    _SOFT_IIC_DRIVE *drv = self->iic_drive;

    for( ; ; )
    {
        drv->start(drv);
        drv->send_byte(drv, (self->device_address<<1) | IIC_R);
        if(drv->wait_ack(drv)) break;

        uint8_t i=0;
        if(len > 1)
        {
            for( ; i<len-1; i++)
            {
                data[i] = drv->read_byte(drv, IIC_ACK);
            }
        }
        data[i] = drv->read_byte(drv, IIC_NACK);
        drv->stop(drv);
        return 0;
    }

    drv->stop(drv);
    return 1;
}

static uint8_t _IIC_write_double(IIC_DEVICE *self, uint8_t *reg_dat, uint8_t reg_len, uint8_t *dat, uint16_t data_len)
{
    if (self->iic_drive == NULL) return 1;
    _SOFT_IIC_DRIVE *drv = self->iic_drive;

    // ----
    drv->start(drv);
    drv->send_byte(drv, (self->device_address<<1) | IIC_W);
    if (IIC_NACK == drv->wait_ack(drv)) 
    {
        goto ack_error;
    }

    for (uint8_t i=0; i<reg_len; i++)
    {
        drv->send_byte(drv, reg_dat[i]);
        if(IIC_NACK == drv->wait_ack(drv))
        {
            goto ack_error;
        }
    }
    // ----
    for (uint8_t i=0; i<data_len; i++)
    {
        drv->send_byte(drv, dat[i]);
        if(IIC_NACK == drv->wait_ack(drv))
        {
            goto ack_error;
        }
    }
    drv->stop(drv);
    return 0;


ack_error:
    drv->stop(drv);
    return 1;
}

static uint8_t _IIC_read_double(IIC_DEVICE *self, uint8_t *reg_dat, uint8_t reg_len, uint8_t *dat, uint16_t data_len)
{
    if(self->iic_drive == NULL) return 1;

    _SOFT_IIC_DRIVE *drv = self->iic_drive;

    // ----
    drv->start(drv);
    drv->send_byte(drv, (self->device_address<<1) | IIC_W);
    if (IIC_NACK == drv->wait_ack(drv))
    {
        goto ack_error;
    }

    for (uint8_t i=0; i<reg_len; i++)
    {
        drv->send_byte(drv, reg_dat[i]);
        if(IIC_NACK == drv->wait_ack(drv))
        {
            goto ack_error;
        }
    }
    // ---
    drv->start(drv);
    drv->send_byte(drv, (self->device_address<<1) | IIC_R);
    if (IIC_NACK == drv->wait_ack(drv))
    {
        goto ack_error;
    }

    uint8_t i=0;
    if (data_len > 1)
    {
        for ( ; i<data_len-1; i++)
        {
            dat[i] = drv->read_byte(drv, IIC_ACK);
        }
    }
    dat[i] = drv->read_byte(drv, IIC_NACK);
    drv->stop(drv);
    
    return 0;


ack_error:
    drv->stop(drv);
    return 1;
}

static uint8_t _IIC_writeReg(IIC_DEVICE *self, uint8_t reg, uint8_t *data, uint16_t len)
{
   return self->write_double(self, &reg, 1, data, len);
}


static uint8_t _IIC_readReg(IIC_DEVICE *self, uint8_t reg, uint8_t *data, uint16_t len)
{
    return self->read_double(self, &reg, 1, data, len);
}


/*************************************************************
 * @description: 扫描得到一个设备最低地址的设备addr
 * @param {IIC_DEVICE} *iic_device
 * @return {uint8_t} 0xff 无设备，其它设备地址
 *************************************************************/
static uint8_t scan_DevLowAddr(IIC_DEVICE *self)
{
    if(self->iic_drive == NULL) return 0xff;
    
    _SOFT_IIC_DRIVE *dev = self->iic_drive;
    
    uint8_t device_id = 0xff;
    for(uint8_t i=0; i<=0x7f; i++)
    {
        dev->start(dev);
        dev->send_byte(dev, (i<<1) | IIC_R);
        if(dev->wait_ack(dev) == IIC_ACK) 
        {
            dev->stop(dev);
            device_id = i;
            break;
        }
        dev->stop(dev);
    }
    
    return device_id;
}

/*************************************************************
 * @description: 更新设备最低地址的设备addr
 * @param {IIC_DEVICE} *iic_device
 * @return {*} 0:成功，1:失败
 *************************************************************/
static uint8_t update_DevLowAddr(IIC_DEVICE *self)
{
    uint8_t addr = self->scan_LowAddr(self);
    if(addr == 0xff) return 1;
    
    self->set_addr(self, addr);
    
    return 0;
}

/*************************************************************
 * @description: 根据设备地址列表更新设备地址
 * @param {IIC_DEVICE} *iic_device
 * @param {uint8_t} *addr_list 地址列表
 * @param {uint8_t} len 地址列表长度
 * @return {*} 0:成功，1:失败
 *************************************************************/
static uint8_t update_DevAddrFromDevList(IIC_DEVICE *self, const uint8_t *addr_list, uint8_t len)
{
    if(self->iic_drive == NULL) return 1;
    
    _SOFT_IIC_DRIVE *drv = self->iic_drive;

    for(uint8_t i=0; i<len; i++)
    {
        drv->start(drv);
        drv->send_byte(drv, (addr_list[i]<<1) | IIC_R);
        if(drv->wait_ack(drv) == IIC_ACK)
        {
            drv->stop(drv);
            self->device_address = addr_list[i];
            return 0;
        }
        drv->stop(drv);
    }
    drv->stop(drv);
    return 1;
}


/*************************************************************
 * @description: 检查设备是否在线
 * @param {IIC_DEVICE} *iic_device
 * @return {*} 1 在线，0 离线
 *************************************************************/
static uint8_t IIC_Device_isOnline(IIC_DEVICE *self)
{
    uint8_t ack = 0;

    if(self->iic_drive == NULL) return !IIC_DEVICE_ONLINE;

    _SOFT_IIC_DRIVE *drv = self->iic_drive;

    drv->start(drv);
    drv->send_byte(drv, (self->device_address<<1) | IIC_R);
    ack = drv->wait_ack(drv);
    drv->read_byte(drv, IIC_NACK);
    drv->stop(drv);

    // printf("port %d, IIC Device %d is %s\n", (uint32_t)self->user_data, self->device_address, ack==IIC_ACK ? "Online" : "Offline");

    return ack==IIC_ACK ? IIC_DEVICE_ONLINE : !IIC_DEVICE_ONLINE;
}

/**
 * [ _SOFT_IIC_DRIVE _IIC_Drive_init = .. ]
 * _SOFT_IIC_DRIVE _IIC_SDA_input = ..
 * 
 *          ......
 * 
 * _SOFT_IIC_DRIVE _IIC_SDA_read = ..
 * 
 * 
 * IIC_Drive_Add (self)
 * 
*/

void IIC_Drive_Add(IIC_DEVICE_LIST *IIC_Device_List, IIC_DEVICE *iic_device)
{
    if(IIC_Device_List->iic_device_header == NULL) {
        IIC_Device_List->iic_device_header = iic_device;
    }
    else {
        IIC_Device_List->iic_device_tail->next = iic_device;
    }
    IIC_Device_List->iic_device_tail = iic_device;

    if(iic_device->iic_drive)
    {
        _SOFT_IIC_DRIVE *self = iic_device->iic_drive;
        
        if(self->Drive_init) self->Drive_init(self);

        self->start     = _IIC_Start;
        self->stop      = _IIC_Stop;
        self->nack      = _IIC_NAck;
        self->ack       = _IIC_Ack;
        self->wait_ack  = _IIC_WaitAck;
        self->send_byte = _IIC_SendByte;
        self->read_byte = _IIC_ReadByte;

        self->set_default            = _IIC_set_default;
        self->set_DelayTime_us       = _IIC_set_DelayTime_us;
        self->set_waitack_delayUnits = _IIC_set_waitack_delayUnits;
    }

    if(iic_device->Device_init) {
        iic_device->Device_init();
    }
    iic_device->writeBytes      =  _IIC_writeBytes;
    iic_device->readBytes       =  _IIC_readBytes;
    iic_device->write_double    =  _IIC_write_double;
    iic_device->read_double     =  _IIC_read_double;
    iic_device->writeReg        =  _IIC_writeReg;
    iic_device->readReg         =  _IIC_readReg;
    iic_device->isOnline        = IIC_Device_isOnline;
    iic_device->scan_LowAddr    = scan_DevLowAddr;
    iic_device->update_LowAddr  = update_DevLowAddr;
    iic_device->update_AddrFromDevList = update_DevAddrFromDevList;
    iic_device->set_addr        = _IIC_set_address;
    iic_device->set_default     = _Device_set_default;
    iic_device->next            = NULL;
}




/*************************************************************
 * @description: 获取iic设备的数量
 * @return {*}
 *************************************************************/
uint8_t IIC_Get_DeviceList_Num(IIC_DEVICE_LIST *IIC_Device_List)
{
    if(IIC_Device_List->iic_device_header == NULL) return 0;
    IIC_DEVICE *iic_device = IIC_Device_List->iic_device_header ;
    uint8_t num = 0;

    for( ; num<255; )
    {
        num++;       
        if(iic_device->next == NULL) break;
        iic_device = iic_device->next;
    }
    return num;
}

/*************************************************************
 * @description: 根据插入顺序，获取iic设备
 * @param {uint8_t} id
 * @return {*}
 *************************************************************/
IIC_DEVICE *IIC_Get_DeviceInList(IIC_DEVICE_LIST *IIC_Device_List, uint8_t id)
{
    if(IIC_Device_List->iic_device_header == NULL) return NULL;

    IIC_DEVICE *iic_device = IIC_Device_List->iic_device_header ;
    for(int i=0; i<id; i++)
    {
        if(iic_device->next == NULL) return NULL;
        iic_device = iic_device->next;
    }
    return iic_device;
}



