/*************************************************************
 * @LastEditors: 蔡雅超
 * @Date: 2024-05-12 10:10:48
 * @LastEditTime: 2024-10-10 17:10:07
 *************************************************************/
#include "IIC_Port.h"

#include "delay.h"

struct IIC_GPIO_PortPin_
{
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;
};

struct IIC_GPIO_Group_
{
    struct IIC_GPIO_PortPin_ SCL;
    struct IIC_GPIO_PortPin_ SDA;
};

static const struct IIC_GPIO_Group_ _IIC_GPIO_Group[] = {
    {.SCL={GPIOB, GPIO_PIN_12},  .SDA={GPIOB, GPIO_PIN_13}},
};


// iic 端口操作句柄
static PORT_IIC_Dev port_iic_dev = {0};


// iic 驱动接口
static void IIC_SDA_input(_SOFT_IIC_DRIVE *iic_drive)
{
    UNUSED(iic_drive);
    // SDA_IN();
}

static void IIC_SDA_output(_SOFT_IIC_DRIVE *iic_drive)
{
    UNUSED(iic_drive);
    // SDA_OUT();
}

// sda 读
static uint8_t IIC_SDA_read(_SOFT_IIC_DRIVE *iic_drive)
{
    const struct IIC_GPIO_PortPin_ *gpio = &_IIC_GPIO_Group[iic_drive->user_data].SDA;
    
    return HAL_GPIO_ReadPin((GPIO_TypeDef*)gpio->GPIOx, gpio->GPIO_Pin);
}

// sda 写
static void IIC_SDA_write(_SOFT_IIC_DRIVE *iic_drive, uint8_t val)
{
    const struct IIC_GPIO_PortPin_ *gpio = &_IIC_GPIO_Group[iic_drive->user_data].SDA;

    HAL_GPIO_WritePin((GPIO_TypeDef*)gpio->GPIOx, gpio->GPIO_Pin, val ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

// scl 写
static void IIC_SCL_wrire(_SOFT_IIC_DRIVE *iic_drive, uint8_t val)
{
    const struct IIC_GPIO_PortPin_ *gpio = &_IIC_GPIO_Group[iic_drive->user_data].SCL;

    HAL_GPIO_WritePin((GPIO_TypeDef*)gpio->GPIOx, gpio->GPIO_Pin, val ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static void IIC_Delay(_SOFT_IIC_DRIVE *iic_drive)
{
    delay_us(iic_drive->delay_time_us);
}


void IIC_Port_Init(void)
{
    _SOFT_IIC_DRIVE *drive;
    for(uint8_t i=0; i<IIC_PORT_NUM; i++)
    {
        drive = &port_iic_dev.port_iic_drv[i];
        // 配置iic端口
        drive->_SDA_input = IIC_SDA_input;
        drive->_SDA_output = IIC_SDA_output;
        drive->_SDA_read = IIC_SDA_read;
        drive->_SDA_write = IIC_SDA_write;
        drive->_SCL_write = IIC_SCL_wrire;

        // 配置iic延时
        drive->_Delay = IIC_Delay;

        // user_data  ，区分不同的iic端口
        drive->user_data = i;

        // 将iic端口接入到对应的iic设备
        port_iic_dev.portDev[i].dev.iic_drive = drive;

        // 将iic设备加入到列表
        IIC_Drive_Add(&port_iic_dev.port_iic_device_list, &port_iic_dev.portDev[i].dev);

        port_iic_dev.portDev[i].dev.set_default(&port_iic_dev.portDev[i].dev);// 设置默认值
    }
}



/*************************************************************
 * @description: 更新端口的IIC状态，是否有设备接入
 * @param {uint8_t} ms       函数执行间隔时间，单位ms
 * @param {uint8_t} interval 函数执行间隔，单位ms
 * @return {*}
 *************************************************************/
void update_IIC_Port_DevSta(uint8_t ms, uint8_t interval)
{
    port_iic_dev.count_ms += ms;
    if(port_iic_dev.count_ms >= interval) {
        port_iic_dev.count_ms = 0;
    }
    else {
        return;
    }

    for(uint8_t i=0; i<IIC_PORT_NUM; i++)
    {
        IIC_DEVICE *dev = &port_iic_dev.portDev[i].dev;
        
        uint8_t res = dev->update_LowAddr(dev);

        if(res == 0) {
            port_iic_dev.portDev[i].port_dev_sta |= (1<<IIC_PORT_DEV_STA_Have);
        }
        else {
            port_iic_dev.portDev[i].port_dev_sta &= ~(1<<IIC_PORT_DEV_STA_Have);
        }
    }
}

/*************************************************************
 * @description: 获取端口的IIC状态，是否有设备接入
 * @param {uint8_t} port 1-6
 * @return {*}  1:有设备接入  0:无设备接入
 *************************************************************/
uint8_t get_IIC_Port_DevSta(uint8_t port)
{
    if(port==0 || port>IIC_PORT_NUM) return 0;

    port--;

    if(port_iic_dev.portDev[port].port_dev_sta & (1<<IIC_PORT_DEV_STA_Have)) {
        return 1;
    }
    else {
        return 0;
    }
}


/*************************************************************
 * @description: 设置端口的设备地址
 * @param {uint8_t} port 1-6
 * @param {uint8_t} addr 端口设备地址
 * @return {*}
 *************************************************************/
void set_IIC_Port_DevAddr(uint8_t port, uint8_t addr)
{
    if(port==0 || port>IIC_PORT_NUM) return;

    port--;
    
    port_iic_dev.portDev[port].dev.device_address = addr;
}

/*************************************************************
 * @description: 获取端口的设备地址
 * @param {uint8_t} port 1-6
 * @return 记录的设备地址
 *************************************************************/
uint8_t get_IIC_Port_DevAddr(uint8_t port)
{
    if(port==0 || port>IIC_PORT_NUM) return 0xff;

    port--;

    return port_iic_dev.portDev[port].dev.device_address;
}

/*************************************************************
 * @description: 获取端口的设备句柄
 * @param {uint8_t} port 1-8
 * @return {*}
 *************************************************************/
IIC_DEVICE *get_IIC_Port_DevHandle(uint8_t port)
{
    IIC_DEVICE *device = NULL;
    if(port==0 || port>IIC_PORT_NUM) return NULL;

    port--;
    device = &port_iic_dev.portDev[port].dev;


    return device;
}

/*************************************************************
 * @description: 获取端口的驱动句柄
 * @param {uint8_t} port port 1-6
 * @return {*}
 *************************************************************/
_SOFT_IIC_DRIVE *get_IIC_Port_DrvHandle(uint8_t port)
{
    if(port==0 || port>IIC_PORT_NUM) return NULL;

    port--;

    return &port_iic_dev.port_iic_drv[port];
}


/*************************************************************
 * @description: 获取端口的驱动句柄
 * @param {IIC_DEVICE} *dev 设备驱动句柄
 * @return {*}
 *************************************************************/
_SOFT_IIC_DRIVE *get_IIC_Port_DrvHandleforDev(IIC_DEVICE *dev)
{
    if(dev==NULL) return NULL;

    return dev->iic_drive;
}


void deinit_all_iic_port(void)
{
    IIC_DEVICE *device = NULL;
    for(uint8_t i=1; i<=IIC_PORT_NUM; i++)
    {
       device = get_IIC_Port_DevHandle(i);
       if(device != NULL) 
       {
           device->set_default(device);
       }
    }
}
