################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
lib/FreeRTOS/portable/CCS4/ARM_CM3/port.obj: ../lib/FreeRTOS/portable/CCS4/ARM_CM3/port.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/TI/ccsv4/tools/compiler/tms470/bin/cl470" -mv7M3 -g -O2 --gcc --define=ccs --define=PART_LM3S8962 --include_path="C:/TI/ccsv4/tools/compiler/tms470/include" --include_path="C:/Users/Spook/Documents/workspace/UPnP_SSDP" --include_path="C:/Users/Spook/Documents/workspace/UPnP_SSDP/lib/uip" --include_path="C:/Users/Spook/Documents/workspace/UPnP_SSDP/lib" --include_path="C:/Users/Spook/Documents/workspace/UPnP_SSDP/lib/inc" --include_path="C:/Users/Spook/Documents/workspace/UPnP_SSDP/lib/driverlib" --include_path="C:/Users/Spook/Documents/workspace/UPnP_SSDP/lib/FreeRTOS/include" --diag_warning=225 -me --gen_func_subsections=on --abi=eabi --code_state=16 --ual --preproc_with_compile --preproc_dependency="lib/FreeRTOS/portable/CCS4/ARM_CM3/port.pp" --obj_directory="lib/FreeRTOS/portable/CCS4/ARM_CM3" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '


