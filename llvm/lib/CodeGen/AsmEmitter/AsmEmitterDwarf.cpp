//===-- AsmEmitterDwarf.cpp - AsmEmitter Dwarf Support --------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the Dwarf emissions parts of AsmEmitter.
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/Twine.h"
#include "llvm/BinaryFormat/Dwarf.h"
#include "llvm/CodeGen/AsmEmitter.h"
#include "llvm/CodeGen/DIE.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCDwarf.h"
#include "llvm/MC/MCSection.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Target/TargetLoweringObjectFile.h"
#include <cstdint>
using namespace llvm;

#define DEBUG_TYPE "asm-printer"

//===----------------------------------------------------------------------===//
// Dwarf Lowering Routines
//===----------------------------------------------------------------------===//

void AsmEmitter::emitCFIInstruction(const MCCFIInstruction &Inst) const {
  switch (Inst.getOperation()) {
  default:
    llvm_unreachable("Unexpected instruction");
  case MCCFIInstruction::OpDefCfaOffset:
    OutStreamer->emitCFIDefCfaOffset(Inst.getOffset());
    break;
  case MCCFIInstruction::OpAdjustCfaOffset:
    OutStreamer->emitCFIAdjustCfaOffset(Inst.getOffset());
    break;
  case MCCFIInstruction::OpDefCfa:
    OutStreamer->emitCFIDefCfa(Inst.getRegister(), Inst.getOffset());
    break;
  case MCCFIInstruction::OpDefCfaRegister:
    OutStreamer->emitCFIDefCfaRegister(Inst.getRegister());
    break;
  case MCCFIInstruction::OpLLVMDefAspaceCfa:
    OutStreamer->emitCFILLVMDefAspaceCfa(Inst.getRegister(), Inst.getOffset(),
                                         Inst.getAddressSpace());
    break;
  case MCCFIInstruction::OpOffset:
    OutStreamer->emitCFIOffset(Inst.getRegister(), Inst.getOffset());
    break;
  case MCCFIInstruction::OpRegister:
    OutStreamer->emitCFIRegister(Inst.getRegister(), Inst.getRegister2());
    break;
  case MCCFIInstruction::OpWindowSave:
    OutStreamer->emitCFIWindowSave();
    break;
  case MCCFIInstruction::OpNegateRAState:
    OutStreamer->emitCFINegateRAState();
    break;
  case MCCFIInstruction::OpSameValue:
    OutStreamer->emitCFISameValue(Inst.getRegister());
    break;
  case MCCFIInstruction::OpGnuArgsSize:
    OutStreamer->emitCFIGnuArgsSize(Inst.getOffset());
    break;
  case MCCFIInstruction::OpEscape:
    OutStreamer->AddComment(Inst.getComment());
    OutStreamer->emitCFIEscape(Inst.getValues());
    break;
  case MCCFIInstruction::OpRestore:
    OutStreamer->emitCFIRestore(Inst.getRegister());
    break;
  case MCCFIInstruction::OpUndefined:
    OutStreamer->emitCFIUndefined(Inst.getRegister());
    break;
  case MCCFIInstruction::OpRememberState:
    OutStreamer->emitCFIRememberState();
    break;
  case MCCFIInstruction::OpRestoreState:
    OutStreamer->emitCFIRestoreState();
    break;
  }
}
