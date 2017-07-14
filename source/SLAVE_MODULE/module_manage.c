/**
  ******************************************************************************
  * @file    module_manage.c
  * @author  王鑫
  * @version V1.0.0
  * @date    2017.7.10
  * @brief   模块管理
  ******************************************************************************
  */
#define MODULE_GLOBALS
#include "module_manage.h"
#include "string.h"
#include "crc.h"
#include "USART3.H"
#include "Timer5_Config.h"




MODULE_INF module=
{
    "8833_BOARD",///< 模块名称
    "V1.0.0",///< 模块版本
    1///< 模块ID
};

uint8_t get_com_offset_add(COM_NUM com_num)
{
    uint8_t addr_offset[4] = {0, 16, 32, 48};//地址偏移，第一路0，第二路16，第三路32，第四路48
    
    return addr_offset[com_num];
}

void update_module_addr_flag(void)
{
    int32_t i = 0;
    int32_t j = 0;
    uint8_t addr = 0;
    uint8_t offset_addr = 0;
    COM_STRUCT* com_inf = NULL;
    uint8_t id = 0;
    
    for(i = 0; i < 64; i++)
    {
        if(i == 0 || i == 16 || i == 32 || i == 48 || i > 64)
        {
            continue;
        }
        
        id = road_inf_pool[i].module_inf.id;//id范围是1-15
        
        if(id != 0 && id < 16)
        {
            offset_addr = get_com_offset_add(road_inf_pool[i].com_num);
            addr = offset_addr + id;
            roads_flag.road_buf[j] = addr;
            syn_test_port[j].com = com_inf;
            j++;
        }
    }
    
    roads_flag.count = j;
}
/**
  * @brief  获取模块信息
  * @param  [in] data 数据
  * @param  [out] ack_data 应答数据
  * @param  [out] len 应答数据的长度
  * @retval 无
  */
void get_module_inf(uint8_t *data, uint8_t *ack_data, uint32_t *len)
{
    memcpy(ack_data, &module, sizeof(module));
    *len = sizeof(module);
}

enum{
    GET_MODULE_INF = 1,
};

/**
  * @brief  向发送数据添加CRC校验
  * @param  [in] data 数据
  * @param  [in] len 数据帧长度
  * @retval 无
  */
void add_crc_to_send_data(uint8_t *data, uint32_t len)
{
    uint16_t crc_val;
    uint16_t *p_u16;
    
    crc_val = get_crc16(data, len);
    p_u16 = (uint16_t *)&data[len];
    *p_u16 = crc_val;
}
/**
  * @brief  串口1的接收处理函数
  * @param  [in] com_num 串口号
  * @param  [in] frame 数据帧
  * @param  [in] len 数据帧长度
  * @retval 无
  */
void com_receive_dispose(COM_STRUCT *com, uint8_t *data, uint32_t len)
{
    uint16_t *p_crc;
    uint16_t crc_val;
    FRAME_T *frame = (void*)data;
    FRAME_T *ack_frame = NULL;
    uint32_t frame_len = 0;
    
    /* CRC校验 */
    p_crc = (uint16_t *)&data[len - 2];
    crc_val = get_crc16(data, len - 2);
    
    /* 校验失败放弃解析 */
    if(*p_crc != crc_val || crc_val == 0)
    {
        return;
    }
    
    /* 检查通信地址 1.为本地地址相同 2.主播地址0 否则就不是本机的数据包 */
    if(frame->addr != module.id && frame->addr != 0)
    {
        return;
    }
    
    /* 准备应答帧数据 */
    ack_frame = (void*)com->frame_buf;
    ack_frame->addr = frame->addr;
    ack_frame->fun_code = frame->fun_code;
    
    /* 指令解析 */
    switch(frame->fun_code)
    {
        case GET_MODULE_INF:
            get_module_inf(frame->data, ack_frame->data, &frame_len);
            break;
        default:
            
            break;
    }
    
    /* 发送应答 */
    if(frame->addr != 0)
    {
        frame_len += FRAME_HEAD_SIZE;
        add_crc_to_send_data((uint8_t*)ack_frame, frame_len);
        frame_len += 2;
        com->send_fun(com, (uint8_t*)ack_frame, frame_len);
        com->status = MODULE_COMM_SEND;//状态机走入发送状态
    }
}

/**
  * @brief  初始化模块管理的环境
  * @param  无
  * @retval 无
  */
void init_module_manage_env(void)
{
    register_tim5_server_fun(usart3_judge_timout);//注册串口接收完成超时定时器
}
/**
  * @brief  初始化模块管理的环境
  * @param  无
  * @retval 无
  */
void module_comm_task(void)
{
    com_comm_status_machine(&com1);
}

/************************ (C) COPYRIGHT 2017 长盛仪器 *****END OF FILE****/