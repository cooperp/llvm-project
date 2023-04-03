//===- llvm/CodeGen/AsmEmitter.h - AsmEmitter Framework ---------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains a class to be used as the base class for target specific
// asm writers.  This class primarily handles common functionality used by
// all asm writers.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CODEGEN_ASMEMITTER_H
#define LLVM_CODEGEN_ASMEMITTER_H

#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

namespace llvm {

class DIE;
class DIEAbbrev;
class MCAsmInfo;
class MCCFIInstruction;
class MCContext;
class MCStreamer;
class MCSymbol;
class raw_ostream;
class TargetMachine;
class Twine;

/// This class is intended to be used as a driving class for all asm emitters.
class AsmEmitter {
public:

  /// Target Asm Printer information.
  const MCAsmInfo *MAI;

  /// This is the context for the output file that we are streaming. This owns
  /// all of the global MC-related objects for the generated translation unit.
  MCContext &OutContext;

  /// This is the MCStreamer object for the file we are generating. This
  /// contains the transient state for the current translation unit that we are
  /// generating (such as the current section etc).
  std::unique_ptr<MCStreamer> OutStreamer;

  explicit AsmEmitter(TargetMachine &TM, std::unique_ptr<MCStreamer> Streamer);
  ~AsmEmitter();

  bool doesDwarfUseRelocationsAcrossSections() const {
    return DwarfUsesRelocationsAcrossSections;
  }

  void setDwarfUsesRelocationsAcrossSections(bool Enable) {
    DwarfUsesRelocationsAcrossSections = Enable;
  }

  //===------------------------------------------------------------------===//
  // Symbol Lowering Routines.
  //===------------------------------------------------------------------===//

  MCSymbol *createTempSymbol(const Twine &Name) const;

  //===------------------------------------------------------------------===//
  // Emission Helper Routines.
  //===------------------------------------------------------------------===//

  /// Emit a byte directive and value.
  void emitInt8(int Value) const;

  /// Emit a short directive and value.
  void emitInt16(int Value) const;

  /// Emit a long directive and value.
  void emitInt32(int Value) const;

  /// Emit a long long directive and value.
  void emitInt64(uint64_t Value) const;

  /// Emit the specified signed leb128 value.
  void emitSLEB128(int64_t Value, const char *Desc = nullptr) const;

  /// Emit the specified unsigned leb128 value.
  void emitULEB128(uint64_t Value, const char *Desc = nullptr,
                   unsigned PadTo = 0) const;

  /// Emit something like ".long Hi-Lo" where the size in bytes of the directive
  /// is specified by Size and Hi/Lo specify the labels.  This implicitly uses
  /// .set if it is available.
  void emitLabelDifference(const MCSymbol *Hi, const MCSymbol *Lo,
                           unsigned Size) const;

  /// Emit something like ".uleb128 Hi-Lo".
  void emitLabelDifferenceAsULEB128(const MCSymbol *Hi,
                                    const MCSymbol *Lo) const;

  /// Emit something like ".long Label+Offset" where the size in bytes of the
  /// directive is specified by Size and Label specifies the label.  This
  /// implicitly uses .set if it is available.
  void emitLabelPlusOffset(const MCSymbol *Label, uint64_t Offset,
                           unsigned Size, bool IsSectionRelative = false) const;

  /// Emit something like ".long Label" where the size in bytes of the directive
  /// is specified by Size and Label specifies the label.
  void emitLabelReference(const MCSymbol *Label, unsigned Size,
                          bool IsSectionRelative = false) const {
    emitLabelPlusOffset(Label, 0, Size, IsSectionRelative);
  }

  //===------------------------------------------------------------------===//
  // Dwarf Lowering Routines
  //===------------------------------------------------------------------===//

  /// Emit frame instruction to describe the layout of the frame.
  void emitCFIInstruction(const MCCFIInstruction &Inst) const;

private:

  bool DwarfUsesRelocationsAcrossSections = false;

  /// Emit comments in assembly output if this is true.
  bool VerboseAsm;

  /// Return true if assembly output should contain comments.
  bool isVerbose() const { return VerboseAsm; }
};

} // end namespace llvm

#endif // LLVM_CODEGEN_ASMEMITTER_H
