//与verilator无关的一些头文件
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

//接入nvboard
#include <nvboard.h>

//使用verilater必须include
#include "Vtop.h" 	//仿真模型的头文件，由top.v生成，如果顶层文件名更改则也需要更改
#include <verilated.h>

#define CONFIG_FST_WAVE_TRACE 1		//是否生成波形

// contextp用来保存仿真的时间
VerilatedContext *contextp = new VerilatedContext;

// 构建一个名为top的仿真模型
Vtop *top = new Vtop{contextp};

//declare
void nvboard_bind_all_pins(TOP_NAME* top);

/* 用于时序电路
static void single_cycle() {
  top->clk = 0; top->eval();
  top->clk = 1; top->eval();
}
static void reset(int n) {
  top->rst = 1;
  while (n-- > 0) single_cycle();
  top->rst = 0;
}
*/

//如果生成FST格式的wave
#if CONFIG_FST_WAVE_TRACE
#include "verilated_fst_c.h"			//波形文件所需的头文件
VerilatedFstC *tfp = new VerilatedFstC; 	// 创建一个波形文件指针

void step_and_dump_wave(){
	nvboard_update();
	top->eval();
	contextp->timeInc(1);
	tfp->dump(contextp->time());
	printf("a = %d,  b= %d,  s= %d, y = %d\n", top->a, top->b, top->s, top->y);	//按需打印想要的
}

#endif

//如果生成vcd格式的wave
#if CONFIG_VCD_WAVE_TRACE
#include "verilated_cvd_c.h"			//波形文件所需的头文件
VerilatedFstC *tfp = new VerilatedVcdC; 	// 创建一个波形文件指针

void step_and_dump_wave(){
	top->eval();
	contextp->timeInc(1);
	tfp->dump(contextp->time());
}

#endif

//仿真的过程
int main(int argc, char **argv)
{
	//绑定nvboard引脚，初始化设置nvboard_bind_all_pins_temp
	nvboard_bind_all_pins(top);
  	nvboard_init();
	// 传递参数给verilator
	contextp->commandArgs(argc, argv);
	
//如果生成FST格式的wave
#if CONFIG_FST_WAVE_TRACE
	contextp->traceEverOn(true);		  // 启用跟踪
	top->trace(tfp, 99);				  // 采样深度为99
	tfp->open("build/logs/cpu_wave.fst"); // 打开波形文件，文件地址和文件名可以自定义
#endif

	/***************对top端口的初始化*******************/
//	reset(10);
	
	top->s=0; 	top->a=0; top->b=0;  step_and_dump_wave();   // 将s，a和b均初始化为“0”
				top->b=1;  step_and_dump_wave();   // 将b改为“1”，s和a的值不变，继续保持“0”，
				top->a=1; top->b=0;  step_and_dump_wave();   // 将a，b分别改为“1”和“0”，s的值不变，
				top->b=1;  step_and_dump_wave();   // 将b改为“1”，s和a的值不变，维持10个时间单位
	top->s=1; 	top->a=0; top->b=0;  step_and_dump_wave();   // 将s，a，b分别变为“1,0,0”，维持10个时间单位
				top->b=1;  step_and_dump_wave();
				top->a=1; top->b=0;  step_and_dump_wave();
				top->b=1;  step_and_dump_wave();

/*****************仿真结束，一些善后工作***************/
#if CONFIG_FST_WAVE_TRACE
	step_and_dump_wave();	//不添加此句的话会漏掉一个波形,由此可见更新电路状态是在边沿未稳定时进行的
	tfp->close(); // 关闭波形文件
#endif

	// 清理top仿真模型，并销毁相关指针，并将指针变为空指针
	nvboard_quit();
	top->final();
	delete top;
	top = nullptr;
	delete contextp;
	contextp = nullptr;

	return 0;
}
