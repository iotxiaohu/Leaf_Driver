# leaf_flash使用说明

## 概述

相比较传统各种flash存储方案, 都是比较大的, 因此本人开发一个只针对STM32的小型存储系统

---

### `优点:`

* 真的很小
* 所占空间真的不大
* 拿开直接就可以用
* 存储灵活性高

### `缺点:`

* 目前只能用在STM32的芯片上面
* 根据需要存储的变量选择页

---

## 实现思路

特点:

* 4字节为存储的最小单元
* `Node`包含`头`、`数据`、`头`

|头|
|:-:|
|**数据**|
|**尾**|

* `头`占4字节,前两字节为数据部分的字节数, 后两字节`0x01`表示是`头`
* `尾`占4字节,前两字节为数据部分的字节数, 后两字节`0x04`表示是`尾`
* `数据`占n字节,表示实时存储的数据
* `页`由很多`Node`组成

|第xx页|
|:-:|
|Node1|
|Node2|
|Node2|
||
||
||

* 当数据存满了之后就要擦除掉重新再存
* 读取的时候从后往前读取最新的`Node`

---

## 移植

### 第1步

修改`leaf_flash.h`要用到的芯片的页大小, 和要用到的页的首地址

<div align=center>
<img width="500" src="https://github.com/leafguo/Leaf_Driver/blob/master/Leaf_Flash/image/1.png?raw=true"/>
</div>

### 第2步

如果要用测试例程, 取消这个注释

<div align=center>
<img width="500" src="https://github.com/leafguo/Leaf_Driver/blob/master/Leaf_Flash/image/2.png?raw=true"/>
</div>

### 第3步

* 使用的时候加入头文件`#include "leaf_flash.h"`
* 申明要存储变量的起始页地址和要占的页数如`L_node test1 = {PAGE122, 1};`
* 调用`Write_LeafFlash()`写
* 调用`Read_LeafFlash()`读
* 详情请参考`leaf_flash.c`的参考例程

<div align=center>
<img width="700" src="https://github.com/leafguo/Leaf_Driver/blob/master/Leaf_Flash/image/3.png?raw=true"/>
</div>

---
