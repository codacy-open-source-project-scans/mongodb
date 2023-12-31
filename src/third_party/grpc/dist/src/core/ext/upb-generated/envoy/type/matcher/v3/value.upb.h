/* This file was generated by upbc (the upb compiler) from the input
 * file:
 *
 *     envoy/type/matcher/v3/value.proto
 *
 * Do not edit -- your changes will be discarded when the file is
 * regenerated. */

#ifndef ENVOY_TYPE_MATCHER_V3_VALUE_PROTO_UPB_H_
#define ENVOY_TYPE_MATCHER_V3_VALUE_PROTO_UPB_H_

#include "upb/generated_code_support.h"
// Must be last. 
#include "upb/port/def.inc"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct envoy_type_matcher_v3_ValueMatcher envoy_type_matcher_v3_ValueMatcher;
typedef struct envoy_type_matcher_v3_ValueMatcher_NullMatch envoy_type_matcher_v3_ValueMatcher_NullMatch;
typedef struct envoy_type_matcher_v3_ListMatcher envoy_type_matcher_v3_ListMatcher;
extern const upb_MiniTable envoy_type_matcher_v3_ValueMatcher_msg_init;
extern const upb_MiniTable envoy_type_matcher_v3_ValueMatcher_NullMatch_msg_init;
extern const upb_MiniTable envoy_type_matcher_v3_ListMatcher_msg_init;
struct envoy_type_matcher_v3_DoubleMatcher;
struct envoy_type_matcher_v3_StringMatcher;
extern const upb_MiniTable envoy_type_matcher_v3_DoubleMatcher_msg_init;
extern const upb_MiniTable envoy_type_matcher_v3_StringMatcher_msg_init;



/* envoy.type.matcher.v3.ValueMatcher */

