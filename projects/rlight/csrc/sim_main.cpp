//与verilator无关的一些头文件
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

//接入nvboard
#include <nvboard.h>

//使用verilater必须include
#include "Vlight.h" //仿真模型的头文件，由top.v生成，如果顶层文件名更改则也需要更改
#include <verilated.h>

#define CONFIG_FST_WAVE_TRACE 0	//不生成波形

// contextp用来保存仿真的时间
VerilatedContext *contextp = new VerilatedContext;

// 构建一个名为top的仿真模型
Vlight *top = new Vlight{contextp};

//declare
void nvboard_bind_all_pins(TOP_NAME* top);


static void single_cycle() {
  top->clk = 0; top->eval();
  top->clk = 1; top->eval();
}

static void reset(int n) {
  top->rst = 1;
  while (n-- > 0) single_cycle();
  top->rst = 0;
}

//如果生成FST格式的wave
#if CONFIG_FST_WAVE_TRACE
#include "verilated_fst_c.h"			//波形文件所需的头文件
VerilatedFstC *tfp = new VerilatedFstC; 	// 创建一个波形文件指针
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
	reset(10);
	
//	int cycle = 50;	//定义仿真循环次数，即仿真50个循环后就自动结束
	/**************verilator的仿真循环*****************/
	while (1)	// 仿真50个循环后就自动结束
	{
		nvboard_update();		//
		single_cycle();
//		printf("led = %d\n", top->led);	//按需打印想要的
    
		contextp->timeInc(1); //推动仿真时间
		
#if CONFIG_FST_WAVE_TRACE
		tfp->dump(contextp->time()); // 按照时间采样
#endif
//	cycle--;
	
	}

/*****************仿真结束，一些善后工作***************/
#if CONFIG_FST_WAVE_TRACE
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
