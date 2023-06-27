/* This file was generated by upbc (the upb compiler) from the input
 * file:
 *
 *     envoy/type/matcher/v3/regex.proto
 *
 * Do not edit -- your changes will be discarded when the file is
 * regenerated. */

#ifndef ENVOY_TYPE_MATCHER_V3_REGEX_PROTO_UPB_H_
#define ENVOY_TYPE_MATCHER_V3_REGEX_PROTO_UPB_H_

#include "upb/msg_internal.h"
#include "upb/decode.h"
#include "upb/decode_fast.h"
#include "upb/encode.h"

#include "upb/port_def.inc"

#ifdef __cplusplus
extern "C" {
#endif

struct envoy_type_matcher_v3_RegexMatcher;
struct envoy_type_matcher_v3_RegexMatcher_GoogleRE2;
struct envoy_type_matcher_v3_RegexMatchAndSubstitute;
typedef struct envoy_type_matcher_v3_RegexMatcher envoy_type_matcher_v3_RegexMatcher;
typedef struct envoy_type_matcher_v3_RegexMatcher_GoogleRE2 envoy_type_matcher_v3_RegexMatcher_GoogleRE2;
typedef struct envoy_type_matcher_v3_RegexMatchAndSubstitute envoy_type_matcher_v3_RegexMatchAndSubstitute;
extern const upb_MiniTable envoy_type_matcher_v3_RegexMatcher_msginit;
extern const upb_MiniTable envoy_type_matcher_v3_RegexMatcher_GoogleRE2_msginit;
extern const upb_MiniTable envoy_type_matcher_v3_RegexMatchAndSubstitute_msginit;
struct google_protobuf_UInt32Value;
extern const upb_MiniTable google_protobuf_UInt32Value_msginit;



/* envoy.type.matcher.v3.RegexMatcher */

UPB_INLINE envoy_type_matcher_v3_RegexMatcher* envoy_type_matcher_v3_RegexMatcher_new(upb_Arena* arena) {
  return (envoy_type_matcher_v3_RegexMatcher*)_upb_Message_New(&envoy_type_matcher_v3_RegexMatcher_msginit, arena);
}
UPB_INLINE envoy_type_matcher_v3_RegexMatcher* envoy_type_matcher_v3_RegexMatcher_parse(const char* buf, size_t size, upb_Arena* arena) {
  envoy_type_matcher_v3_RegexMatcher* ret = envoy_type_matcher_v3_RegexMatcher_new(arena);
  if (!ret) return NULL;
  if (upb_Decode(buf, size, ret, &envoy_type_matcher_v3_RegexMatcher_msginit, NULL, 0, arena) != kUpb_DecodeStatus_Ok) {
    return NULL;
  }
  return ret;
}
UPB_INLINE envoy_type_matcher_v3_RegexMatcher* envoy_type_matcher_v3_RegexMatcher_parse_ex(const char* buf, size_t size,
                           const upb_ExtensionRegistry* extreg,
                           int options, upb_Arena* arena) {
  envoy_type_matcher_v3_RegexMatcher* ret = envoy_type_matcher_v3_RegexMatcher_new(arena);
  if (!ret) return NULL;
  if (upb_Decode(buf, size, ret, &envoy_type_matcher_v3_RegexMatcher_msginit, extreg, options, arena) !=
      kUpb_DecodeStatus_Ok) {
    return NULL;
  }
  return ret;
}
UPB_INLINE char* envoy_type_matcher_v3_RegexMatcher_serialize(const envoy_type_matcher_v3_RegexMatcher* msg, upb_Arena* arena, size_t* len) {
  return upb_Encode(msg, &envoy_type_matcher_v3_RegexMatcher_msginit, 0, arena, len);
}
UPB_INLINE char* envoy_type_matcher_v3_RegexMatcher_serialize_ex(const envoy_type_matcher_v3_RegexMatcher* msg, int options,
                                 upb_Arena* arena, size_t* len) {
  return upb_Encode(msg, &envoy_type_matcher_v3_RegexMatcher_msginit, options, arena, len);
}
typedef enum {
  envoy_type_matcher_v3_RegexMatcher_engine_type_google_re2 = 1,
  envoy_type_matcher_v3_RegexMatcher_engine_type_NOT_SET = 0
} envoy_type_matcher_v3_RegexMatcher_engine_type_oneofcases;
UPB_INLINE envoy_type_matcher_v3_RegexMatcher_engine_type_oneofcases envoy_type_matcher_v3_RegexMatcher_engine_type_case(const envoy_type_matcher_v3_RegexMatcher* msg) {
  return (envoy_type_matcher_v3_RegexMatcher_engine_type_oneofcases)*UPB_PTR_AT(msg, UPB_SIZE(0, 0), int32_t);
}
UPB_INLINE bool envoy_type_matcher_v3_RegexMatcher_has_google_re2(const envoy_type_matcher_v3_RegexMatcher* msg) {
  return _upb_getoneofcase(msg, UPB_SIZE(0, 0)) == 1;
}
UPB_INLINE void envoy_type_matcher_v3_RegexMatcher_clear_google_re2(const envoy_type_matcher_v3_RegexMatcher* msg) {
  UPB_WRITE_ONEOF(msg, envoy_type_matcher_v3_RegexMatcher_GoogleRE2*, UPB_SIZE(12, 24), 0, UPB_SIZE(0, 0), envoy_type_matcher_v3_RegexMatcher_engine_type_NOT_SET);
}
UPB_INLINE const envoy_type_matcher_v3_RegexMatcher_GoogleRE2* envoy_type_matcher_v3_RegexMatcher_google_re2(const envoy_type_matcher_v3_RegexMatcher* msg) {
  return UPB_READ_ONEOF(msg, const envoy_type_matcher_v3_RegexMatcher_GoogleRE2*, UPB_SIZE(12, 24), UPB_SIZE(0, 0), 1, NULL);
}
UPB_INLINE void envoy_type_matcher_v3_RegexMatcher_clear_regex(const envoy_type_matcher_v3_RegexMatcher* msg) {
  *UPB_PTR_AT(msg, UPB_SIZE(4, 8), upb_StringView) = upb_StringView_FromDataAndSize(NULL, 0);
}
UPB_INLINE upb_StringView envoy_type_matcher_v3_RegexMatcher_regex(const envoy_type_matcher_v3_RegexMatcher* msg) {
  return *UPB_PTR_AT(msg, UPB_SIZE(4, 8), upb_StringView);
}

UPB_INLINE void envoy_type_matcher_v3_RegexMatcher_set_google_re2(envoy_type_matcher_v3_RegexMatcher *msg, envoy_type_matcher_v3_RegexMatcher_GoogleRE2* value) {
  UPB_WRITE_ONEOF(msg, envoy_type_matcher_v3_RegexMatcher_GoogleRE2*, UPB_SIZE(12, 24), value, UPB_SIZE(0, 0), 1);
}
UPB_INLINE struct envoy_type_matcher_v3_RegexMatcher_GoogleRE2* envoy_type_matcher_v3_RegexMatcher_mutable_google_re2(envoy_type_matcher_v3_RegexMatcher* msg, upb_Arena* arena) {
  struct envoy_type_matcher_v3_RegexMatcher_GoogleRE2* sub = (struct envoy_type_matcher_v3_RegexMatcher_GoogleRE2*)envoy_type_matcher_v3_RegexMatcher_google_re2(msg);
  if (sub == NULL) {
    sub = (struct envoy_type_matcher_v3_RegexMatcher_GoogleRE2*)_upb_Message_New(&envoy_type_matcher_v3_RegexMatcher_GoogleRE2_msginit, arena);
    if (!sub) return NULL;
    envoy_type_matcher_v3_RegexMatcher_set_google_re2(msg, sub);
  }
  return sub;
}
UPB_INLINE void envoy_type_matcher_v3_RegexMatcher_set_regex(envoy_type_matcher_v3_RegexMatcher *msg, upb_StringView value) {
  *UPB_PTR_AT(msg, UPB_SIZE(4, 8), upb_StringView) = value;
}

/* envoy.type.matcher.v3.RegexMatcher.GoogleRE2 */

UPB_INLINE envoy_type_matcher_v3_RegexMatcher_GoogleRE2* envoy_type_matcher_v3_RegexMatcher_GoogleRE2_new(upb_Arena* arena) {
  return (envoy_type_matcher_v3_RegexMatcher_GoogleRE2*)_upb_Message_New(&envoy_type_matcher_v3_RegexMatcher_GoogleRE2_msginit, arena);
}
UPB_INLINE envoy_type_matcher_v3_RegexMatcher_GoogleRE2* envoy_type_matcher_v3_RegexMatcher_GoogleRE2_parse(const char* buf, size_t size, upb_Arena* arena) {
  envoy_type_matcher_v3_RegexMatcher_GoogleRE2* ret = envoy_type_matcher_v3_RegexMatcher_GoogleRE2_new(arena);
  if (!ret) return NULL;
  if (upb_Decode(buf, size, ret, &envoy_type_matcher_v3_RegexMatcher_GoogleRE2_msginit, NULL, 0, arena) != kUpb_DecodeStatus_Ok) {
    return NULL;
  }
  return ret;
}
UPB_INLINE envoy_type_matcher_v3_RegexMatcher_GoogleRE2* envoy_type_matcher_v3_RegexMatcher_GoogleRE2_parse_ex(const char* buf, size_t size,
                           const upb_ExtensionRegistry* extreg,
                           int options, upb_Arena* arena) {
  envoy_type_matcher_v3_RegexMatcher_GoogleRE2* ret = envoy_type_matcher_v3_RegexMatcher_GoogleRE2_new(arena);
  if (!ret) return NULL;
  if (upb_Decode(buf, size, ret, &envoy_type_matcher_v3_RegexMatcher_GoogleRE2_msginit, extreg, options, arena) !=
      kUpb_DecodeStatus_Ok) {
    return NULL;
  }
  return ret;
}
UPB_INLINE char* envoy_type_matcher_v3_RegexMatcher_GoogleRE2_serialize(const envoy_type_matcher_v3_RegexMatcher_GoogleRE2* msg, upb_Arena* arena, size_t* len) {
  return upb_Encode(msg, &envoy_type_matcher_v3_RegexMatcher_GoogleRE2_msginit, 0, arena, len);
}
UPB_INLINE char* envoy_type_matcher_v3_RegexMatcher_GoogleRE2_serialize_ex(const envoy_type_matcher_v3_RegexMatcher_GoogleRE2* msg, int options,
                                 upb_Arena* arena, size_t* len) {
  return upb_Encode(msg, &envoy_type_matcher_v3_RegexMatcher_GoogleRE2_msginit, options, arena, len);
}
UPB_INLINE bool envoy_type_matcher_v3_RegexMatcher_GoogleRE2_has_max_program_size(const envoy_type_matcher_v3_RegexMatcher_GoogleRE2* msg) {
  return _upb_hasbit(msg, 1);
}
UPB_INLINE void envoy_type_matcher_v3_RegexMatcher_GoogleRE2_clear_max_program_size(const envoy_type_matcher_v3_RegexMatcher_GoogleRE2* msg) {
  *UPB_PTR_AT(msg, UPB_SIZE(4, 8), const upb_Message*) = NULL;
}
UPB_INLINE const struct google_protobuf_UInt32Value* envoy_type_matcher_v3_RegexMatcher_GoogleRE2_max_program_size(const envoy_type_matcher_v3_RegexMatcher_GoogleRE2* msg) {
  return *UPB_PTR_AT(msg, UPB_SIZE(4, 8), const struct google_protobuf_UInt32Value*);
}

UPB_INLINE void envoy_type_matcher_v3_RegexMatcher_GoogleRE2_set_max_program_size(envoy_type_matcher_v3_RegexMatcher_GoogleRE2 *msg, struct google_protobuf_UInt32Value* value) {
  _upb_sethas(msg, 1);
  *UPB_PTR_AT(msg, UPB_SIZE(4, 8), struct google_protobuf_UInt32Value*) = value;
}
UPB_INLINE struct google_protobuf_UInt32Value* envoy_type_matcher_v3_RegexMatcher_GoogleRE2_mutable_max_program_size(envoy_type_matcher_v3_RegexMatcher_GoogleRE2* msg, upb_Arena* arena) {
  struct google_protobuf_UInt32Value* sub = (struct google_protobuf_UInt32Value*)envoy_type_matcher_v3_RegexMatcher_GoogleRE2_max_program_size(msg);
  if (sub == NULL) {
    sub = (struct google_protobuf_UInt32Value*)_upb_Message_New(&google_protobuf_UInt32Value_msginit, arena);
    if (!sub) return NULL;
    envoy_type_matcher_v3_RegexMatcher_GoogleRE2_set_max_program_size(msg, sub);
  }
  return sub;
}

/* envoy.type.matcher.v3.RegexMatchAndSubstitute */

UPB_INLINE envoy_type_matcher_v3_RegexMatchAndSubstitute* envoy_type_matcher_v3_RegexMatchAndSubstitute_new(upb_Arena* arena) {
  return (envoy_type_matcher_v3_RegexMatchAndSubstitute*)_upb_Message_New(&envoy_type_matcher_v3_RegexMatchAndSubstitute_msginit, arena);
}
UPB_INLINE envoy_type_matcher_v3_RegexMatchAndSubstitute* envoy_type_matcher_v3_RegexMatchAndSubstitute_parse(const char* buf, size_t size, upb_Arena* arena) {
  envoy_type_matcher_v3_RegexMatchAndSubstitute* ret = envoy_type_matcher_v3_RegexMatchAndSubstitute_new(arena);
  if (!ret) return NULL;
  if (upb_Decode(buf, size, ret, &envoy_type_matcher_v3_RegexMatchAndSubstitute_msginit, NULL, 0, arena) != kUpb_DecodeStatus_Ok) {
    return NULL;
  }
  return ret;
}
UPB_INLINE envoy_type_matcher_v3_RegexMatchAndSubstitute* envoy_type_matcher_v3_RegexMatchAndSubstitute_parse_ex(const char* buf, size_t size,
                           const upb_ExtensionRegistry* extreg,
                           int options, upb_Arena* arena) {
  envoy_type_matcher_v3_RegexMatchAndSubstitute* ret = envoy_type_matcher_v3_RegexMatchAndSubstitute_new(arena);
  if (!ret) return NULL;
  if (upb_Decode(buf, size, ret, &envoy_type_matcher_v3_RegexMatchAndSubstitute_msginit, extreg, options, arena) !=
      kUpb_DecodeStatus_Ok) {
    return NULL;
  }
  return ret;
}
UPB_INLINE char* envoy_type_matcher_v3_RegexMatchAndSubstitute_serialize(const envoy_type_matcher_v3_RegexMatchAndSubstitute* msg, upb_Arena* arena, size_t* len) {
  return upb_Encode(msg, &envoy_type_matcher_v3_RegexMatchAndSubstitute_msginit, 0, arena, len);
}
UPB_INLINE char* envoy_type_matcher_v3_RegexMatchAndSubstitute_serialize_ex(const envoy_type_matcher_v3_RegexMatchAndSubstitute* msg, int options,
                                 upb_Arena* arena, size_t* len) {
  return upb_Encode(msg, &envoy_type_matcher_v3_RegexMatchAndSubstitute_msginit, options, arena, len);
}
UPB_INLINE bool envoy_type_matcher_v3_RegexMatchAndSubstitute_has_pattern(const envoy_type_matcher_v3_RegexMatchAndSubstitute* msg) {
  return _upb_hasbit(msg, 1);
}
UPB_INLINE void envoy_type_matcher_v3_RegexMatchAndSubstitute_clear_pattern(const envoy_type_matcher_v3_RegexMatchAndSubstitute* msg) {
  *UPB_PTR_AT(msg, UPB_SIZE(4, 8), const upb_Message*) = NULL;
}
UPB_INLINE const envoy_type_matcher_v3_RegexMatcher* envoy_type_matcher_v3_RegexMatchAndSubstitute_pattern(const envoy_type_matcher_v3_RegexMatchAndSubstitute* msg) {
  return *UPB_PTR_AT(msg, UPB_SIZE(4, 8), const envoy_type_matcher_v3_RegexMatcher*);
}
UPB_INLINE void envoy_type_matcher_v3_RegexMatchAndSubstitute_clear_substitution(const envoy_type_matcher_v3_RegexMatchAndSubstitute* msg) {
  *UPB_PTR_AT(msg, UPB_SIZE(8, 16), upb_StringView) = upb_StringView_FromDataAndSize(NULL, 0);
}
UPB_INLINE upb_StringView envoy_type_matcher_v3_RegexMatchAndSubstitute_substitution(const envoy_type_matcher_v3_RegexMatchAndSubstitute* msg) {
  return *UPB_PTR_AT(msg, UPB_SIZE(8, 16), upb_StringView);
}

UPB_INLINE void envoy_type_matcher_v3_RegexMatchAndSubstitute_set_pattern(envoy_type_matcher_v3_RegexMatchAndSubstitute *msg, envoy_type_matcher_v3_RegexMatcher* value) {
  _upb_sethas(msg, 1);
  *UPB_PTR_AT(msg, UPB_SIZE(4, 8), envoy_type_matcher_v3_RegexMatcher*) = value;
}
UPB_INLINE struct envoy_type_matcher_v3_RegexMatcher* envoy_type_matcher_v3_RegexMatchAndSubstitute_mutable_pattern(envoy_type_matcher_v3_RegexMatchAndSubstitute* msg, upb_Arena* arena) {
  struct envoy_type_matcher_v3_RegexMatcher* sub = (struct envoy_type_matcher_v3_RegexMatcher*)envoy_type_matcher_v3_RegexMatchAndSubstitute_pattern(msg);
  if (sub == NULL) {
    sub = (struct envoy_type_matcher_v3_RegexMatcher*)_upb_Message_New(&envoy_type_matcher_v3_RegexMatcher_msginit, arena);
    if (!sub) return NULL;
    envoy_type_matcher_v3_RegexMatchAndSubstitute_set_pattern(msg, sub);
  }
  return sub;
}
UPB_INLINE void envoy_type_matcher_v3_RegexMatchAndSubstitute_set_substitution(envoy_type_matcher_v3_RegexMatchAndSubstitute *msg, upb_StringView value) {
  *UPB_PTR_AT(msg, UPB_SIZE(8, 16), upb_StringView) = value;
}

extern const upb_MiniTable_File envoy_type_matcher_v3_regex_proto_upb_file_layout;

#ifdef __cplusplus
}  /* extern "C" */
#endif

#include "upb/port_undef.inc"

#endif  /* ENVOY_TYPE_MATCHER_V3_REGEX_PROTO_UPB_H_ */