UPB_INLINE envoy_type_matcher_v3_ValueMatcher* envoy_type_matcher_v3_ValueMatcher_new(upb_Arena* arena) {
  return (envoy_type_matcher_v3_ValueMatcher*)_upb_Message_New(&envoy_type_matcher_v3_ValueMatcher_msg_init, arena);
}
UPB_INLINE envoy_type_matcher_v3_ValueMatcher* envoy_type_matcher_v3_ValueMatcher_parse(const char* buf, size_t size, upb_Arena* arena) {
  envoy_type_matcher_v3_ValueMatcher* ret = envoy_type_matcher_v3_ValueMatcher_new(arena);
  if (!ret) return NULL;
  if (upb_Decode(buf, size, ret, &envoy_type_matcher_v3_ValueMatcher_msg_init, NULL, 0, arena) != kUpb_DecodeStatus_Ok) {
    return NULL;
  }
  return ret;
}
UPB_INLINE envoy_type_matcher_v3_ValueMatcher* envoy_type_matcher_v3_ValueMatcher_parse_ex(const char* buf, size_t size,
                           const upb_ExtensionRegistry* extreg,
                           int options, upb_Arena* arena) {
  envoy_type_matcher_v3_ValueMatcher* ret = envoy_type_matcher_v3_ValueMatcher_new(arena);
  if (!ret) return NULL;
  if (upb_Decode(buf, size, ret, &envoy_type_matcher_v3_ValueMatcher_msg_init, extreg, options, arena) !=
      kUpb_DecodeStatus_Ok) {
    return NULL;
  }
  return ret;
}
UPB_INLINE char* envoy_type_matcher_v3_ValueMatcher_serialize(const envoy_type_matcher_v3_ValueMatcher* msg, upb_Arena* arena, size_t* len) {
  char* ptr;
  (void)upb_Encode(msg, &envoy_type_matcher_v3_ValueMatcher_msg_init, 0, arena, &ptr, len);
  return ptr;
}
UPB_INLINE char* envoy_type_matcher_v3_ValueMatcher_serialize_ex(const envoy_type_matcher_v3_ValueMatcher* msg, int options,
                                 upb_Arena* arena, size_t* len) {
  char* ptr;
  (void)upb_Encode(msg, &envoy_type_matcher_v3_ValueMatcher_msg_init, options, arena, &ptr, len);
  return ptr;
}
typedef enum {
  envoy_type_matcher_v3_ValueMatcher_match_pattern_null_match = 1,
  envoy_type_matcher_v3_ValueMatcher_match_pattern_double_match = 2,
  envoy_type_matcher_v3_ValueMatcher_match_pattern_string_match = 3,
  envoy_type_matcher_v3_ValueMatcher_match_pattern_bool_match = 4,
  envoy_type_matcher_v3_ValueMatcher_match_pattern_present_match = 5,
  envoy_type_matcher_v3_ValueMatcher_match_pattern_list_match = 6,
  envoy_type_matcher_v3_ValueMatcher_match_pattern_NOT_SET = 0
} envoy_type_matcher_v3_ValueMatcher_match_pattern_oneofcases;
UPB_INLINE envoy_type_matcher_v3_ValueMatcher_match_pattern_oneofcases envoy_type_matcher_v3_ValueMatcher_match_pattern_case(const envoy_type_matcher_v3_ValueMatcher* msg) {
  const upb_MiniTableField field = {1, UPB_SIZE(4, 8), -1, 0, 11, (int)kUpb_FieldMode_Scalar | ((int)UPB_SIZE(kUpb_FieldRep_4Byte, kUpb_FieldRep_8Byte) << kUpb_FieldRep_Shift)};
  return (envoy_type_matcher_v3_ValueMatcher_match_pattern_oneofcases)upb_Message_WhichOneofFieldNumber(msg, &field);
}
UPB_INLINE void envoy_type_matcher_v3_ValueMatcher_clear_null_match(envoy_type_matcher_v3_ValueMatcher* msg) {
  const upb_MiniTableField field = {1, UPB_SIZE(4, 8), -1, 0, 11, (int)kUpb_FieldMode_Scalar | ((int)UPB_SIZE(kUpb_FieldRep_4Byte, kUpb_FieldRep_8Byte) << kUpb_FieldRep_Shift)};
  _upb_Message_ClearNonExtensionField(msg, &field);
}
UPB_INLINE const envoy_type_matcher_v3_ValueMatcher_NullMatch* envoy_type_matcher_v3_ValueMatcher_null_match(const envoy_type_matcher_v3_ValueMatcher* msg) {
  const envoy_type_matcher_v3_ValueMatcher_NullMatch* default_val = NULL;
  const envoy_type_matcher_v3_ValueMatcher_NullMatch* ret;
  const upb_MiniTableField field = {1, UPB_SIZE(4, 8), -1, 0, 11, (int)kUpb_FieldMode_Scalar | ((int)UPB_SIZE(kUpb_FieldRep_4Byte, kUpb_FieldRep_8Byte) << kUpb_FieldRep_Shift)};
  _upb_Message_GetNonExtensionField(msg, &field, &default_val, &ret);
  return ret;
}
UPB_INLINE bool envoy_type_matcher_v3_ValueMatcher_has_null_match(const envoy_type_matcher_v3_ValueMatcher* msg) {
  const upb_MiniTableField field = {1, UPB_SIZE(4, 8), -1, 0, 11, (int)kUpb_FieldMode_Scalar | ((int)UPB_SIZE(kUpb_FieldRep_4Byte, kUpb_FieldRep_8Byte) << kUpb_FieldRep_Shift)};
  return _upb_Message_HasNonExtensionField(msg, &field);
}
UPB_INLINE void envoy_type_matcher_v3_ValueMatcher_clear_double_match(envoy_type_matcher_v3_ValueMatcher* msg) {
  const upb_MiniTableField field = {2, UPB_SIZE(4, 8), -1, 1, 11, (int)kUpb_FieldMode_Scalar | ((int)UPB_SIZE(kUpb_FieldRep_4Byte, kUpb_FieldRep_8Byte) << kUpb_FieldRep_Shift)};
  _upb_Message_ClearNonExtensionField(msg, &field);
}
UPB_INLINE const struct envoy_type_matcher_v3_DoubleMatcher* envoy_type_matcher_v3_ValueMatcher_double_match(const envoy_type_matcher_v3_ValueMatcher* msg) {
  const struct envoy_type_matcher_v3_DoubleMatcher* default_val = NULL;
  const struct envoy_type_matcher_v3_DoubleMatcher* ret;
  const upb_MiniTableField field = {2, UPB_SIZE(4, 8), -1, 1, 11, (int)kUpb_FieldMode_Scalar | ((int)UPB_SIZE(kUpb_FieldRep_4Byte, kUpb_FieldRep_8Byte) << kUpb_FieldRep_Shift)};
  _upb_Message_GetNonExtensionField(msg, &field, &default_val, &ret);
  return ret;
}
UPB_INLINE bool envoy_type_matcher_v3_ValueMatcher_has_double_match(const envoy_type_matcher_v3_ValueMatcher* msg) {
  const upb_MiniTableField field = {2, UPB_SIZE(4, 8), -1, 1, 11, (int)kUpb_FieldMode_Scalar | ((int)UPB_SIZE(kUpb_FieldRep_4Byte, kUpb_FieldRep_8Byte) << kUpb_FieldRep_Shift)};
  return _upb_Message_HasNonExtensionField(msg, &field);
}
UPB_INLINE void envoy_type_matcher_v3_ValueMatcher_clear_string_match(envoy_type_matcher_v3_ValueMatcher* msg) {
  const upb_MiniTableField field = {3, UPB_SIZE(4, 8), -1, 2, 11, (int)kUpb_FieldMode_Scalar | ((int)UPB_SIZE(kUpb_FieldRep_4Byte, kUpb_FieldRep_8Byte) << kUpb_FieldRep_Shift)};
  _upb_Message_ClearNonExtensionField(msg, &field);
}
UPB_INLINE const struct envoy_type_matcher_v3_StringMatcher* envoy_type_matcher_v3_ValueMatcher_string_match(const envoy_type_matcher_v3_ValueMatcher* msg) {
  const struct envoy_type_matcher_v3_StringMatcher* default_val = NULL;
  const struct envoy_type_matcher_v3_StringMatcher* ret;
  const upb_MiniTableField field = {3, UPB_SIZE(4, 8), -1, 2, 11, (int)kUpb_FieldMode_Scalar | ((int)UPB_SIZE(kUpb_FieldRep_4Byte, kUpb_FieldRep_8Byte) << kUpb_FieldRep_Shift)};
  _upb_Message_GetNonExtensionField(msg, &field, &default_val, &ret);
  return ret;
}
UPB_INLINE bool envoy_type_matcher_v3_ValueMatcher_has_string_match(const envoy_type_matcher_v3_ValueMatcher* msg) {
  const upb_MiniTableField field = {3, UPB_SIZE(4, 8), -1, 2, 11, (int)kUpb_FieldMode_Scalar | ((int)UPB_SIZE(kUpb_FieldRep_4Byte, kUpb_FieldRep_8Byte) << kUpb_FieldRep_Shift)};
  return _upb_Message_HasNonExtensionField(msg, &field);
}
UPB_INLINE void envoy_type_matcher_v3_ValueMatcher_clear_bool_match(envoy_type_matcher_v3_ValueMatcher* msg) {
  const upb_MiniTableField field = {4, UPB_SIZE(4, 8), -1, kUpb_NoSub, 8, (int)kUpb_FieldMode_Scalar | ((int)kUpb_FieldRep_1Byte << kUpb_FieldRep_Shift)};
  _upb_Message_ClearNonExtensionField(msg, &field);
}
UPB_INLINE bool envoy_type_matcher_v3_ValueMatcher_bool_match(const envoy_type_matcher_v3_ValueMatcher* msg) {
  bool default_val = false;
  bool ret;
  const upb_MiniTableField field = {4, UPB_SIZE(4, 8), -1, kUpb_NoSub, 8, (int)kUpb_FieldMode_Scalar | ((int)kUpb_FieldRep_1Byte << kUpb_FieldRep_Shift)};
  _upb_Message_GetNonExtensionField(msg, &field, &default_val, &ret);
  return ret;
}
UPB_INLINE bool envoy_type_matcher_v3_ValueMatcher_has_bool_match(const envoy_type_matcher_v3_ValueMatcher* msg) {
  const upb_MiniTableField field = {4, UPB_SIZE(4, 8), -1, kUpb_NoSub, 8, (int)kUpb_FieldMode_Scalar | ((int)kUpb_FieldRep_1Byte << kUpb_FieldRep_Shift)};
  return _upb_Message_HasNonExtensionField(msg, &field);
}
UPB_INLINE void envoy_type_matcher_v3_ValueMatcher_clear_present_match(envoy_type_matcher_v3_ValueMatcher* msg) {
  const upb_MiniTableField field = {5, UPB_SIZE(4, 8), -1, kUpb_NoSub, 8, (int)kUpb_FieldMode_Scalar | ((int)kUpb_FieldRep_1Byte << kUpb_FieldRep_Shift)};
  _upb_Message_ClearNonExtensionField(msg, &field);
}
UPB_INLINE bool envoy_type_matcher_v3_ValueMatcher_present_match(const envoy_type_matcher_v3_ValueMatcher* msg) {
  bool default_val = false;
  bool ret;
  const upb_MiniTableField field = {5, UPB_SIZE(4, 8), -1, kUpb_NoSub, 8, (int)kUpb_FieldMode_Scalar | ((int)kUpb_FieldRep_1Byte << kUpb_FieldRep_Shift)};
  _upb_Message_GetNonExtensionField(msg, &field, &default_val, &ret);
  return ret;
}
UPB_INLINE bool envoy_type_matcher_v3_ValueMatcher_has_present_match(const envoy_type_matcher_v3_ValueMatcher* msg) {
  const upb_MiniTableField field = {5, UPB_SIZE(4, 8), -1, kUpb_NoSub, 8, (int)kUpb_FieldMode_Scalar | ((int)kUpb_FieldRep_1Byte << kUpb_FieldRep_Shift)};
  return _upb_Message_HasNonExtensionField(msg, &field);
}
UPB_INLINE void envoy_type_matcher_v3_ValueMatcher_clear_list_match(envoy_type_matcher_v3_ValueMatcher* msg) {
  const upb_MiniTableField field = {6, UPB_SIZE(4, 8), -1, 3, 11, (int)kUpb_FieldMode_Scalar | ((int)UPB_SIZE(kUpb_FieldRep_4Byte, kUpb_FieldRep_8Byte) << kUpb_FieldRep_Shift)};
  _upb_Message_ClearNonExtensionField(msg, &field);
}
UPB_INLINE const envoy_type_matcher_v3_ListMatcher* envoy_type_matcher_v3_ValueMatcher_list_match(const envoy_type_matcher_v3_ValueMatcher* msg) {
  const envoy_type_matcher_v3_ListMatcher* default_val = NULL;
  const envoy_type_matcher_v3_ListMatcher* ret;
  const upb_MiniTableField field = {6, UPB_SIZE(4, 8), -1, 3, 11, (int)kUpb_FieldMode_Scalar | ((int)UPB_SIZE(kUpb_FieldRep_4Byte, kUpb_FieldRep_8Byte) << kUpb_FieldRep_Shift)};
  _upb_Message_GetNonExtensionField(msg, &field, &default_val, &ret);
  return ret;
}
UPB_INLINE bool envoy_type_matcher_v3_ValueMatcher_has_list_match(const envoy_type_matcher_v3_ValueMatcher* msg) {
  const upb_MiniTableField field = {6, UPB_SIZE(4, 8), -1, 3, 11, (int)kUpb_FieldMode_Scalar | ((int)UPB_SIZE(kUpb_FieldRep_4Byte, kUpb_FieldRep_8Byte) << kUpb_FieldRep_Shift)};
  return _upb_Message_HasNonExtensionField(msg, &field);
}

