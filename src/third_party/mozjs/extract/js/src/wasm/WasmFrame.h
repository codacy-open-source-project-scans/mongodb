/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * vim: set ts=8 sts=2 et sw=2 tw=80:
 *
 * Copyright 2021 Mozilla Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef wasm_frame_h
#define wasm_frame_h

#include <stdint.h>
#include <type_traits>

#include "NamespaceImports.h"

#include "wasm/WasmConstants.h"
#include "wasm/WasmValue.h"

namespace js {
namespace wasm {

struct TlsData;

// Bit set as the lowest bit of a frame pointer, used in two different mutually
// exclusive situations:
// - either it's a low bit tag in a FramePointer value read from the
// Frame::callerFP of an inner wasm frame. This indicates the previous call
// frame has been set up by a JIT caller that directly called into a wasm
// function's body. This is only stored in Frame::callerFP for a wasm frame
// called from JIT code, and thus it can not appear in a JitActivation's
// exitFP.
// - or it's the low big tag set when exiting wasm code in JitActivation's
// exitFP.

constexpr uintptr_t ExitOrJitEntryFPTag = 0x1;

// wasm::Frame represents the bytes pushed by the call instruction and the
// fixed prologue generated by wasm::GenerateCallablePrologue.
//
// Across all architectures it is assumed that, before the call instruction, the
// stack pointer is WasmStackAlignment-aligned. Thus after the prologue, and
// before the function has made its stack reservation, the stack alignment is
// sizeof(Frame) % WasmStackAlignment.
//
// During MacroAssembler code generation, the bytes pushed after the wasm::Frame
// are counted by masm.framePushed. Thus, the stack alignment at any point in
// time is (sizeof(wasm::Frame) + masm.framePushed) % WasmStackAlignment.

class Frame {
  // See GenerateCallableEpilogue for why this must be
  // the first field of wasm::Frame (in a downward-growing stack).
  // It's either the caller's Frame*, for wasm callers, or the JIT caller frame
  // plus a tag otherwise.
  uint8_t* callerFP_;

  // The return address pushed by the call (in the case of ARM/MIPS the return
  // address is pushed by the first instruction of the prologue).
  void* returnAddress_;

 public:
  static constexpr uint32_t callerFPOffset() {
    return offsetof(Frame, callerFP_);
  }
  static constexpr uint32_t returnAddressOffset() {
    return offsetof(Frame, returnAddress_);
  }

  uint8_t* returnAddress() const {
    return reinterpret_cast<uint8_t*>(returnAddress_);
  }

  void** addressOfReturnAddress() {
    return reinterpret_cast<void**>(&returnAddress_);
  }

  uint8_t* rawCaller() const { return callerFP_; }

  Frame* wasmCaller() const {
    MOZ_ASSERT(!callerIsExitOrJitEntryFP());
    return reinterpret_cast<Frame*>(callerFP_);
  }

  bool callerIsExitOrJitEntryFP() const {
    return isExitOrJitEntryFP(callerFP_);
  }

  uint8_t* jitEntryCaller() const { return toJitEntryCaller(callerFP_); }

  static const Frame* fromUntaggedWasmExitFP(const void* savedFP) {
    MOZ_ASSERT(!isExitOrJitEntryFP(savedFP));
    return reinterpret_cast<const Frame*>(savedFP);
  }

  static bool isExitOrJitEntryFP(const void* fp) {
    return reinterpret_cast<uintptr_t>(fp) & ExitOrJitEntryFPTag;
  }

  static uint8_t* toJitEntryCaller(const void* fp) {
    MOZ_ASSERT(isExitOrJitEntryFP(fp));
    return reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(fp) &
                                      ~ExitOrJitEntryFPTag);
  }

  static uint8_t* addExitOrJitEntryFPTag(const Frame* fp) {
    MOZ_ASSERT(!isExitOrJitEntryFP(fp));
    return reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(fp) |
                                      ExitOrJitEntryFPTag);
  }
};

static_assert(!std::is_polymorphic_v<Frame>, "Frame doesn't need a vtable.");
static_assert(sizeof(Frame) == 2 * sizeof(void*),
              "Frame is a two pointer structure");

