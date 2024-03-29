cmake_minimum_required(VERSION 3.18)

project(pcc_project VERSION 0.5.0 LANGUAGES C CXX)

enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(ir_core_src ir_core/Value.cpp ir_core/Instruction.cpp
        ir_core/BasicBlock.cpp ir_core/Constant.cpp
        ir_core/GlobalObject.cpp ir_core/Function.cpp 
        ir_core/GlobalVariable.cpp ir_core/Module.cpp
        ir_core/IRPrinter.cpp ir_core/IRContext.cpp)
        
set(pass_src passes/mem2reg.cpp passes/gvn.cpp
        passes/dce.cpp)

set(code_src ${ir_core_src} ${pass_src}
        codegen.cpp main.cpp 
        parse.cpp  tokenize.cpp 
        type.cpp gen_ir.cpp)


add_executable(pcc ${code_src})
target_include_directories(pcc PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(pcc PRIVATE -g -fno-common -Wno-write-strings -Wno-return-type)


add_test(NAME test COMMAND python run_test.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test)