UPB_INLINE void envoy_type_matcher_v3_ValueMatcher_set_null_match(envoy_type_matcher_v3_ValueMatcher *msg, envoy_type_matcher_v3_ValueMatcher_NullMatch* value) {
  const upb_MiniTableField field = {1, UPB_SIZE(4, 8), -1, 0, 11, (int)kUpb_FieldMode_Scalar | ((int)UPB_SIZE(kUpb_FieldRep_4Byte, kUpb_FieldRep_8Byte) << kUpb_FieldRep_Shift)};
  _upb_Message_SetNonExtensionField(msg, &field, &value);
}
UPB_INLINE struct envoy_type_matcher_v3_ValueMatcher_NullMatch* envoy_type_matcher_v3_ValueMatcher_mutable_null_match(envoy_type_matcher_v3_ValueMatcher* msg, upb_Arena* arena) {
  struct envoy_type_matcher_v3_ValueMatcher_NullMatch* sub = (struct envoy_type_matcher_v3_ValueMatcher_NullMatch*)envoy_type_matcher_v3_ValueMatcher_null_match(msg);
  if (sub == NULL) {
    sub = (struct envoy_type_matcher_v3_ValueMatcher_NullMatch*)_upb_Message_New(&envoy_type_matcher_v3_ValueMatcher_NullMatch_msg_init, arena);
    if (sub) envoy_type_matcher_v3_ValueMatcher_set_null_match(msg, sub);
  }
  return sub;
}
UPB_INLINE void envoy_type_matcher_v3_ValueMatcher_set_double_match(envoy_type_matcher_v3_ValueMatcher *msg, struct envoy_type_matcher_v3_DoubleMatcher* value) {
  const upb_MiniTableField field = {2, UPB_SIZE(4, 8), -1, 1, 11, (int)kUpb_FieldMode_Scalar | ((int)UPB_SIZE(kUpb_FieldRep_4Byte, kUpb_FieldRep_8Byte) << kUpb_FieldRep_Shift)};
  _upb_Message_SetNonExtensionField(msg, &field, &value);
}
UPB_INLINE struct envoy_type_matcher_v3_DoubleMatcher* envoy_type_matcher_v3_ValueMatcher_mutable_double_match(envoy_type_matcher_v3_ValueMatcher* msg, upb_Arena* arena) {
  struct envoy_type_matcher_v3_DoubleMatcher* sub = (struct envoy_type_matcher_v3_DoubleMatcher*)envoy_type_matcher_v3_ValueMatcher_double_match(msg);
  if (sub == NULL) {
    sub = (struct envoy_type_matcher_v3_DoubleMatcher*)_upb_Message_New(&envoy_type_matcher_v3_DoubleMatcher_msg_init, arena);
    if (sub) envoy_type_matcher_v3_ValueMatcher_set_double_match(msg, sub);
  }
  return sub;
}
UPB_INLINE void envoy_type_matcher_v3_ValueMatcher_set_string_match(envoy_type_matcher_v3_ValueMatcher *msg, struct envoy_type_matcher_v3_StringMatcher* value) {
  const upb_MiniTableField field = {3, UPB_SIZE(4, 8), -1, 2, 11, (int)kUpb_FieldMode_Scalar | ((int)UPB_SIZE(kUpb_FieldRep_4Byte, kUpb_FieldRep_8Byte) << kUpb_FieldRep_Shift)};
  _upb_Message_SetNonExtensionField(msg, &field, &value);
}
UPB_INLINE struct envoy_type_matcher_v3_StringMatcher* envoy_type_matcher_v3_ValueMatcher_mutable_string_match(envoy_type_matcher_v3_ValueMatcher* msg, upb_Arena* arena) {
  struct envoy_type_matcher_v3_StringMatcher* sub = (struct envoy_type_matcher_v3_StringMatcher*)envoy_type_matcher_v3_ValueMatcher_string_match(msg);
  if (sub == NULL) {
    sub = (struct envoy_type_matcher_v3_StringMatcher*)_upb_Message_New(&envoy_type_matcher_v3_StringMatcher_msg_init, arena);
    if (sub) envoy_type_matcher_v3_ValueMatcher_set_string_match(msg, sub);
  }
  return sub;
}
UPB_INLINE void envoy_type_matcher_v3_ValueMatcher_set_bool_match(envoy_type_matcher_v3_ValueMatcher *msg, bool value) {
  const upb_MiniTableField field = {4, UPB_SIZE(4, 8), -1, kUpb_NoSub, 8, (int)kUpb_FieldMode_Scalar | ((int)kUpb_FieldRep_1Byte << kUpb_FieldRep_Shift)};
  _upb_Message_SetNonExtensionField(msg, &field, &value);
}
UPB_INLINE void envoy_type_matcher_v3_ValueMatcher_set_present_match(envoy_type_matcher_v3_ValueMatcher *msg, bool value) {
  const upb_MiniTableField field = {5, UPB_SIZE(4, 8), -1, kUpb_NoSub, 8, (int)kUpb_FieldMode_Scalar | ((int)kUpb_FieldRep_1Byte << kUpb_FieldRep_Shift)};
  _upb_Message_SetNonExtensionField(msg, &field, &value);
}
UPB_INLINE void envoy_type_matcher_v3_ValueMatcher_set_list_match(envoy_type_matcher_v3_ValueMatcher *msg, envoy_type_matcher_v3_ListMatcher* value) {
  const upb_MiniTableField field = {6, UPB_SIZE(4, 8), -1, 3, 11, (int)kUpb_FieldMode_Scalar | ((int)UPB_SIZE(kUpb_FieldRep_4Byte, kUpb_FieldRep_8Byte) << kUpb_FieldRep_Shift)};
  _upb_Message_SetNonExtensionField(msg, &field, &value);
}
UPB_INLINE struct envoy_type_matcher_v3_ListMatcher* envoy_type_matcher_v3_ValueMatcher_mutable_list_match(envoy_type_matcher_v3_ValueMatcher* msg, upb_Arena* arena) {
  struct envoy_type_matcher_v3_ListMatcher* sub = (struct envoy_type_matcher_v3_ListMatcher*)envoy_type_matcher_v3_ValueMatcher_list_match(msg);
  if (sub == NULL) {
    sub = (struct envoy_type_matcher_v3_ListMatcher*)_upb_Message_New(&envoy_type_matcher_v3_ListMatcher_msg_init, arena);
    if (sub) envoy_type_matcher_v3_ValueMatcher_set_list_match(msg, sub);
  }
  return sub;
}

