################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
libMU/startup_MU.obj: ../libMU/startup_MU.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/TI/ccsv4/tools/compiler/tms470/bin/cl470" -mv7M3 -g -O2 --gcc --define=ccs --define=PART_LM3S8962 --include_path="C:/TI/ccsv4/tools/compiler/tms470/include" --include_path="C:/Users/Spook/Documents/workspace/UPnP_SSDP" --include_path="C:/Users/Spook/Documents/workspace/UPnP_SSDP/lib/uip" --include_path="C:/Users/Spook/Documents/workspace/UPnP_SSDP/lib" --include_path="C:/Users/Spook/Documents/workspace/UPnP_SSDP/lib/inc" --include_path="C:/Users/Spook/Documents/workspace/UPnP_SSDP/lib/driverlib" --include_path="C:/Users/Spook/Documents/workspace/UPnP_SSDP/lib/FreeRTOS/include" --diag_warning=225 -me --gen_func_subsections=on --abi=eabi --code_state=16 --ual --preproc_with_compile --preproc_dependency="libMU/startup_MU.pp" --obj_directory="libMU" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

libMU/startup_MU_Handlers.obj: ../libMU/startup_MU_Handlers.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/TI/ccsv4/tools/compiler/tms470/bin/cl470" -mv7M3 -g -O2 --gcc --define=ccs --define=PART_LM3S8962 --include_path="C:/TI/ccsv4/tools/compiler/tms470/include" --include_path="C:/Users/Spook/Documents/workspace/UPnP_SSDP" --include_path="C:/Users/Spook/Documents/workspace/UPnP_SSDP/lib/uip" --include_path="C:/Users/Spook/Documents/workspace/UPnP_SSDP/lib" --include_path="C:/Users/Spook/Documents/workspace/UPnP_SSDP/lib/inc" --include_path="C:/Users/Spook/Documents/workspace/UPnP_SSDP/lib/driverlib" --include_path="C:/Users/Spook/Documents/workspace/UPnP_SSDP/lib/FreeRTOS/include" --diag_warning=225 -me --gen_func_subsections=on --abi=eabi --code_state=16 --ual --preproc_with_compile --preproc_dependency="libMU/startup_MU_Handlers.pp" --obj_directory="libMU" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '


