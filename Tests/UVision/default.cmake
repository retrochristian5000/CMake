# Use ARMCM0 as default device
set(CMAKE_UV_DEVICE ARMCM0)
# ARMCM0 is a cortex-m0
set(CMAKE_SYSTEM_PROCESSOR cortex-m0)
# Use ArmClang as default generator toolset
set(CMAKE_GENERATOR_TOOLSET ARMClang)
# Use ArmCC as asm compiler (Packs usually contain ArmCC assembly code)
set(CMAKE_ASM_COMPILER_ID ARMCC)