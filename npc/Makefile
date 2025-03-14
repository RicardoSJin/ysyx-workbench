#顶层文件的名字
TOPNAME = top

#指定verilator
VERILATOR = verilator

######################## 构建后的项目 ########################
#构建后的地址
BUILD_DIR = ./build/verilator

#构建后文件存放的地址
OBJ_DIR = $(BUILD_DIR)/obj_dir

#可执行文件的地址
BIN = $(OBJ_DIR)/$(TOPNAME)

######################## verilator选项 ########################
#创建生成文件的目录,存放生成的文件
VERILATOR_FLAGS += --Mdir $(OBJ_DIR)

#优化分配 
VERILATOR_FLAGS += -x-assign fast

#将所有warning 变成error
VERILATOR_FLAGS += -Wall

#Make waveforms,fst格式
VERILATOR_FLAGS += --trace-fst

#检查SystemVerilog断言
VERILATOR_FLAGS += --assert

#debug模式，生成更多调试信息
VERILATOR_FLAGS += --debug

#Add this trace to get a backtrace in gdb
VERILATOR_FLAGS += --gdbbt

# 可用gdb调试
VERILATOR_FLAGS += --gdb

#加速编译
VERILATOR_FLAGS += -j

#调用工具链来构建库，不加则由verilator自己构建
VERILATOR_FLAGS += --build

#指定顶层文件
VERILATOR_FLAGS += --top-module $(TOPNAME)

#生成C++可执行文件，并指定文件名字
VERILATOR_FLAGS += --cc --exe -o $(abspath $(BIN))

######################## 查找相关文件 ########################
#所有 .v 或者 .sv 文件
VSRCS = $(shell find $(abspath ./build/chisel) -name "*.v" -or -name "*.sv")

#所有 .c 或 .cc 或 .cpp 文件
CSRCS = $(shell find $(abspath ./src/main/cpp) -name "*.c" -or -name "*.cc" -or -name "*.cpp")

all:
	@echo "Write this Makefile by your self."

######################## 目标与命令 ########################
sim:
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	@echo "-------- VERILATE --- AND --- BUILD --------"
#清空再创建编译后文件放置的目录
	@rm -rf $(OBJ_DIR)
	@mkdir -p $(OBJ_DIR)

#使用verilator进行编译，产生可执行文件
	@$(VERILATOR) $(VERILATOR_FLAGS) $(VSRCS) $(CSRCS)

	@echo
	@echo "-------------------- RUN --------------------"

#清空再创建波形文件夹
	@rm -rf $(BUILD_DIR)/logs
	@mkdir -p $(BUILD_DIR)/logs

#执行可执行文件，运行并仿真，生成波形
	@$(BIN) +trace

	@echo
	@echo "-------------------- DONE --------------------"
	@echo "To see waveforms, open $(BUILD_DIR)/logs/cpu_wave.fst in a waveform viewer"
	@echo
	
gdb:
	@echo "-------- VERILATE --- AND --- BUILD --------"

#清空再创建编译后文件放置的目录
	@rm -rf $(OBJ_DIR)
	@mkdir -p $(OBJ_DIR)

#使用verilator进行编译，产生可执行文件
	@$(VERILATOR) $(VERILATOR_FLAGS) $(VSRCS) $(CSRCS)

	@echo
	@echo "-------------------- GDB --------------------"

#清空再创建波形文件夹
	@rm -rf $(BUILD_DIR)/logs
	@mkdir -p $(BUILD_DIR)/logs

#gbd执行可执行文件，运行并仿真，生成波形
	gdb -s $(BIN) --args $(BIN)

	@echo
	@echo "-------------------- DONE --------------------"
	@echo "To see waveforms, open $(BUILD_DIR)/logs/cpu_wave.fst in a waveform viewer"
	@echo

include ../Makefile