/* envoy.type.matcher.v3.ValueMatcher.NullMatch */

UPB_INLINE envoy_type_matcher_v3_ValueMatcher_NullMatch* envoy_type_matcher_v3_ValueMatcher_NullMatch_new(upb_Arena* arena) {
  return (envoy_type_matcher_v3_ValueMatcher_NullMatch*)_upb_Message_New(&envoy_type_matcher_v3_ValueMatcher_NullMatch_msg_init, arena);
}
UPB_INLINE envoy_type_matcher_v3_ValueMatcher_NullMatch* envoy_type_matcher_v3_ValueMatcher_NullMatch_parse(const char* buf, size_t size, upb_Arena* arena) {
  envoy_type_matcher_v3_ValueMatcher_NullMatch* ret = envoy_type_matcher_v3_ValueMatcher_NullMatch_new(arena);
  if (!ret) return NULL;
  if (upb_Decode(buf, size, ret, &envoy_type_matcher_v3_ValueMatcher_NullMatch_msg_init, NULL, 0, arena) != kUpb_DecodeStatus_Ok) {
    return NULL;
  }
  return ret;
}
UPB_INLINE envoy_type_matcher_v3_ValueMatcher_NullMatch* envoy_type_matcher_v3_ValueMatcher_NullMatch_parse_ex(const char* buf, size_t size,
                           const upb_ExtensionRegistry* extreg,
                           int options, upb_Arena* arena) {
  envoy_type_matcher_v3_ValueMatcher_NullMatch* ret = envoy_type_matcher_v3_ValueMatcher_NullMatch_new(arena);
  if (!ret) return NULL;
  if (upb_Decode(buf, size, ret, &envoy_type_matcher_v3_ValueMatcher_NullMatch_msg_init, extreg, options, arena) !=
      kUpb_DecodeStatus_Ok) {
    return NULL;
  }
  return ret;
}
UPB_INLINE char* envoy_type_matcher_v3_ValueMatcher_NullMatch_serialize(const envoy_type_matcher_v3_ValueMatcher_NullMatch* msg, upb_Arena* arena, size_t* len) {
  char* ptr;
  (void)upb_Encode(msg, &envoy_type_matcher_v3_ValueMatcher_NullMatch_msg_init, 0, arena, &ptr, len);
  return ptr;
}
UPB_INLINE char* envoy_type_matcher_v3_ValueMatcher_NullMatch_serialize_ex(const envoy_type_matcher_v3_ValueMatcher_NullMatch* msg, int options,
                                 upb_Arena* arena, size_t* len) {
  char* ptr;
  (void)upb_Encode(msg, &envoy_type_matcher_v3_ValueMatcher_NullMatch_msg_init, options, arena, &ptr, len);
  return ptr;
}


