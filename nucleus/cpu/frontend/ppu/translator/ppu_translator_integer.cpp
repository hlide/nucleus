/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_translator.h"
#include "nucleus/assert.h"
#include "nucleus/cpu/frontend/ppu/ppu_utils.h"

namespace cpu {
namespace frontend {
namespace ppu {

using namespace cpu::hir;

// Utilities
Value* addDidCarry(Builder& builder, Value* add, Value* lhs) {
    // 32-bit
    /*return builder.createCmpUGT(
        builder.createTrunc(v2, TYPE_I32),
        builder.createNot(builder.createTrunc(v1, TYPE_I32)));*/

    // 64-bit
    return builder.createCmpULT(add, lhs);
}

Value* subDidCarry(Builder& builder, Value* v1, Value* v2) {
    // 32-bit
    /*return builder.createOr(
        builder.createCmpUGT(
            builder.createTrunc(v1, TYPE_I32),
            builder.createNot(builder.createNeg(builder.createTrunc(v2, TYPE_I32)))),
        builder.createCmpEQ(
            builder.createTrunc(v2, TYPE_I32),
            builder.getConstantI32(0)));*/

    // 64-bit
    return builder.createOr(
        builder.createCmpUGT(v1, builder.createNot(builder.createNeg(v2))),
        builder.createCmpEQ(v2, builder.getConstantI64(0)));
}

Value* addWithCarryDidCarry(Builder& builder, Value* v1, Value* v2, Value* v3) {
    /*// 32-bit
    v1 = builder.createTrunc(v1, TYPE_I32);
    v2 = builder.createTrunc(v2, TYPE_I32);
    v3 = builder.createZExt(v3, TYPE_I32);
    return builder.createOr(
        builder.createCmpULT(builder.createAdd(builder.createAdd(v1, v2), v3), v3),
        builder.createCmpULT(builder.createAdd(v1, v2), v1));*/

    // 64-bit
    v3 = builder.createZExt(v3, TYPE_I64);
    return builder.createOr(
        builder.createCmpULT(builder.createAdd(builder.createAdd(v1, v2), v3), v3),
        builder.createCmpULT(builder.createAdd(v1, v2), v1));
}

/**
 * PPC64 Instructions:
 *  - UISA: Integer instructions (Section: 4.2.1)
 */

void Translator::addx(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rd;

    if (code.oe) {
        assert_always("Unimplemented: OE flag");
        rd = builder.getConstantI64(0); // TODO
    } else {
        rd = builder.createAdd(ra, rb);
    }

    if (code.rc) {
        updateCR0(rd);
    }

    setGPR(code.rd, rd);
}

void Translator::addcx(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rd;
    Value* ca;

    if (code.oe) {
        assert_always("Unimplemented");
        rd = builder.getConstantI64(0); // TODO
        ca = builder.getConstantI8(0); // TODO
        // TODO: XER OV update
    } else {
        rd = builder.createAdd(ra, rb);
        ca = addDidCarry(builder, rd, ra);
    }
    if (code.rc) {
        updateCR0(rd);
    }

    setXER_CA(ca);
    setGPR(code.rd, rd);
}

void Translator::addex(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* ca = getXER_CA();
    Value* rd;

    if (code.oe) {
        assert_always("Unimplemented");
        rd = builder.getConstantI64(0); // TODO
        // TODO: XER OV update
    } else {
        rd = builder.createAdd(ra, rb);
        rd = builder.createAdd(rd, builder.createZExt(ca, TYPE_I64));
        ca = addWithCarryDidCarry(builder, ra, rb, ca);
    }
    if (code.rc) {
        updateCR0(rd);
    }

    setXER_CA(ca);
    setGPR(code.rd, rd);
}

void Translator::addi(Instruction code)
{
    Value* simm = builder.getConstantI64(code.simm);
    Value* ra;
    Value* rd;

    if (code.ra) {
        ra = getGPR(code.ra);
        rd = builder.createAdd(ra, simm);
    } else {
        rd = simm;
    }

    setGPR(code.rd, rd);
}

void Translator::addic(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rd;
    Value* ca;

    Value* simm = builder.getConstantI64(code.simm);
    rd = builder.createAdd(ra, simm);
    ca = addDidCarry(builder, rd, ra);

    setXER_CA(ca);
    setGPR(code.rd, rd);
}

void Translator::addic_(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rd;
    Value* ca;

    Value* simm = builder.getConstantI64(code.simm);
    rd = builder.createAdd(ra, simm);
    ca = addDidCarry(builder, rd, ra);

    updateCR0(rd);

    setXER_CA(ca);
    setGPR(code.rd, rd);
}

void Translator::addis(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* simm = builder.getConstantI64(code.simm << 16);
    Value* rd;

    if (code.ra) {
        rd = builder.createAdd(ra, simm);
    } else {
        rd = simm;
    }

    setGPR(code.rd, rd);
}

void Translator::addmex(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* ca = getXER_CA();
    Value* rd;

    if (code.oe) {
        assert_always("Unimplemented");
        rd = builder.getConstantI64(0); // TODO
        // TODO: XER OV update
    } else {
        rd = builder.createSub(ra, builder.getConstantI64(1));
        rd = builder.createAdd(rd, builder.createZExt(ca, TYPE_I64));
        ca = addWithCarryDidCarry(builder, ra, builder.getConstantI64(-1), ca);
    }

    if (code.rc) {
        updateCR0(rd);
    }

    setXER_CA(ca);
    setGPR(code.rd, rd);
}

void Translator::addzex(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* ca = getXER_CA();
    Value* rd;

    if (code.oe) {
        assert_always("Unimplemented");
        rd = builder.getConstantI64(0); // TODO
        // TODO: XER OV update
    } else {
        rd = builder.createAdd(ra, builder.createZExt(ca, TYPE_I64));
        ca = addWithCarryDidCarry(builder, ra, builder.getConstantI64(0), ca);
    }

    if (code.rc) {
        updateCR0(rd);
    }

    setXER_CA(ca);
    setGPR(code.rd, rd);
}

void Translator::andx(Instruction code)
{
    Value* rs = getGPR(code.rs);
    Value* rb = getGPR(code.rb);
    Value* ra;

    ra = builder.createAnd(rs, rb);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Translator::andcx(Instruction code)
{
    Value* rb = getGPR(code.rb);
    Value* rs = getGPR(code.rs);
    Value* ra;

    ra = builder.createAnd(rs, builder.createNot(rb));
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Translator::andi_(Instruction code)
{
    Value* constant = builder.getConstantI64(code.uimm);
    Value* rs = getGPR(code.rs);
    Value* ra;

    ra = builder.createAnd(rs, constant);
    updateCR0(ra);

    setGPR(code.ra, ra);
}

void Translator::andis_(Instruction code)
{
    Value* constant = builder.getConstantI64(code.uimm << 16);
    Value* rs = getGPR(code.rs);
    Value* ra;

    ra = builder.createAnd(rs, constant);
    updateCR0(ra);

    setGPR(code.ra, ra);
}

void Translator::cmp(Instruction code)
{
    if (code.l10) {
        updateCR(code.crfd, getGPR(code.ra, TYPE_I64), getGPR(code.rb, TYPE_I64), false);
    } else {
        updateCR(code.crfd, getGPR(code.ra, TYPE_I32), getGPR(code.rb, TYPE_I32), false);
    }
}

void Translator::cmpi(Instruction code)
{
    if (code.l10) {
        updateCR(code.crfd, getGPR(code.ra, TYPE_I64), builder.getConstantI64(code.simm), false);
    } else {
        updateCR(code.crfd, getGPR(code.ra, TYPE_I32), builder.getConstantI32(code.simm), false);
    }
}

void Translator::cmpl(Instruction code)
{
    if (code.l10) {
        updateCR(code.crfd, getGPR(code.ra, TYPE_I64), getGPR(code.rb, TYPE_I64), true);
    } else {
        updateCR(code.crfd, getGPR(code.ra, TYPE_I32), getGPR(code.rb, TYPE_I32), true);
    }
}

void Translator::cmpli(Instruction code)
{
    if (code.l10) {
        updateCR(code.crfd, getGPR(code.ra, TYPE_I64), builder.getConstantI64(code.uimm), true);
    } else {
        updateCR(code.crfd, getGPR(code.ra, TYPE_I32), builder.getConstantI32(code.uimm), true);
    }
}

void Translator::cntlzdx(Instruction code)
{
    Value* rs = getGPR(code.rs);
    Value* ra;

    ra = builder.createCtlz(rs);
    ra = builder.createZExt(ra, TYPE_I64);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Translator::cntlzwx(Instruction code)
{
    Value* rs = getGPR(code.rs, TYPE_I32);
    Value* ra;

    ra = builder.createCtlz(rs);
    ra = builder.createZExt(ra, TYPE_I64);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Translator::divdx(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rd;

    rd = builder.createDiv(ra, rb, ARITHMETIC_SIGNED);

    if (code.oe) {
        assert_always("Unimplemented");
        // TODO: XER OV update
    }
    if (code.rc) {
        updateCR0(rd);
    }

    setGPR(code.rd, rd);
}

void Translator::divdux(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rd;

    rd = builder.createDiv(ra, rb, ARITHMETIC_UNSIGNED);

    if (code.oe) {
        assert_always("Unimplemented");
        // TODO: XER OV update
    }
    if (code.rc) {
        updateCR0(rd);
    }

    setGPR(code.rd, rd);
}

void Translator::divwx(Instruction code)
{
    Value* ra = getGPR(code.ra, TYPE_I32);
    Value* rb = getGPR(code.rb, TYPE_I32);
    Value* rd;

    auto result = builder.createDiv(ra, rb, ARITHMETIC_SIGNED);
    rd = builder.createZExt(result, TYPE_I64);

    if (code.oe) {
        assert_always("Unimplemented");
        // TODO: XER OV update
    }
    if (code.rc) {
        updateCR0(rd);
    }

    setGPR(code.rd, rd);
}

void Translator::divwux(Instruction code)
{
    Value* ra = getGPR(code.ra, TYPE_I32);
    Value* rb = getGPR(code.rb, TYPE_I32);
    Value* rd;

    auto result = builder.createDiv(ra, rb, ARITHMETIC_UNSIGNED);
    rd = builder.createZExt(result, TYPE_I64);

    if (code.oe) {
        assert_always("Unimplemented");
        // TODO: XER OV update
    }
    if (code.rc) {
        updateCR0(rd);
    }

    setGPR(code.rd, rd);
}

void Translator::eqvx(Instruction code)
{
    Value* rs = getGPR(code.rs);
    Value* rb = getGPR(code.rb);
    Value* ra;

    ra = builder.createXor(rs, rb);
    ra = builder.createNot(ra);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Translator::extsbx(Instruction code)
{
    Value* rs = getGPR(code.rs, TYPE_I8);
    Value* ra;

    ra = builder.createSExt(rs, TYPE_I64);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Translator::extshx(Instruction code)
{
    Value* rs = getGPR(code.rs, TYPE_I16);
    Value* ra;

    ra = builder.createSExt(rs, TYPE_I64);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Translator::extswx(Instruction code)
{
    Value* rs = getGPR(code.rs, TYPE_I32);
    Value* ra;

    ra = builder.createSExt(rs, TYPE_I64);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Translator::mulhdx(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rd;

    rd = builder.createMulH(ra, rb, ARITHMETIC_SIGNED);
    if (code.rc) {
        updateCR0(rd);
    }

    setGPR(code.rd, rd);
}

void Translator::mulhdux(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rd;

    rd = builder.createMulH(ra, rb, ARITHMETIC_UNSIGNED);
    if (code.rc) {
        updateCR0(rd);
    }

    setGPR(code.rd, rd);
}

void Translator::mulhwx(Instruction code)
{
    Value* ra = getGPR(code.ra, TYPE_I32);
    Value* rb = getGPR(code.rb, TYPE_I32);
    Value* rd;

    rd = builder.createMulH(ra, rb, ARITHMETIC_SIGNED);
    rd = builder.createZExt(rd, TYPE_I64);
    if (code.rc) {
        updateCR0(rd);
    }

    setGPR(code.rd, rd);
}

void Translator::mulhwux(Instruction code)
{
    Value* ra = getGPR(code.ra, TYPE_I32);
    Value* rb = getGPR(code.rb, TYPE_I32);
    Value* rd;

    rd = builder.createMulH(ra, rb, ARITHMETIC_UNSIGNED);
    rd = builder.createZExt(rd, TYPE_I64);
    if (code.rc) {
        updateCR0(rd);
    }

    setGPR(code.rd, rd);
}

void Translator::mulldx(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rd;

    rd = builder.createMul(ra, rb, ARITHMETIC_SIGNED);
    if (code.oe) {
        assert_always("Unimplemented");
        // TODO: XER OV update
    }
    if (code.rc) {
        updateCR0(rd);
    }

    setGPR(code.rd, rd);
}

void Translator::mulli(Instruction code)
{
    Value* constant = builder.getConstantI64(code.simm);
    Value* ra = getGPR(code.ra);
    Value* rd;

    rd = builder.createMul(ra, constant, ARITHMETIC_SIGNED);

    setGPR(code.rd, rd);
}

void Translator::mullwx(Instruction code)
{
    Value* ra = getGPR(code.ra, TYPE_I32);
    Value* rb = getGPR(code.rb, TYPE_I32);
    Value* rd;

    auto ra_i64 = builder.createSExt(ra, TYPE_I64);
    auto rb_i64 = builder.createSExt(rb, TYPE_I64);
    rd = builder.createMul(ra_i64, rb_i64, ARITHMETIC_SIGNED);

    if (code.oe) {
        assert_always("Unimplemented");
        // TODO: XER OV update
    }
    if (code.rc) {
        updateCR0(rd);
    }

    setGPR(code.rd, rd);
}

void Translator::nandx(Instruction code)
{
    Value* rs = getGPR(code.rs);
    Value* rb = getGPR(code.rb);
    Value* ra;

    ra = builder.createAnd(rs, rb);
    ra = builder.createNot(ra);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Translator::negx(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rd;

    rd = builder.createNeg(ra);
    if (code.oe) {
        assert_always("Unimplemented");
        // TODO: XER OV update
    }
    if (code.rc) {
        updateCR0(rd);
    }

    setGPR(code.rd, rd);
}

void Translator::norx(Instruction code)
{
    Value* rs = getGPR(code.rs);
    Value* rb = getGPR(code.rb);
    Value* ra;

    ra = builder.createOr(rs, rb);
    ra = builder.createNot(ra);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Translator::orx(Instruction code)
{
    Value* rs = getGPR(code.rs);
    Value* rb = getGPR(code.rb);
    Value* ra;

    ra = builder.createOr(rs, rb);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Translator::orcx(Instruction code)
{
    Value* rs = getGPR(code.rs);
    Value* rb = getGPR(code.rb);
    Value* ra;

    rb = builder.createNot(rb);
    ra = builder.createOr(rs, rb);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Translator::ori(Instruction code)
{
    Value* constant = builder.getConstantI64(code.uimm);
    Value* rs = getGPR(code.rs);
    Value* ra;

    ra = builder.createOr(rs, constant);

    setGPR(code.ra, ra);
}

void Translator::oris(Instruction code)
{
    Value* constant = builder.getConstantI64(code.uimm << 16);
    Value* rs = getGPR(code.rs);
    Value* ra;

    ra = builder.createOr(rs, constant);

    setGPR(code.ra, ra);
}

void Translator::rldc_lr(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::rldicx(Instruction code)
{
    Value* rs = getGPR(code.rs);
    Value* ra = rs;

    const U32 sh = code.sh | (code.sh_ << 5);
    const U32 mb = code.mb | (code.mb_ << 5);
    if (sh) {
        auto resl = builder.createShr(rs, 64 - sh);
        auto resh = builder.createShl(rs, sh);
        ra = builder.createOr(resh, resl);
    }

    ra = builder.createAnd(ra, builder.getConstantI64(rotateMask[mb][63 - sh]));
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Translator::rldiclx(Instruction code)
{
    Value* rs = getGPR(code.rs);
    Value* ra = rs;

    const U32 sh = code.sh | (code.sh_ << 5);
    const U32 mb = code.mb | (code.mb_ << 5);
    if (sh) {
        auto resl = builder.createShr(rs, 64 - sh);
        auto resh = builder.createShl(rs, sh);
        ra = builder.createOr(resh, resl);
    }

    ra = builder.createAnd(ra, builder.getConstantI64(rotateMask[mb][63]));
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Translator::rldicrx(Instruction code)
{
    Value* rs = getGPR(code.rs);
    Value* ra = rs;

    const U32 sh = code.sh | (code.sh_ << 5);
    const U32 me = code.me_ | (code.me__ << 5);
    if (sh) {
        auto resl = builder.createShr(rs, 64 - sh);
        auto resh = builder.createShl(rs, sh);
        ra = builder.createOr(resh, resl);
    }

    ra = builder.createAnd(ra, builder.getConstantI64(rotateMask[0][me]));
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Translator::rldimix(Instruction code)
{
    Value* rs = getGPR(code.rs);
    Value* ra = getGPR(code.ra);
    Value* temp = rs;

    const U32 sh = code.sh | (code.sh_ << 5);
    const U32 mb = code.mb | (code.mb_ << 5);
    if (sh) {
        auto resl = builder.createShr(rs, 64 - sh);
        auto resh = builder.createShl(rs, sh);
        temp = builder.createOr(resh, resl);
    }

    const U64 mask = rotateMask[mb][63 - sh];
    temp = builder.createAnd(temp, builder.getConstantI64(mask));
    ra = builder.createAnd(ra, builder.getConstantI64(~mask));
    ra = builder.createOr(ra, temp);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Translator::rlwimix(Instruction code)
{
    Value* rs_trunc = builder.createZExt(getGPR(code.rs, TYPE_I32), TYPE_I64);
    Value* rs_shift = builder.createShl(rs_trunc, 32);

    Value* rs = builder.createOr(rs_trunc, rs_shift);
    Value* ra = getGPR(code.ra);
    Value* temp = rs;

    if (code.sh) {
        auto resl = builder.createShr(rs, 64 - code.sh);
        auto resh = builder.createShl(rs, code.sh);
        temp = builder.createOr(resh, resl);
    }

    const U64 mask = rotateMask[32 + code.mb][32 + code.me];
    temp = builder.createAnd(temp, builder.getConstantI64(mask));
    ra = builder.createAnd(ra, builder.getConstantI64(~mask));
    ra = builder.createOr(ra, temp);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Translator::rlwinmx(Instruction code)
{
    Value* rs_trunc = builder.createZExt(getGPR(code.rs, TYPE_I32), TYPE_I64);
    Value* rs_shift = builder.createShl(rs_trunc, 32);

    Value* rs = builder.createOr(rs_trunc, rs_shift);
    Value* ra = rs;

    if (code.sh) {
        auto resl = builder.createShr(rs, 64 - code.sh);
        auto resh = builder.createShl(rs, code.sh);
        ra = builder.createOr(resh, resl);
    }

    ra = builder.createAnd(ra, builder.getConstantI64(rotateMask[32 + code.mb][32 + code.me]));
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Translator::rlwnmx(Instruction code)
{
    Value* rs_trunc = builder.createZExt(getGPR(code.rs, TYPE_I32), TYPE_I64);
    Value* rs_shift = builder.createShl(rs_trunc, 32);

    Value* rs = builder.createOr(rs_trunc, rs_shift);
    Value* rb = getGPR(code.rb);
    Value* ra;

    auto shl = builder.createAnd(rb, builder.getConstantI64(0x1F));
    auto shr = builder.createSub(builder.getConstantI64(32), shl);
    auto resl = builder.createShr(rs, shr);
    auto resh = builder.createShl(rs, shl);
    ra = builder.createOr(resh, resl);
    ra = builder.createAnd(ra, builder.getConstantI64(rotateMask[32 + code.mb][32 + code.me]));
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Translator::sldx(Instruction code)
{
    Value* rs = getGPR(code.rs);
    Value* rb = getGPR(code.rb, TYPE_I8);
    Value* ra;

    rb = builder.createAnd(rb, builder.getConstantI8(0x7F));
    ra = builder.createSelect(builder.createShr(rb, 6),
        builder.getConstantI64(0),
        builder.createShl(rs, rb));

    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Translator::slwx(Instruction code)
{
    Value* rs = getGPR(code.rs, TYPE_I32);
    Value* rb = getGPR(code.rb, TYPE_I8);
    Value* ra;

    rs = builder.createZExt(rs, TYPE_I64);
    rb = builder.createAnd(rb, builder.getConstantI8(0x3F));
    ra = builder.createZExt(builder.createTrunc(builder.createShl(rs, rb), TYPE_I32), TYPE_I64);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Translator::sradx(Instruction code)
{
    Value* rs = getGPR(code.rs);
    Value* rb = getGPR(code.rb);
    Value* ra;
    Value* ca;

    // TODO: This is totally wrong
    ra = builder.createShrA(rs, builder.createTrunc(rb, TYPE_I8));
    ca = builder.getConstantI8(0);
    if (code.rc) {
        updateCR0(ra);
    }

    setXER_CA(ca);
    setGPR(code.ra, ra);
}

void Translator::sradix(Instruction code)
{
    Value* rs = getGPR(code.rs);
    Value* ra;
    Value* ca;

    const U32 sh = code.sh | (code.sh_ << 5);
    if (sh == 0) {
        ra = rs;
        ca = builder.getConstantI8(0);
    } else {
        ra = builder.createShrA(rs, sh);
        ca = builder.getConstantI8(0); // TODO
    }

    if (code.rc) {
        updateCR0(ra);
    }

    setXER_CA(ca);
    setGPR(code.ra, ra);
}

void Translator::srawx(Instruction code)
{
    Value* rs = getGPR(code.rs, TYPE_I32);
    Value* rb = getGPR(code.rb, TYPE_I8);
    Value* ra;
    Value* ca;

    rs = builder.createShl(builder.createZExt(rs, TYPE_I64), 32);
    ra = builder.createShrA(rs, builder.createAnd(rb, builder.getConstantI8(0x3F)));
    ca = builder.createAnd(
        builder.createTrunc(builder.createShr(rs, 0x3F), TYPE_I8),
        builder.createCmpNE(builder.createTrunc(ra, TYPE_I32), builder.getConstantI32(0)));

    ra = builder.createShrA(ra, 32);
    if (code.rc) {
        updateCR0(ra);
    }

    setXER_CA(ca);
    setGPR(code.ra, ra);
}

void Translator::srawix(Instruction code)
{
    Value* rs = getGPR(code.rs, TYPE_I32);
    Value* ra;
    Value* ca;

    if (code.sh == 0) {
        ra = builder.createSExt(rs, TYPE_I64);
        ca = builder.getConstantI8(0);
    } else {
        rs = builder.createShl(builder.createZExt(rs, TYPE_I64), 32);
        ra = builder.createShrA(rs, code.sh);
        ca = builder.createAnd(
            builder.createTrunc(builder.createShr(rs, 0x3F), TYPE_I8),
            builder.createCmpNE(builder.createTrunc(ra, TYPE_I32), builder.getConstantI32(0)));
        ra = builder.createShrA(ra, 32);
    }

    if (code.rc) {
        updateCR0(ra);
    }

    setXER_CA(ca);
    setGPR(code.ra, ra);
}

void Translator::srdx(Instruction code)
{
    Value* rs = getGPR(code.rs);
    Value* rb = getGPR(code.rb, TYPE_I8);
    Value* ra;

    rs = builder.createZExt(rs, TYPE_I64);
    rb = builder.createAnd(rb, builder.getConstantI8(0x7F));
    ra = builder.createSelect(builder.createAnd(rb, builder.getConstantI8(0x40)),
        builder.getConstantI64(0),
        builder.createShr(rs, rb));

    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Translator::srwx(Instruction code)
{
    Value* rs = getGPR(code.rs, TYPE_I32);
    Value* rb = getGPR(code.rb, TYPE_I8);
    Value* ra;

    rs = builder.createZExt(rs, TYPE_I64);
    rb = builder.createAnd(rb, builder.getConstantI8(0x3F));
    ra = builder.createShr(rs, rb);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Translator::subfx(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rd;

    if (code.oe) {
        assert_always("Unimplemented");
        rd = builder.getConstantI64(0); // TODO
        // TODO: XER SO, OV update
    } else {
        rd = builder.createSub(rb, ra);
    }

    if (code.rc) {
        updateCR0(rd);
    }

    setGPR(code.rd, rd);
}

void Translator::subfcx(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rd;
    Value* ca;

    if (code.oe) {
        assert_always("Unimplemented");
        rd = builder.getConstantI64(0); // TODO
        ca = builder.getConstantI8(0); // TODO
        // TODO: XER OV update
    } else {
        rd = builder.createSub(rb, ra);
        ca = subDidCarry(builder, rb, ra);
    }

    if (code.rc) {
        updateCR0(rd);
    }

    setXER_CA(ca);
    setGPR(code.rd, rd);
}

void Translator::subfex(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* ca = getXER_CA();
    Value* rd;

    if (code.oe) {
        assert_always("Unimplemented");
        rd = builder.getConstantI64(0); // TODO
        // TODO: XER OV update
    } else {
        rd = builder.createAdd(builder.createNot(ra), rb);
        rd = builder.createAdd(rd, builder.createZExt(ca, TYPE_I64));
        ca = addWithCarryDidCarry(builder, builder.createNot(ra), rb, ca);
    }

    if (code.rc) {
        updateCR0(rd);
    }

    setXER_CA(ca);
    setGPR(code.rd, rd);
}

void Translator::subfic(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rd;
    Value* ca;

    Value* simm = builder.getConstantI64(code.simm);
    rd = builder.createSub(simm, ra);
    ca = subDidCarry(builder, simm, ra);

    setXER_CA(ca);
    setGPR(code.rd, rd);
}

void Translator::subfmex(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rd;
    Value* ca;

    if (code.oe) {
        assert_always("Unimplemented");
        rd = builder.getConstantI64(0); // TODO
        ca = builder.getConstantI8(0); // TODO
        // TODO: XER OV update
    } else {
        rd = builder.createSub(builder.createNot(ra), builder.getConstantI64(1));
        rd = builder.createAdd(rd, builder.createZExt(getXER_CA(), TYPE_I64));
        ca = addWithCarryDidCarry(builder, builder.createNot(ra), builder.getConstantI64(-1), getXER_CA());
    }

    if (code.rc) {
        updateCR0(rd);
    }

    setXER_CA(ca);
    setGPR(code.rd, rd);
}

void Translator::subfzex(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rd;
    Value* ca;

    if (code.oe) {
        assert_always("Unimplemented");
        rd = builder.getConstantI64(0); // TODO
        ca = builder.getConstantI8(0); // TODO
        // TODO: XER OV update
    } else {
        rd = builder.createNot(ra);
        rd = builder.createAdd(rd, builder.createZExt(getXER_CA(), TYPE_I64));
        ca = addWithCarryDidCarry(builder, builder.createNot(ra), builder.getConstantI64(0), getXER_CA());
    }

    if (code.rc) {
        updateCR0(rd);
    }

    setXER_CA(ca);
    setGPR(code.rd, rd);
}

void Translator::xorx(Instruction code)
{
    Value* rs = getGPR(code.rs);
    Value* rb = getGPR(code.rb);
    Value* ra;

    ra = builder.createXor(rs, rb);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Translator::xori(Instruction code)
{
    Value* constant = builder.getConstantI64(code.uimm);
    Value* rs = getGPR(code.rs);
    Value* ra;

    ra = builder.createXor(rs, constant);

    setGPR(code.ra, ra);
}

void Translator::xoris(Instruction code)
{
    Value* constant = builder.getConstantI64(code.uimm << 16);
    Value* rs = getGPR(code.rs);
    Value* ra;

    ra = builder.createXor(rs, constant);

    setGPR(code.ra, ra);
}

}  // namespace ppu
}  // namespace frontend
}  // namespace cpu