class FrameWithTls : public Frame {
  TlsData* calleeTls_;
  TlsData* callerTls_;

 public:
  TlsData* calleeTls() { return calleeTls_; }
  TlsData* callerTls() { return callerTls_; }

  constexpr static uint32_t sizeWithoutFrame() {
    return sizeof(wasm::FrameWithTls) - sizeof(wasm::Frame);
  }

  constexpr static uint32_t calleeTLSOffset() {
    return offsetof(FrameWithTls, calleeTls_) - sizeof(wasm::Frame);
  }

  constexpr static uint32_t callerTLSOffset() {
    return offsetof(FrameWithTls, callerTls_) - sizeof(wasm::Frame);
  }
};

static_assert(FrameWithTls::calleeTLSOffset() == 0u,
              "Callee tls stored right above the return address.");
static_assert(FrameWithTls::callerTLSOffset() == sizeof(void*),
              "Caller tls stored right above the callee tls.");

static_assert(FrameWithTls::sizeWithoutFrame() == 2 * sizeof(void*),
              "There are only two additional slots");

#if defined(JS_CODEGEN_ARM64)
static_assert(sizeof(Frame) % 16 == 0, "frame is aligned");
#endif

// A DebugFrame is a Frame with additional fields that are added after the
// normal function prologue by the baseline compiler. If a Module is compiled
// with debugging enabled, then all its code creates DebugFrames on the stack
// instead of just Frames. These extra fields are used by the Debugger API.

class DebugFrame {
  // The register results field.  Initialized only during the baseline
  // compiler's return sequence to allow the debugger to inspect and
  // modify the return values of a frame being debugged.
  union SpilledRegisterResult {
   private:
    int32_t i32_;
    int64_t i64_;
    intptr_t ref_;
    AnyRef anyref_;
    float f32_;
    double f64_;
#ifdef ENABLE_WASM_SIMD
    V128 v128_;
#endif
#ifdef DEBUG
    // Should we add a new value representation, this will remind us to update
    // SpilledRegisterResult.
    static inline void assertAllValueTypesHandled(ValType type) {
      switch (type.kind()) {
        case ValType::I32:
        case ValType::I64:
        case ValType::F32:
        case ValType::F64:
        case ValType::V128:
        case ValType::Rtt:
          return;
        case ValType::Ref:
          switch (type.refTypeKind()) {
            case RefType::Func:
            case RefType::Extern:
            case RefType::Eq:
            case RefType::TypeIndex:
              return;
          }
      }
    }
#endif
  };
  SpilledRegisterResult registerResults_[MaxRegisterResults];

  // The returnValue() method returns a HandleValue pointing to this field.
  js::Value cachedReturnJSValue_;

  // If the function returns multiple results, this field is initialized
  // to a pointer to the stack results.
  void* stackResultsPointer_;

  // The function index of this frame. Technically, this could be derived
  // given a PC into this frame (which could lookup the CodeRange which has
  // the function index), but this isn't always readily available.
  uint32_t funcIndex_;

  // Flags whose meaning are described below.
  union Flags {
    uint32_t allFlags;
    struct {
      uint32_t observing : 1;
      uint32_t isDebuggee : 1;
      uint32_t prevUpToDate : 1;
      uint32_t hasCachedSavedFrame : 1;
      uint32_t hasCachedReturnJSValue : 1;
      uint32_t hasSpilledRefRegisterResult : MaxRegisterResults;
    };
  } flags_{};

  // Avoid -Wunused-private-field warnings.
 protected:
#if defined(JS_CODEGEN_MIPS32) || defined(JS_CODEGEN_ARM) || \
    defined(JS_CODEGEN_X86) || defined(__wasi__)
  // See alignmentStaticAsserts().  For MIPS32, ARM32 and X86 DebugFrame is only
  // 4-byte aligned, so we add another word to get up to 8-byte
  // alignment.
  uint32_t padding_;
#endif
#if defined(ENABLE_WASM_SIMD) && defined(JS_CODEGEN_ARM64)
  uint64_t padding_;
#endif

