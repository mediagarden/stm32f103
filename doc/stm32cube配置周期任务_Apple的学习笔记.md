# [stm32cube 配置周期任务 --Apple 的学习笔记](https://www.jianshu.com/p/b4710e042c50)

# 一，前言

我做同样的事喜欢用不同的方法，之前刚刚写完一个国产芯片的最小工程代码，套路都是一样的。所以今天我玩自己的 stm32F407 开发板就不想再手写代码了，期望用工具生成低代码的方式来实现最小工程 led 闪烁。

# 二，stm32cube 入门

之前就用过，但是有写忘记了。晚上稍微看了下又想起来了。至于系统时钟，外设时钟配置为多少，timer 要如何分频的基本上单片机都是类似的，找到对应章节看下就知道了了。我的配置如下 time 外设使用的时钟是 42M APB1 时钟

![img](http://upload-images.jianshu.io/upload_images/12010880-86d6b9fc8bc44565.png)



Timer6 的配置如下 1ms 中断。timer7 同样思路配置为 5ms 中断。

![img](http://upload-images.jianshu.io/upload_images/12010880-a345efec8bb12c83.png)



打开中断，中断优先级在 nvic 中配置，我做的是可抢占的 2 个周期任务

![img](http://upload-images.jianshu.io/upload_images/12010880-56ceb8677a1a0819.png)



# 三，遇到的问题总结

1. 要手工将启动文件的路径添加到工程中，否则编译报错 "error: L6236E: No section matches selector - no section to be FIRST/LAST."
2. 手写代码要放入 USER CODE BEGIN 的注释内，否则重新生成代码会被覆盖。
3. 一开始无法进入中断，看了代码发现没有使能 time 中断初始化，后来看到有 api 及网上搜索下了，原来是要手工添加的。我手动添加的 code 如下

```
//放在初始化while前面
    HAL_TIM_Base_Start_IT(&htim6);
    HAL_TIM_Base_Start_IT(&htim7);
```

```
//放在main函数上面
unsigned short cnt1ms=0u;
unsigned short cnt5ms=0u;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == htim6.Instance)
    {
            if(cnt1ms++>=500)  /* 500ms flash */
            {
                cnt1ms=0;
                HAL_GPIO_TogglePin(GPIOF, LED0_Pin);
            }
    }
        else if (htim->Instance == htim7.Instance)
        {
                if(cnt5ms++>=100)  /* 500ms flash */
                {
                    cnt5ms=0;
                    HAL_GPIO_TogglePin(GPIOF, LED1_Pin);
                }
        }
        else
        {
        }   
}
```

# 四，小结

为什么我开始用低代码工具了呢！因为我不想重复劳动了。做个最小工程就像看小家电说明书一样去看 spec，每个厂家寄存器不同，但是设计思路是雷同的，我说的重复劳动就是这个雷同的内容我比较熟悉，所以不想重复劳动。既然是业余时间玩，就玩个不同的，所以今晚就决定用工具了。真的是分分钟搞定，我连寄存器都不用看了，仅快速看了 timer 和 rcc 章节的原理就完成了配置，哈哈~