/* envoy.type.matcher.v3.ListMatcher */

UPB_INLINE envoy_type_matcher_v3_ListMatcher* envoy_type_matcher_v3_ListMatcher_new(upb_Arena* arena) {
  return (envoy_type_matcher_v3_ListMatcher*)_upb_Message_New(&envoy_type_matcher_v3_ListMatcher_msg_init, arena);
}
UPB_INLINE envoy_type_matcher_v3_ListMatcher* envoy_type_matcher_v3_ListMatcher_parse(const char* buf, size_t size, upb_Arena* arena) {
  envoy_type_matcher_v3_ListMatcher* ret = envoy_type_matcher_v3_ListMatcher_new(arena);
  if (!ret) return NULL;
  if (upb_Decode(buf, size, ret, &envoy_type_matcher_v3_ListMatcher_msg_init, NULL, 0, arena) != kUpb_DecodeStatus_Ok) {
    return NULL;
  }
  return ret;
}
UPB_INLINE envoy_type_matcher_v3_ListMatcher* envoy_type_matcher_v3_ListMatcher_parse_ex(const char* buf, size_t size,
                           const upb_ExtensionRegistry* extreg,
                           int options, upb_Arena* arena) {
  envoy_type_matcher_v3_ListMatcher* ret = envoy_type_matcher_v3_ListMatcher_new(arena);
  if (!ret) return NULL;
  if (upb_Decode(buf, size, ret, &envoy_type_matcher_v3_ListMatcher_msg_init, extreg, options, arena) !=
      kUpb_DecodeStatus_Ok) {
    return NULL;
  }
  return ret;
}
UPB_INLINE char* envoy_type_matcher_v3_ListMatcher_serialize(const envoy_type_matcher_v3_ListMatcher* msg, upb_Arena* arena, size_t* len) {
  char* ptr;
  (void)upb_Encode(msg, &envoy_type_matcher_v3_ListMatcher_msg_init, 0, arena, &ptr, len);
  return ptr;
}
UPB_INLINE char* envoy_type_matcher_v3_ListMatcher_serialize_ex(const envoy_type_matcher_v3_ListMatcher* msg, int options,
                                 upb_Arena* arena, size_t* len) {
  char* ptr;
  (void)upb_Encode(msg, &envoy_type_matcher_v3_ListMatcher_msg_init, options, arena, &ptr, len);
  return ptr;
}
typedef enum {
  envoy_type_matcher_v3_ListMatcher_match_pattern_one_of = 1,
  envoy_type_matcher_v3_ListMatcher_match_pattern_NOT_SET = 0
} envoy_type_matcher_v3_ListMatcher_match_pattern_oneofcases;
UPB_INLINE envoy_type_matcher_v3_ListMatcher_match_pattern_oneofcases envoy_type_matcher_v3_ListMatcher_match_pattern_case(const envoy_type_matcher_v3_ListMatcher* msg) {
  const upb_MiniTableField field = {1, UPB_SIZE(4, 8), -1, 0, 11, (int)kUpb_FieldMode_Scalar | ((int)UPB_SIZE(kUpb_FieldRep_4Byte, kUpb_FieldRep_8Byte) << kUpb_FieldRep_Shift)};
  return (envoy_type_matcher_v3_ListMatcher_match_pattern_oneofcases)upb_Message_WhichOneofFieldNumber(msg, &field);
}
UPB_INLINE void envoy_type_matcher_v3_ListMatcher_clear_one_of(envoy_type_matcher_v3_ListMatcher* msg) {
  const upb_MiniTableField field = {1, UPB_SIZE(4, 8), -1, 0, 11, (int)kUpb_FieldMode_Scalar | ((int)UPB_SIZE(kUpb_FieldRep_4Byte, kUpb_FieldRep_8Byte) << kUpb_FieldRep_Shift)};
  _upb_Message_ClearNonExtensionField(msg, &field);
}
UPB_INLINE const envoy_type_matcher_v3_ValueMatcher* envoy_type_matcher_v3_ListMatcher_one_of(const envoy_type_matcher_v3_ListMatcher* msg) {
  const envoy_type_matcher_v3_ValueMatcher* default_val = NULL;
  const envoy_type_matcher_v3_ValueMatcher* ret;
  const upb_MiniTableField field = {1, UPB_SIZE(4, 8), -1, 0, 11, (int)kUpb_FieldMode_Scalar | ((int)UPB_SIZE(kUpb_FieldRep_4Byte, kUpb_FieldRep_8Byte) << kUpb_FieldRep_Shift)};
  _upb_Message_GetNonExtensionField(msg, &field, &default_val, &ret);
  return ret;
}
UPB_INLINE bool envoy_type_matcher_v3_ListMatcher_has_one_of(const envoy_type_matcher_v3_ListMatcher* msg) {
  const upb_MiniTableField field = {1, UPB_SIZE(4, 8), -1, 0, 11, (int)kUpb_FieldMode_Scalar | ((int)UPB_SIZE(kUpb_FieldRep_4Byte, kUpb_FieldRep_8Byte) << kUpb_FieldRep_Shift)};
  return _upb_Message_HasNonExtensionField(msg, &field);
}

