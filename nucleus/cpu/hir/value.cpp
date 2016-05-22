/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "value.h"
#include "nucleus/cpu/hir/block.h"
#include "nucleus/cpu/hir/function.h"
#include "nucleus/cpu/hir/instruction.h"
#include "nucleus/assert.h"

namespace cpu {
namespace hir {

S32 Value::getId() {
    if (id < 0) {
        Function* parFunction;
        if (flags & VALUE_IS_ARGUMENT) {
            parFunction = parent.function;
        } else {
            parFunction = parent.instruction->parent->parent;
        }
        id = parFunction->valueIdCounter++;
    }
    return id;
}

// Type properties
bool Value::isTypeInteger() const {
    return type == TYPE_I8 || type == TYPE_I16 || type == TYPE_I32 || type == TYPE_I64;
}

bool Value::isTypeFloat() const {
    return type == TYPE_F32 || type == TYPE_F64;
}

bool Value::isTypeVector() const {
    return type == TYPE_V128 || type == TYPE_V256;
}

// Constant properties
bool Value::isConstant() const {
    return (flags & VALUE_IS_CONSTANT);
}

bool Value::isConstantFalse() const {
    if (!isConstant()) {
        return false;
    }
    switch (type) {
    case TYPE_I8:   return !constant.i8;
    case TYPE_I16:  return !constant.i16;
    case TYPE_I32:  return !constant.i32;
    case TYPE_I64:  return !constant.i64;
    case TYPE_F32:  return !constant.f32;
    case TYPE_F64:  return !constant.f64;
    case TYPE_V128: assert_always("Unimplemented Value::isConstantFalse for TYPE_V128"); return false;
    case TYPE_V256: assert_always("Unimplemented Value::isConstantFalse for TYPE_V256"); return false;

    default:
        assert_always("Wrong type");
        return false;
    }
}

bool Value::isConstantTrue() const {
    if (!isConstant()) {
        return false;
    }
    switch (type) {
    case TYPE_I8:   return !!constant.i8;
    case TYPE_I16:  return !!constant.i16;
    case TYPE_I32:  return !!constant.i32;
    case TYPE_I64:  return !!constant.i64;
    case TYPE_F32:  return !!constant.f32;
    case TYPE_F64:  return !!constant.f64;
    case TYPE_V128: assert_always("Unimplemented Value::isConstantTrue for TYPE_V128"); return false;
    case TYPE_V256: assert_always("Unimplemented Value::isConstantTrue for TYPE_V256"); return false;

    default:
        assert_always("Wrong type");
        return false;
    }
}

bool Value::isConstantZero() const {
    if (!isConstant()) {
        return false;
    }
    switch (type) {
    case TYPE_I8:   return (constant.i8 == 0);
    case TYPE_I16:  return (constant.i16 == 0);
    case TYPE_I32:  return (constant.i32 == 0);
    case TYPE_I64:  return (constant.i64 == 0);
    case TYPE_F32:  return (constant.f32 == 0);
    case TYPE_F64:  return (constant.f64 == 0);
    case TYPE_V128: return (constant.v128.u64[0] == 0 && constant.v128.u64[1] == 0);
    case TYPE_V256: assert_always("Unimplemented Value::isConstantZero for TYPE_V256"); return false;

    default:
        assert_always("Wrong type");
        return false;
    }
}

// Constants setters
void Value::setConstantI8(U08 c) {
    constant.i8 = c;
    type = TYPE_I8;
    flags |= VALUE_IS_CONSTANT;
}
void Value::setConstantI16(U16 c) {
    constant.i16 = c;
    type = TYPE_I16;
    flags |= VALUE_IS_CONSTANT;
}
void Value::setConstantI32(U32 c) {
    constant.i32 = c;
    type = TYPE_I32;
    flags |= VALUE_IS_CONSTANT;
}
void Value::setConstantI64(U64 c) {
    constant.i64 = c;
    type = TYPE_I64;
    flags |= VALUE_IS_CONSTANT;
}
void Value::setConstantF32(F32 c) {
    constant.f32 = c;
    type = TYPE_F32;
    flags |= VALUE_IS_CONSTANT;
}
void Value::setConstantF64(F64 c) {
    constant.f64 = c;
    type = TYPE_F64;
    flags |= VALUE_IS_CONSTANT;
}
void Value::setConstantV128(V128 c) {
    constant.v128 = c;
    type = TYPE_V128;
    flags |= VALUE_IS_CONSTANT;
}
void Value::setConstantV256(V256 c) {
    constant.v256 = c;
    type = TYPE_V256;
    flags |= VALUE_IS_CONSTANT;
}

// Constant operations
void Value::doAdd(Value* rhs) {
    switch (type) {
    case TYPE_I8:   constant.i8  += rhs->constant.i8;   break;
    case TYPE_I16:  constant.i16 += rhs->constant.i16;  break;
    case TYPE_I32:  constant.i32 += rhs->constant.i32;  break;
    case TYPE_I64:  constant.i64 += rhs->constant.i64;  break;
    default:
        assert_always("Unimplemented case");
    }
}

void Value::doSub(Value* rhs) {
    switch (type) {
    case TYPE_I8:   constant.i8  -= rhs->constant.i8;   break;
    case TYPE_I16:  constant.i16 -= rhs->constant.i16;  break;
    case TYPE_I32:  constant.i32 -= rhs->constant.i32;  break;
    case TYPE_I64:  constant.i64 -= rhs->constant.i64;  break;
    default:
        assert_always("Unimplemented case");
    }
}

void Value::doMul(Value* rhs, ArithmeticFlags flags) {
    if (flags & ARITHMETIC_SIGNED) {
        switch (type) {
        case TYPE_I8:   constant.i8  *= rhs->constant.i8;   break;
        case TYPE_I16:  constant.i16 *= rhs->constant.i16;  break;
        case TYPE_I32:  constant.i32 *= rhs->constant.i32;  break;
        case TYPE_I64:  constant.i64 *= rhs->constant.i64;  break;
        default:
            assert_always("Unimplemented case");
        }
    } else {
        switch (type) {
        case TYPE_I8:   constant.i8  *= U08(rhs->constant.i8);    break;
        case TYPE_I16:  constant.i16 *= U16(rhs->constant.i16);  break;
        case TYPE_I32:  constant.i32 *= U32(rhs->constant.i32);  break;
        case TYPE_I64:  constant.i64 *= U64(rhs->constant.i64);  break;
        default:
            assert_always("Unimplemented case");
        }
    }
}

void Value::doMulH(Value* rhs, ArithmeticFlags flags) {
}

void Value::doDiv(Value* rhs, ArithmeticFlags flags) {
    if (flags & ARITHMETIC_SIGNED) {
        switch (type) {
        case TYPE_I8:   constant.i8  /= rhs->constant.i8;   break;
        case TYPE_I16:  constant.i16 /= rhs->constant.i16;  break;
        case TYPE_I32:  constant.i32 /= rhs->constant.i32;  break;
        case TYPE_I64:  constant.i64 /= rhs->constant.i64;  break;
        default:
            assert_always("Unimplemented case");
        }
    } else {
        switch (type) {
        case TYPE_I8:   constant.i8  /= U08(rhs->constant.i8);    break;
        case TYPE_I16:  constant.i16 /= U16(rhs->constant.i16);  break;
        case TYPE_I32:  constant.i32 /= U32(rhs->constant.i32);  break;
        case TYPE_I64:  constant.i64 /= U64(rhs->constant.i64);  break;
        default:
            assert_always("Unimplemented case");
        }
    }
}

void Value::doNeg() {
    switch (type) {
    case TYPE_I8:   constant.i8  = -constant.i8;   break;
    case TYPE_I16:  constant.i16 = -constant.i16;  break;
    case TYPE_I32:  constant.i32 = -constant.i32;  break;
    case TYPE_I64:  constant.i64 = -constant.i64;  break;
    default:
        assert_always("Unimplemented case");
    }
}

void Value::doAnd(Value* rhs) {
    switch (type) {
    case TYPE_I8:   constant.i8  &= rhs->constant.i8;   break;
    case TYPE_I16:  constant.i16 &= rhs->constant.i16;  break;
    case TYPE_I32:  constant.i32 &= rhs->constant.i32;  break;
    case TYPE_I64:  constant.i64 &= rhs->constant.i64;  break;
    default:
        assert_always("Unimplemented case");
    }
}

void Value::doOr(Value* rhs) {
    switch (type) {
    case TYPE_I8:   constant.i8  |= rhs->constant.i8;   break;
    case TYPE_I16:  constant.i16 |= rhs->constant.i16;  break;
    case TYPE_I32:  constant.i32 |= rhs->constant.i32;  break;
    case TYPE_I64:  constant.i64 |= rhs->constant.i64;  break;
    default:
        assert_always("Unimplemented case");
    }
}

void Value::doXor(Value* rhs) {
    switch (type) {
    case TYPE_I8:   constant.i8  ^= rhs->constant.i8;   break;
    case TYPE_I16:  constant.i16 ^= rhs->constant.i16;  break;
    case TYPE_I32:  constant.i32 ^= rhs->constant.i32;  break;
    case TYPE_I64:  constant.i64 ^= rhs->constant.i64;  break;
    default:
        assert_always("Unimplemented case");
    }
}

void Value::doNot() {
    switch (type) {
    case TYPE_I8:   constant.i8  = ~constant.i8;   break;
    case TYPE_I16:  constant.i16 = ~constant.i16;  break;
    case TYPE_I32:  constant.i32 = ~constant.i32;  break;
    case TYPE_I64:  constant.i64 = ~constant.i64;  break;
    default:
        assert_always("Unimplemented case");
    }
}

void Value::doShl(Value* amount) {
    switch (type) {
    case TYPE_I8:   constant.i8  <<= amount->constant.i8;  break;
    case TYPE_I16:  constant.i16 <<= amount->constant.i8;  break;
    case TYPE_I32:  constant.i32 <<= amount->constant.i8;  break;
    case TYPE_I64:  constant.i64 <<= amount->constant.i8;  break;
    default:
        assert_always("Unimplemented case");
    }
}

void Value::doShr(Value* amount) {
    switch (type) {
    case TYPE_I8:   constant.i8  = U08(constant.i8)   >> amount->constant.i8;  break;
    case TYPE_I16:  constant.i16 = U16(constant.i16) >> amount->constant.i8;  break;
    case TYPE_I32:  constant.i32 = U32(constant.i32) >> amount->constant.i8;  break;
    case TYPE_I64:  constant.i64 = U64(constant.i64) >> amount->constant.i8;  break;
    default:
        assert_always("Unimplemented case");
    }
}

void Value::doShrA(Value* amount) {
    switch (type) {
    case TYPE_I8:   constant.i8  = S08(constant.i8)   >> amount->constant.i8;  break;
    case TYPE_I16:  constant.i16 = S16(constant.i16) >> amount->constant.i8;  break;
    case TYPE_I32:  constant.i32 = S32(constant.i32) >> amount->constant.i8;  break;
    case TYPE_I64:  constant.i64 = S64(constant.i64) >> amount->constant.i8;  break;
    default:
        assert_always("Unimplemented case");
    }
}

void Value::doZExt(Type newType) {
    switch (type) {
    case TYPE_I8:   type = newType; constant.i64 &= 0xFF;        break;
    case TYPE_I16:  type = newType; constant.i64 &= 0xFFFF;      break;
    case TYPE_I32:  type = newType; constant.i64 &= 0xFFFFFFFF;  break;
    default:
        assert_always("Unimplemented case");
    }
}

void Value::doSExt(Type newType) {
    switch (type) {
    case TYPE_I8:
        type = newType;
        switch (newType) {
        case TYPE_I16:  constant.i16 = constant.i8;  return;
        case TYPE_I32:  constant.i32 = constant.i8;  return;
        case TYPE_I64:  constant.i64 = constant.i8;  return;
        default:
            assert_always("Unimplemented case");
            return;
        }
    case TYPE_I16:
        type = newType;
        switch (newType) {
        case TYPE_I32:  constant.i32 = constant.i16;  return;
        case TYPE_I64:  constant.i64 = constant.i16;  return;
        default:
            assert_always("Unimplemented case");
            return;
        }
    case TYPE_I32:
        type = newType;
        switch (newType) {
        case TYPE_I64:  constant.i64 = constant.i32;  return;
        default:
            assert_always("Unimplemented case");
            return;
        }
    default:
        assert_always("Unimplemented case");
    }
}

void Value::doTrunc(Type newType) {
    switch (type) {
    case TYPE_I16:
        type = newType;
        switch (newType) {
        case TYPE_I8:  constant.i64 &= 0xFF;  return;
        default:
            assert_always("Unimplemented case");
            return;
        }
    case TYPE_I32:
        type = newType;
        switch (newType) {
        case TYPE_I8:   constant.i64 &= 0xFF;    return;
        case TYPE_I16:  constant.i64 &= 0xFFFF;  return;
        default:
            assert_always("Unimplemented case");
            return;
        }
    case TYPE_I64:
        type = newType;
        switch (newType) {
        case TYPE_I8:   constant.i64 &= 0xFF;        return;
        case TYPE_I16:  constant.i64 &= 0xFFFF;      return;
        case TYPE_I32:  constant.i64 &= 0xFFFFFFFF;  return;
        default:
            assert_always("Unimplemented case");
            return;
        }
    default:
        assert_always("Unimplemented case");
    }
}

void Value::doCast(Type newType) {
    type = newType;
}

void Value::doConvert(Type newType) {
    switch (type) {
    case TYPE_I32:
        type = newType;
        switch (newType) {
        case TYPE_I8:  constant.i8  =  S08(constant.i32); return;
        case TYPE_I16: constant.i16 = S16(constant.i32); return;
        case TYPE_I32: constant.i32 = S32(constant.i32); return;
        case TYPE_I64: constant.i64 = S64(constant.i32); return;
        case TYPE_F32: constant.f32 = F32(constant.i32); return;
        case TYPE_F64: constant.f64 = F64(constant.i32); return;
        default:
            assert_always("Unimplemented case");
            return;
        }
    case TYPE_I64:
        type = newType;
        switch (newType) {
        case TYPE_I8:  constant.i8  =  S08(constant.i64); return;
        case TYPE_I16: constant.i16 = S16(constant.i64); return;
        case TYPE_I32: constant.i32 = S32(constant.i64); return;
        case TYPE_I64: constant.i64 = S64(constant.i64); return;
        case TYPE_F32: constant.f32 = F32(constant.i64); return;
        case TYPE_F64: constant.f64 = F64(constant.i64); return;
        default:
            assert_always("Unimplemented case");
            return;
        }
    case TYPE_F32:
        type = newType;
        switch (newType) {
        case TYPE_I8:  constant.i8  =  S08(constant.f32); return;
        case TYPE_I16: constant.i16 = S16(constant.f32); return;
        case TYPE_I32: constant.i32 = S32(constant.f32); return;
        case TYPE_I64: constant.i64 = S64(constant.f32); return;
        case TYPE_F32: constant.f32 = F32(constant.f32); return;
        case TYPE_F64: constant.f64 = F64(constant.f32); return;
        default:
            assert_always("Unimplemented case");
            return;
        }
    case TYPE_F64:
        type = newType;
        switch (newType) {
        case TYPE_I8:  constant.i8  =  S08(constant.f64); return;
        case TYPE_I16: constant.i16 = S16(constant.f64); return;
        case TYPE_I32: constant.i32 = S32(constant.f64); return;
        case TYPE_I64: constant.i64 = S64(constant.f64); return;
        case TYPE_F32: constant.f32 = F32(constant.f64); return;
        case TYPE_F64: constant.f64 = F64(constant.f64); return;
        default:
            assert_always("Unimplemented case");
            return;
        }
    default:
        assert_always("Unimplemented case");
        return;
    }
}

void Value::doCompare(Value* rhs, CompareFlags flags) {
    switch (type) {
    case TYPE_I8:
        switch (flags) {
        case COMPARE_EQ:  constant.i8 = (constant.i8 == rhs->constant.i8) ? 1 : 0;  break;
        case COMPARE_NE:  constant.i8 = (constant.i8 != rhs->constant.i8) ? 1 : 0;  break;
        case COMPARE_SLT: constant.i8 = (constant.i8 <  rhs->constant.i8) ? 1 : 0;  break;
        case COMPARE_SLE: constant.i8 = (constant.i8 <= rhs->constant.i8) ? 1 : 0;  break;
        case COMPARE_SGE: constant.i8 = (constant.i8 >= rhs->constant.i8) ? 1 : 0;  break;
        case COMPARE_SGT: constant.i8 = (constant.i8 >  rhs->constant.i8) ? 1 : 0;  break;
        case COMPARE_ULT: constant.i8 = (U08(constant.i8) <  U08(rhs->constant.i8)) ? 1 : 0;  break;
        case COMPARE_ULE: constant.i8 = (U08(constant.i8) <= U08(rhs->constant.i8)) ? 1 : 0;  break;
        case COMPARE_UGE: constant.i8 = (U08(constant.i8) >= U08(rhs->constant.i8)) ? 1 : 0;  break;
        case COMPARE_UGT: constant.i8 = (U08(constant.i8) >  U08(rhs->constant.i8)) ? 1 : 0;  break;
        }
        break;

    case TYPE_I16:
        switch (flags) {
        case COMPARE_EQ:  constant.i8 = (constant.i16 == rhs->constant.i16) ? 1 : 0;  break;
        case COMPARE_NE:  constant.i8 = (constant.i16 != rhs->constant.i16) ? 1 : 0;  break;
        case COMPARE_SLT: constant.i8 = (constant.i16 <  rhs->constant.i16) ? 1 : 0;  break;
        case COMPARE_SLE: constant.i8 = (constant.i16 <= rhs->constant.i16) ? 1 : 0;  break;
        case COMPARE_SGE: constant.i8 = (constant.i16 >= rhs->constant.i16) ? 1 : 0;  break;
        case COMPARE_SGT: constant.i8 = (constant.i16 >  rhs->constant.i16) ? 1 : 0;  break;
        case COMPARE_ULT: constant.i8 = (U16(constant.i16) <  U16(rhs->constant.i16)) ? 1 : 0;  break;
        case COMPARE_ULE: constant.i8 = (U16(constant.i16) <= U16(rhs->constant.i16)) ? 1 : 0;  break;
        case COMPARE_UGE: constant.i8 = (U16(constant.i16) >= U16(rhs->constant.i16)) ? 1 : 0;  break;
        case COMPARE_UGT: constant.i8 = (U16(constant.i16) >  U16(rhs->constant.i16)) ? 1 : 0;  break;
        }
        break;

    case TYPE_I32:
        switch (flags) {
        case COMPARE_EQ:  constant.i8 = (constant.i32 == rhs->constant.i32) ? 1 : 0;  break;
        case COMPARE_NE:  constant.i8 = (constant.i32 != rhs->constant.i32) ? 1 : 0;  break;
        case COMPARE_SLT: constant.i8 = (constant.i32 <  rhs->constant.i32) ? 1 : 0;  break;
        case COMPARE_SLE: constant.i8 = (constant.i32 <= rhs->constant.i32) ? 1 : 0;  break;
        case COMPARE_SGE: constant.i8 = (constant.i32 >= rhs->constant.i32) ? 1 : 0;  break;
        case COMPARE_SGT: constant.i8 = (constant.i32 >  rhs->constant.i32) ? 1 : 0;  break;
        case COMPARE_ULT: constant.i8 = (U32(constant.i32) <  U32(rhs->constant.i32)) ? 1 : 0;  break;
        case COMPARE_ULE: constant.i8 = (U32(constant.i32) <= U32(rhs->constant.i32)) ? 1 : 0;  break;
        case COMPARE_UGE: constant.i8 = (U32(constant.i32) >= U32(rhs->constant.i32)) ? 1 : 0;  break;
        case COMPARE_UGT: constant.i8 = (U32(constant.i32) >  U32(rhs->constant.i32)) ? 1 : 0;  break;
        }
        break;

    case TYPE_I64:
        switch (flags) {
        case COMPARE_EQ:  constant.i8 = (constant.i64 == rhs->constant.i64) ? 1 : 0;  break;
        case COMPARE_NE:  constant.i8 = (constant.i64 != rhs->constant.i64) ? 1 : 0;  break;
        case COMPARE_SLT: constant.i8 = (constant.i64 <  rhs->constant.i64) ? 1 : 0;  break;
        case COMPARE_SLE: constant.i8 = (constant.i64 <= rhs->constant.i64) ? 1 : 0;  break;
        case COMPARE_SGE: constant.i8 = (constant.i64 >= rhs->constant.i64) ? 1 : 0;  break;
        case COMPARE_SGT: constant.i8 = (constant.i64 >  rhs->constant.i64) ? 1 : 0;  break;
        case COMPARE_ULT: constant.i8 = (U64(constant.i64) <  U64(rhs->constant.i64)) ? 1 : 0;  break;
        case COMPARE_ULE: constant.i8 = (U64(constant.i64) <= U64(rhs->constant.i64)) ? 1 : 0;  break;
        case COMPARE_UGE: constant.i8 = (U64(constant.i64) >= U64(rhs->constant.i64)) ? 1 : 0;  break;
        case COMPARE_UGT: constant.i8 = (U64(constant.i64) >  U64(rhs->constant.i64)) ? 1 : 0;  break;
        }
        break;

    default:
        assert_always("Unimplemented case");
    }
    type = TYPE_I8;
}

}  // namespace hir
}  // namespace cpu