 private:
  // The Frame goes at the end since the stack grows down.
  Frame frame_;

 public:
  static DebugFrame* from(Frame* fp);
  Frame& frame() { return frame_; }
  uint32_t funcIndex() const { return funcIndex_; }
  Instance* instance() const;
  GlobalObject* global() const;
  bool hasGlobal(const GlobalObject* global) const;
  JSObject* environmentChain() const;
  bool getLocal(uint32_t localIndex, MutableHandleValue vp);

  // The return value must be written from the unboxed representation in the
  // results union into cachedReturnJSValue_ by updateReturnJSValue() before
  // returnValue() can return a Handle to it.

  bool hasCachedReturnJSValue() const { return flags_.hasCachedReturnJSValue; }
  [[nodiscard]] bool updateReturnJSValue(JSContext* cx);
  HandleValue returnValue() const;
  void clearReturnJSValue();

  // Once the debugger observes a frame, it must be notified via
  // onLeaveFrame() before the frame is popped. Calling observe() ensures the
  // leave frame traps are enabled. Both methods are idempotent so the caller
  // doesn't have to worry about calling them more than once.

  void observe(JSContext* cx);
  void leave(JSContext* cx);

  // The 'isDebugge' bit is initialized to false and set by the WebAssembly
  // runtime right before a frame is exposed to the debugger, as required by
  // the Debugger API. The bit is then used for Debugger-internal purposes
  // afterwards.

  bool isDebuggee() const { return flags_.isDebuggee; }
  void setIsDebuggee() { flags_.isDebuggee = true; }
  void unsetIsDebuggee() { flags_.isDebuggee = false; }

  // These are opaque boolean flags used by the debugger to implement
  // AbstractFramePtr. They are initialized to false and not otherwise read or
  // written by wasm code or runtime.

  bool prevUpToDate() const { return flags_.prevUpToDate; }
  void setPrevUpToDate() { flags_.prevUpToDate = true; }
  void unsetPrevUpToDate() { flags_.prevUpToDate = false; }

  bool hasCachedSavedFrame() const { return flags_.hasCachedSavedFrame; }
  void setHasCachedSavedFrame() { flags_.hasCachedSavedFrame = true; }
  void clearHasCachedSavedFrame() { flags_.hasCachedSavedFrame = false; }

  bool hasSpilledRegisterRefResult(size_t n) const {
    uint32_t mask = hasSpilledRegisterRefResultBitMask(n);
    return (flags_.allFlags & mask) != 0;
  }

  // DebugFrame is accessed directly by JIT code.

  static constexpr size_t offsetOfRegisterResults() {
    return offsetof(DebugFrame, registerResults_);
  }
  static constexpr size_t offsetOfRegisterResult(size_t n) {
    MOZ_ASSERT(n < MaxRegisterResults);
    return offsetOfRegisterResults() + n * sizeof(SpilledRegisterResult);
  }
  static constexpr size_t offsetOfCachedReturnJSValue() {
    return offsetof(DebugFrame, cachedReturnJSValue_);
  }
  static constexpr size_t offsetOfStackResultsPointer() {
    return offsetof(DebugFrame, stackResultsPointer_);
  }
  static constexpr size_t offsetOfFlags() {
    return offsetof(DebugFrame, flags_);
  }
  static constexpr uint32_t hasSpilledRegisterRefResultBitMask(size_t n) {
    MOZ_ASSERT(n < MaxRegisterResults);
    union Flags flags{0};
    flags.hasSpilledRefRegisterResult = 1 << n;
    MOZ_ASSERT(flags.allFlags != 0);
    return flags.allFlags;
  }
  static constexpr size_t offsetOfFuncIndex() {
    return offsetof(DebugFrame, funcIndex_);
  }
  static constexpr size_t offsetOfFrame() {
    return offsetof(DebugFrame, frame_);
  }

  // DebugFrames are aligned to 8-byte aligned, allowing them to be placed in
  // an AbstractFramePtr.

  static const unsigned Alignment = 8;
  static void alignmentStaticAsserts();
};

}  // namespace wasm
}  // namespace js

#endif  // wasm_frame_h
