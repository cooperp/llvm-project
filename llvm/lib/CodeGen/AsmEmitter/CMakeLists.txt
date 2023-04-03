add_llvm_component_library(LLVMAsmEmitter
  AsmEmitter.cpp
  AsmEmitterDwarf.cpp

  LINK_COMPONENTS
  BinaryFormat
  DebugInfoCodeView
  DebugInfoDWARF
  DebugInfoMSF
  MC
  MCParser
  Remarks
  Support
  Target
  TargetParser
  )