UPB_INLINE void envoy_type_matcher_v3_ListMatcher_set_one_of(envoy_type_matcher_v3_ListMatcher *msg, envoy_type_matcher_v3_ValueMatcher* value) {
  const upb_MiniTableField field = {1, UPB_SIZE(4, 8), -1, 0, 11, (int)kUpb_FieldMode_Scalar | ((int)UPB_SIZE(kUpb_FieldRep_4Byte, kUpb_FieldRep_8Byte) << kUpb_FieldRep_Shift)};
  _upb_Message_SetNonExtensionField(msg, &field, &value);
}
UPB_INLINE struct envoy_type_matcher_v3_ValueMatcher* envoy_type_matcher_v3_ListMatcher_mutable_one_of(envoy_type_matcher_v3_ListMatcher* msg, upb_Arena* arena) {
  struct envoy_type_matcher_v3_ValueMatcher* sub = (struct envoy_type_matcher_v3_ValueMatcher*)envoy_type_matcher_v3_ListMatcher_one_of(msg);
  if (sub == NULL) {
    sub = (struct envoy_type_matcher_v3_ValueMatcher*)_upb_Message_New(&envoy_type_matcher_v3_ValueMatcher_msg_init, arena);
    if (sub) envoy_type_matcher_v3_ListMatcher_set_one_of(msg, sub);
  }
  return sub;
}

extern const upb_MiniTableFile envoy_type_matcher_v3_value_proto_upb_file_layout;

#ifdef __cplusplus
}  /* extern "C" */
#endif

#include "upb/port/undef.inc"

#endif  /* ENVOY_TYPE_MATCHER_V3_VALUE_PROTO_UPB_H_ */
