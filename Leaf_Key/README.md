# leaf_key使用说明

## 概述

各种嵌入式系统中经常会使用到按键, 为了方便使用这儿我自己写了按键驱动, 加入了常见的短按,长安等功能.

---

## 移植

### 第1步

添加`leaf_key.h`头文件

### 第2步

仿照测试例程添加以下功能:
* 按键读取函数,以及按下的电平(必须)
* 短按回调函数(可选)
* 长按回调函数和长按触发的时间(可选)

写电平读取函数

```
/* 读取电平的函数 */
static int read_key1(void)
{
    return HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);
}
```

写短按回调函数
```
/* 测试短按 */
static void key1_ShortPress_cb(void)
{
    printf("Key1 Short Press\n");
    return;
}
```

写长按回调函数

```
/* 测试长按 */
static void key1_LongPress_cb(void)
{
    printf("Key1 Long Press\n");
    return;
}
```

定义一个按键,并且添加各种函数

```
new_key(key1);  //添加 一个按键
key_add_ReadLevel(GetKeyNode(key1), &read_key1);    //设置 "读取电平函数"
key_add_PressLevel(GetKeyNode(key1), 0);            //设置 "按下的电平"
key_add_ShortPress_cb(GetKeyNode(key1), &key1_ShortPress_cb);       //设置 "短按回调函数"
key_add_LongPress_cb(GetKeyNode(key1), &key1_LongPress_cb, 500);    //设置 "长按回调函数"
```

### 第3步

间隔10ms执行`key_transfer();`函数

```
while(1)
{
    key_transfer();
    HAL_Delay(10);
}
```

---
