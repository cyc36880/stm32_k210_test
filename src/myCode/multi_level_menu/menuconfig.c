#include "menuconfig.h"

#include "menu.h"
#include "oledfont.h" //�����ļ�
#include "menufontshow.h" //�ַ�����
#include "Graphicalfunctions.h" // ͼ�λ�����
#include "menu_tool.h" //����

#include "IIC_Port.h"
#include "k210_check.h"

typedef enum
{
	KEY_RELEASE = 0,
	KEY_CLICK,
} key_state_t;

typedef struct
{
	uint8_t last_status;
} key_state_machine_t;


static menu_area startMenu;
static menu_area mainMenu; //��һ���˵�

static key_state_machine_t key1_state_machine={0};
static key_state_machine_t key2_state_machine={0};

static void menu_start(menu_area *target);
static void menu_main(menu_area *target);

// ����״̬��
key_state_t get_key_is_click(key_state_machine_t *state_machine, uint8_t key_status)
{
	if (key_status != state_machine->last_status)
	{
		state_machine->last_status = key_status;
		if (0 == key_status)
		{
			return KEY_CLICK;
		}
	}
	return KEY_RELEASE;
}



/*
	���ܣ���ʼ���˵��б� <�˺����������޸�>
*/
static void MakeMenu(void)
{
	AddToFunctionTicker(50, normalRun, MenuRefresh, 0); //��ʱˢ��
	TargetMenuPointrt.style = ENABLE; //ָʾ����Ч
	
	TargetMenu = &startMenu;
	
	// ��������
	SetMenu(&startMenu, 0, 0, 0, 0, ENABLE, NULL)->menuinterface = menu_start;
	AddToMenuListInit(&startMenu);
	AddToSpecialFunction(&startMenu, MenuTime, 50);

	SetMenu(&mainMenu, 0, 0, 0, 0, ENABLE, NULL)->menuinterface = menu_main;
	AddToMenuListInit(&mainMenu);

    LED_GREEN(0);
    LED_RED(0);
}

/*
	���ܣ��˵���ʾ�ľ���ʵ�ֺ���
	ע�⣺���������� void ���ͣ��βα����ҽ�Ϊ menu_area *target
*/

static void menu_start(menu_area *target)
{
	static const char *str = "K210 TEST";
	static uint8_t count=0;
    if (CkeckMenuList(target))
	{
		FontInfoType *font = StringDeal(str);
		uint8_t str_len_size = 
		target->x = MSCX(font->maxPix);
		target->y = MSCY(font->ascsize[1]);
		target->width = font->maxPix;
		target->high = font->ascsize[1];
		return;
	}
	if (TriggerCheck(target, MenuTime))
	{
		count++;
	}
	if (count == 30)
	{
		count=0;
		gotoMenu(&mainMenu, Menu_Sub);
	}

	m_printf(target, 0, 0, "%s", str);
}


