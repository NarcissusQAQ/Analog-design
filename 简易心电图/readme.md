本系统是以STM32F1为核心，系统由主要由放大电路，滤波电路构成。输入人体心电信号，先经过放大电路将心电信号放大，然后接上低通滤波器滤除低频得杂波使得信号清晰，后接上高频滤波电路，滤除高频杂波，最重要得是最后得50HZ带通滤波器滤除了生物电信号得干扰，最后通过STM32的显示在屏
幕上显示自己的心电图。
