//===- AsmEmitter.cpp - Common AsmEmitter code ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the AsmEmitter class.
//
//===----------------------------------------------------------------------===//

#include "llvm/CodeGen/AsmEmitter.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/Target/TargetMachine.h"

using namespace llvm;

#define DEBUG_TYPE "asm-emitter"

AsmEmitter::AsmEmitter(TargetMachine &TM, std::unique_ptr<MCStreamer> Streamer)
	: MAI(TM.getMCAsmInfo()),
	  OutContext(Streamer->getContext()), OutStreamer(std::move(Streamer)) {
  DwarfUsesRelocationsAcrossSections =
      MAI->doesDwarfUseRelocationsAcrossSections();
	VerboseAsm = OutStreamer->isVerboseAsm();
}

//===----------------------------------------------------------------------===//
// Symbol Lowering Routines.
//===----------------------------------------------------------------------===//

MCSymbol *AsmEmitter::createTempSymbol(const Twine &Name) const {
  return OutContext.createTempSymbol(Name, true);
}

//===--------------------------------------------------------------------===//
// Emission and print routines
//

/// Emit a byte directive and value.
///
void AsmEmitter::emitInt8(int Value) const { OutStreamer->emitInt8(Value); }

/// Emit a short directive and value.
void AsmEmitter::emitInt16(int Value) const { OutStreamer->emitInt16(Value); }

/// Emit a long directive and value.
void AsmEmitter::emitInt32(int Value) const { OutStreamer->emitInt32(Value); }

/// EmitSLEB128 - emit the specified signed leb128 value.
void AsmEmitter::emitSLEB128(int64_t Value, const char *Desc) const {
  if (isVerbose() && Desc)
    OutStreamer->AddComment(Desc);

  OutStreamer->emitSLEB128IntValue(Value);
}

void AsmEmitter::emitULEB128(uint64_t Value, const char *Desc,
                             unsigned PadTo) const {
  if (isVerbose() && Desc)
    OutStreamer->AddComment(Desc);

  OutStreamer->emitULEB128IntValue(Value, PadTo);
}

/// Emit a long long directive and value.
void AsmEmitter::emitInt64(uint64_t Value) const {
  OutStreamer->emitInt64(Value);
}

/// Emit something like ".long Hi-Lo" where the size in bytes of the directive
/// is specified by Size and Hi/Lo specify the labels. This implicitly uses
/// .set if it avoids relocations.
void AsmEmitter::emitLabelDifference(const MCSymbol *Hi, const MCSymbol *Lo,
                                     unsigned Size) const {
  OutStreamer->emitAbsoluteSymbolDiff(Hi, Lo, Size);
}

/// Emit something like ".uleb128 Hi-Lo".
void AsmEmitter::emitLabelDifferenceAsULEB128(const MCSymbol *Hi,
                                              const MCSymbol *Lo) const {
  OutStreamer->emitAbsoluteSymbolDiffAsULEB128(Hi, Lo);
}

/// EmitLabelPlusOffset - Emit something like ".long Label+Offset"
/// where the size in bytes of the directive is specified by Size and Label
/// specifies the label.  This implicitly uses .set if it is available.
void AsmEmitter::emitLabelPlusOffset(const MCSymbol *Label, uint64_t Offset,
                                     unsigned Size,
                                     bool IsSectionRelative) const {
  if (MAI->needsDwarfSectionOffsetDirective() && IsSectionRelative) {
    OutStreamer->emitCOFFSecRel32(Label, Offset);
    if (Size > 4)
      OutStreamer->emitZeros(Size - 4);
    return;
  }

  // Emit Label+Offset (or just Label if Offset is zero)
  const MCExpr *Expr = MCSymbolRefExpr::create(Label, OutContext);
  if (Offset)
    Expr = MCBinaryExpr::createAdd(
        Expr, MCConstantExpr::create(Offset, OutContext), OutContext);

  OutStreamer->emitValue(Expr, Size);
}