static void menu_main(menu_area *target)
{
    /**
     * 0: ����
     * 1�����߼��
     * 2������У��
     * 3��sd ״̬
     * 4��wifi ״̬
     * 5: ���״̬չʾ
     */
    static uint8_t task_step = 0;  //������
    static uint8_t error_code = 0; //״̬����
    static uint8_t popup = 0;
    static uint8_t popup_count = 0;
    static uint8_t popup_flag = 0;

    key_state_t  key1_status = KEY_RELEASE;
    key_state_t  key2_status = KEY_RELEASE;

	static const char *title_str = "k210 TEST";
	static uint8_t title_str_size = 0;
	if (CkeckMenuList(target))
	{
		FontInfoType *font = StringDeal(title_str);
		title_str_size = font->maxPix;
		target->x = 0;
		target->y =0;
		target->width = SCREENWIDTH;
		target->high = font->ascsize[1];
		return;
	}
	m_printf(NULL, MSCX(title_str_size), TargetMenuPointrt.y, "%s", title_str);

    key1_status = get_key_is_click(&key1_state_machine, key_1_status),
    key2_status = get_key_is_click(&key2_state_machine, key_2_status);


    if (key2_status == KEY_CLICK || key1_status == KEY_CLICK)
    {
        LED_GREEN(0);
        LED_RED(0);
        task_step = 0;  //������
        error_code = 0; //״̬����
        popup = 0;
        popup_count = 0;
        popup_flag = 0;
        HAL_Delay(500);
    }

    if (key1_status == KEY_CLICK)
    {
        if (0 == task_step)
        {
            task_step = 1;
        }
        else
        {
            task_step = 0;
            error_code = 0;
        }
    }
    static check_error_t check_status = CHECK_OK;
    check_status = CHECK_OK;
    static uint16_t retry = 0;
    switch (task_step)
    {
        case 0:
            m_printf(NULL, 0, 30, "STA: idle");
            break;
        case 1:
            check_status = wait_k210_online(20);
            m_printf(NULL, 0, 30, "Online: %s", CHECK_OK==check_status ? "online" : "offline...");  
            break;
        case 2:
            check_status = check_k210_verify();
            m_printf(NULL, 0, 30, "VER: %s,%d", CHECK_OK==check_status ? "ok" : "error", retry);
            break;
        case 3:
            check_status = get_k210_sd_status();
            m_printf(NULL, 0, 30, "SD: %s %d", CHECK_OK==check_status ? "ok" : "error", retry);
            break;
        case 4:
            check_status = get_k210_wifi_status();
            m_printf(NULL, 0, 30, "Wifi: %s %d", CHECK_OK==check_status ? "ok" : "error", retry);
            break;
        case 5:
            if (0 == error_code)
                m_printf(NULL, 0, 30, "RES: OK!!!"), LED_GREEN(50);
            else
                m_printf(NULL, 0, 30, "RES: error"), LED_RED(50);
            break;
        default:
            break;
    }

    if (0 != task_step)
    {
        if (1 == task_step)
        {
            if ( CHECK_OK == check_status ) task_step++;
        }
        else if ( CHECK_OK == check_status )
        {
            if (task_step < 5) task_step++;
        }
        else
        {
            if (retry < 20)
            {
                retry++;
            }
            else
            {
                error_code |= (1<<(task_step));
                retry = 0;
                task_step++;
            }
        }
    }

    m_printf(NULL, 0, 50, "%d %d %d %d", (error_code>>1)&1, (error_code>>2)&1, (error_code>>3)&1, (error_code>>4)&1);




    //

    if (KEY_CLICK == key2_status)
    {
        popup_flag = 0;
        if (0 == popup)
        {
            popup = 1;
            popup_count = 0;
        }
        else
        {
            popup = 0;
            popup_count = 0;
        }
    }

    if (popup)
    {
        uint8_t x = 60;
        uint8_t y = 45;
        DrawLine(x, y+16, x+60, y+16);
        if (1==popup )
        {
            if (popup_count < 20)
                popup_count++;
            else
                popup = 2;
            popup_flag = 1;
            if (CHECK_OK == set_k210_factory_text())
            {
                popup_flag = 0;
                popup = 2;
            }
        }
        if (1 == popup)
        {
            m_printf(NULL, x, y, "count %d", popup_count);
        }
        else if (2 == popup)
        {
            if (0 == popup_flag)
                m_printf(NULL, x, y, "ok"), LED_GREEN(50);
            else
                m_printf(NULL, x, y, "error"), LED_RED(50);
        }
    }



    // m_printf(NULL, 0, 50, "%d", check_status);
}








// ==================================================================
// ======================== �� �� �� �� �� �� =======================
// ==================================================================



//**************** while��ȫ������  ************************


void AlwaysRun(void)
{
	
}



/* ************** �˵�����ˢ�� ******************** */


/**** �˵���ÿ��ˢ�¶���ִ�иú��� ִ�����ȼ���� ��������Ҫд��ʾ��غ������ᱻ�쳣��գ����� ****/
void MenuAlwaysRun_PH(void)
{

}


/**** �˵���ÿ��ˢ�¶���ִ�иú��� ִ�����ȼ���****/
void MenuAlwaysRun_PM(void)
{

}



/**** �˵���ÿ��ˢ�¶���ִ�иú��� ִ�����ȼ��� ****/
void MenuAlwaysRun_PL(void)
{

}




//************* ��ʼ���˵�������� *********************

void MenuInit(void)
{
	MenuSysBaseInit(); //ϵͳ��ʼ��
	MakeMenu(); //�˵��б��ʼ��
	sMenuListInit();
	/* ********** USER BEGIN********** */
	
	
	/* ********** USER END ********** */
	
	MenuHeartTimeStart = ENABLE;//�˵�����
	
}








