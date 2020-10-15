# Define these parameters to reflect the proper directories.
TOP=/home/shubham/merging-nballerina/nballerina
LLVM_INCLUDES_DIR=$(TOP)/llvm-project-master/llvm/include
LLVM_BUILD_INCLUDES_DIR=$(TOP)/build/llvm/include
LLVM_BUILD_LIB_DIR=$(TOP)/build/llvm/lib

# No user definable parameters below.
PARALLEL_MAKE=make -j 20
CXX=g++
CFLAGS=-g -Wall -I. -I$(LLVM_INCLUDES_DIR) -I$(LLVM_BUILD_INCLUDES_DIR)
LDFLAGS= -L$(LLVM_BUILD_LIB_DIR) $(LIBS) 
LIBS= -lLLVMXRay -lLLVMWindowsManifest -lLLVMTestingSupport -lLLVMTableGen \
      -lLLVMSymbolize -lLLVMDebugInfoPDB -lLLVMOrcJIT -lLLVMOrcError -lLLVMJITLink \
      -lLLVMObjectYAML -lLLVMMCA -lLLVMLTO -lLLVMPasses -lLLVMCoroutines \
      -lLLVMObjCARCOpts -lLLVMExtensions -lLLVMLineEditor -lLLVMLibDriver \
      -lLLVMInterpreter -lgtest_main -lgtest -lLLVMFuzzMutate -lLLVMMCJIT \
      -lLLVMExecutionEngine -lLLVMRuntimeDyld -lLLVMDWARFLinker -lLLVMDlltoolDriver \
      -lLLVMOption -lLLVMDebugInfoGSYM -lLLVMCoverage -lLLVMXCoreDisassembler \
      -lLLVMXCoreCodeGen -lLLVMXCoreDesc -lLLVMXCoreInfo -lLLVMX86Disassembler \
      -lLLVMX86AsmParser -lLLVMX86CodeGen -lLLVMX86Desc -lLLVMX86Info \
      -lLLVMWebAssemblyDisassembler -lLLVMWebAssemblyCodeGen -lLLVMWebAssemblyDesc \
      -lLLVMWebAssemblyAsmParser -lLLVMWebAssemblyInfo -lLLVMSystemZDisassembler \
      -lLLVMSystemZCodeGen -lLLVMSystemZAsmParser -lLLVMSystemZDesc \
      -lLLVMSystemZInfo -lLLVMSparcDisassembler -lLLVMSparcCodeGen \
      -lLLVMSparcAsmParser -lLLVMSparcDesc -lLLVMSparcInfo -lLLVMRISCVDisassembler \
      -lLLVMRISCVCodeGen -lLLVMRISCVAsmParser -lLLVMRISCVDesc -lLLVMRISCVUtils \
      -lLLVMRISCVInfo -lLLVMPowerPCDisassembler -lLLVMPowerPCCodeGen \
      -lLLVMPowerPCAsmParser -lLLVMPowerPCDesc -lLLVMPowerPCInfo -lLLVMNVPTXCodeGen \
      -lLLVMNVPTXDesc -lLLVMNVPTXInfo -lLLVMMSP430Disassembler -lLLVMMSP430CodeGen \
      -lLLVMMSP430AsmParser -lLLVMMSP430Desc -lLLVMMSP430Info -lLLVMMipsDisassembler \
      -lLLVMMipsCodeGen -lLLVMMipsAsmParser -lLLVMMipsDesc -lLLVMMipsInfo \
      -lLLVMLanaiDisassembler -lLLVMLanaiCodeGen -lLLVMLanaiAsmParser \
      -lLLVMLanaiDesc -lLLVMLanaiInfo -lLLVMHexagonDisassembler -lLLVMHexagonCodeGen \
      -lLLVMHexagonAsmParser -lLLVMHexagonDesc -lLLVMHexagonInfo \
      -lLLVMBPFDisassembler -lLLVMBPFCodeGen -lLLVMBPFAsmParser -lLLVMBPFDesc \
      -lLLVMBPFInfo -lLLVMAVRDisassembler -lLLVMAVRCodeGen -lLLVMAVRAsmParser \
      -lLLVMAVRDesc -lLLVMAVRInfo -lLLVMARMDisassembler -lLLVMARMCodeGen \
      -lLLVMARMAsmParser -lLLVMARMDesc -lLLVMARMUtils -lLLVMARMInfo \
      -lLLVMAMDGPUDisassembler -lLLVMAMDGPUCodeGen -lLLVMMIRParser -lLLVMipo \
      -lLLVMInstrumentation -lLLVMVectorize -lLLVMLinker -lLLVMIRReader \
      -lLLVMAsmParser -lLLVMFrontendOpenMP -lLLVMAMDGPUAsmParser -lLLVMAMDGPUDesc \
      -lLLVMAMDGPUUtils -lLLVMAMDGPUInfo -lLLVMAArch64Disassembler \
      -lLLVMMCDisassembler -lLLVMAArch64CodeGen -lLLVMCFGuard -lLLVMGlobalISel \
      -lLLVMSelectionDAG -lLLVMAsmPrinter -lLLVMDebugInfoDWARF -lLLVMCodeGen \
      -lLLVMTarget -lLLVMScalarOpts -lLLVMInstCombine -lLLVMAggressiveInstCombine \
      -lLLVMTransformUtils -lLLVMBitWriter -lLLVMAnalysis -lLLVMProfileData \
      -lLLVMObject -lLLVMTextAPI -lLLVMBitReader -lLLVMCore -lLLVMRemarks \
      -lLLVMBitstreamReader -lLLVMAArch64AsmParser -lLLVMMCParser -lLLVMAArch64Desc \
      -lLLVMMC -lLLVMDebugInfoCodeView -lLLVMDebugInfoMSF -lLLVMBinaryFormat \
      -lLLVMAArch64Utils -lLLVMAArch64Info -lLLVMSupport -lLLVMDemangle -lpthread \
      -ltinfo

SRCS = llvmPackageGen.cpp \
       llvmBIRNodeGen.cpp \
       llvmBIRLocation.cpp \
       llvmFunctionGen.cpp \
       llvmBasicBlockGen.cpp \
       llvmVarDeclGen.cpp \
       llvmOperandGen.cpp \
       llvmInvokableTypeGen.cpp \
       llvmFuncParamGen.cpp \
       llvmParamGen.cpp \
       llvmTypeDeclGen.cpp \
       llvmNonTerminatorInsnGen.cpp \
       llvmAbstractInsnGen.cpp \
       llvmTerminatorInsnGen.cpp \
       llvmMoveInsn.cpp \
       llvmConstantLoadInsn.cpp \
       llvmBinaryOpInsn.cpp \
       llvmReturnInsn.cpp \
       llvmGoToInsn.cpp \
       llvmUnaryOpInsn.cpp \
       llvmConditionBrInsn.cpp \
       BIRReader.cpp \
       driver.cpp

HEADERS = BIR.h BIRReader.h

OBJS = $(patsubst %.cpp,%.o,$(SRCS))

EXE = nballerinacc

.cpp.o: $(HEADERS)
	$(CXX) -c $(CFLAGS) -o $@ $<

$(EXE): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $@

all: $(EXE)

clean:
	rm -f $(OBJS) $(EXE)
