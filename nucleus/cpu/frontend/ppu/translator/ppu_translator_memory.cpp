/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_translator.h"
#include "nucleus/assert.h"

namespace cpu {
namespace frontend {
namespace ppu {

using namespace cpu::hir;

/**
 * PPC64 Instructions:
 *  - UISA: Load and Store Instructions (Section: 4.2.3)
 *  - UISA: Memory Synchronization Instructions (Section: 4.2.6)
 *  - VEA: Memory Synchronization Instructions (Section: 4.3.2)
 */

void Translator::lbz(Instruction code)
{
    Value* result;
    Value* addr = builder.getConstantI64(code.d);
    Value* rd;

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }

    result = readMemory(addr, TYPE_I8);
    rd = builder.createZExt(result, TYPE_I64);
    setGPR(code.rd, rd);
}

void Translator::lbzu(Instruction code)
{
    Value* result;
    Value* addr = builder.getConstantI64(code.d);
    Value* rd;

    addr = builder.createAdd(addr, getGPR(code.ra));
    result = readMemory(addr, TYPE_I8);
    rd = builder.createZExt(result, TYPE_I64);

    setGPR(code.ra, addr);
    setGPR(code.rd, rd);
}

void Translator::lbzux(Instruction code)
{
    Value* result;
    Value* addr;
    Value* rd;

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    result = readMemory(addr, TYPE_I8);
    rd = builder.createZExt(result, TYPE_I64);

    setGPR(code.ra, addr);
    setGPR(code.rd, rd);
}

void Translator::lbzx(Instruction code)
{
    Value* result;
    Value* addr = getGPR(code.rb);
    Value* rd;

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }
    result = readMemory(addr, TYPE_I8);
    rd = builder.createZExt(result, TYPE_I64);

    setGPR(code.rd, rd);
}

void Translator::ld(Instruction code)
{
    Value* addr = builder.getConstantI64(code.ds << 2);
    Value* rd;

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }
    rd = readMemory(addr, TYPE_I64);

    setGPR(code.rd, rd);
}

void Translator::ldarx(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rd;

    Value* addr = rb;
    if (code.ra) {
        addr = builder.createAdd(addr, ra);
    }

    // TODO: Reservation

    rd = readMemory(addr, TYPE_I64);
    setGPR(code.rd, rd);
}

void Translator::ldbrx(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::ldu(Instruction code)
{
    Value* addr = builder.getConstantI64(code.ds << 2);
    Value* rd;

    addr = builder.createAdd(addr, getGPR(code.ra));
    rd = readMemory(addr, TYPE_I64);

    setGPR(code.ra, addr);
    setGPR(code.rd, rd);
}

void Translator::ldux(Instruction code)
{
    Value* addr;
    Value* rd;

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    rd = readMemory(addr, TYPE_I64);

    setGPR(code.ra, addr);
    setGPR(code.rd, rd);
}

void Translator::ldx(Instruction code)
{
    Value* addr = getGPR(code.rb);
    Value* rd;

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }
    rd = readMemory(addr, TYPE_I64);

    setGPR(code.rd, rd);
}

void Translator::lfd(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* frd;

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }

    frd = readMemory(addr, TYPE_F64);
    setFPR(code.frd, frd);
}

void Translator::lfdu(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* frd;

    addr = builder.createAdd(addr, getGPR(code.ra));
    frd = readMemory(addr, TYPE_F64);

    setGPR(code.ra, addr);
    setFPR(code.frd, frd);
}

void Translator::lfdux(Instruction code)
{
    Value* addr;
    Value* frd;

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    frd = readMemory(addr, TYPE_F64);

    setGPR(code.ra, addr);
    setFPR(code.frd, frd);
}

void Translator::lfdx(Instruction code)
{
    Value* addr = getGPR(code.rb);
    Value* frd;

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }
    frd = readMemory(addr, TYPE_F64);

    setFPR(code.frd, frd);
}

void Translator::lfs(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* result;

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }

    result = readMemory(addr, TYPE_F32);
    setFPR(code.frd, result);
}

void Translator::lfsu(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* result;

    addr = builder.createAdd(addr, getGPR(code.ra));
    result = readMemory(addr, TYPE_F32);

    setGPR(code.ra, addr);
    setFPR(code.frd, result);
}

void Translator::lfsux(Instruction code)
{
    Value* addr;
    Value* result;

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    result = readMemory(addr, TYPE_F32);

    setGPR(code.ra, addr);
    setFPR(code.frd, result);
}

void Translator::lfsx(Instruction code)
{
    Value* addr = getGPR(code.rb);
    Value* result;

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }
    result = readMemory(addr, TYPE_F32);

    setFPR(code.frd, result);
}

void Translator::lha(Instruction code)
{
    Value* result;
    Value* addr = builder.getConstantI64(code.d);
    Value* rd;

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }

    result = readMemory(addr, TYPE_I16);
    rd = builder.createSExt(result, TYPE_I64);
    setGPR(code.rd, rd);
}

void Translator::lhau(Instruction code)
{
    Value* result;
    Value* addr = builder.getConstantI64(code.d);
    Value* rd;

    addr = builder.createAdd(addr, getGPR(code.ra));
    result = readMemory(addr, TYPE_I16);
    rd = builder.createSExt(result, TYPE_I64);

    setGPR(code.ra, addr);
    setGPR(code.rd, rd);
}

void Translator::lhaux(Instruction code)
{
    Value* result;
    Value* addr;
    Value* rd;

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    result = readMemory(addr, TYPE_I16);
    rd = builder.createSExt(result, TYPE_I64);

    setGPR(code.ra, addr);
    setGPR(code.rd, rd);
}

void Translator::lhax(Instruction code)
{
    Value* result;
    Value* addr = getGPR(code.rb);
    Value* rd;

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }
    result = readMemory(addr, TYPE_I16);
    rd = builder.createSExt(result, TYPE_I64);

    setGPR(code.rd, rd);
}

void Translator::lhbrx(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::lhz(Instruction code)
{
    Value* result;
    Value* addr = builder.getConstantI64(code.d);
    Value* rd;

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }

    result = readMemory(addr, TYPE_I16);
    rd = builder.createZExt(result, TYPE_I64);
    setGPR(code.rd, rd);
}

void Translator::lhzu(Instruction code)
{
    Value* result;
    Value* addr = builder.getConstantI64(code.d);
    Value* rd;

    addr = builder.createAdd(addr, getGPR(code.ra));
    result = readMemory(addr, TYPE_I16);
    rd = builder.createZExt(result, TYPE_I64);

    setGPR(code.ra, addr);
    setGPR(code.rd, rd);
}

void Translator::lhzux(Instruction code)
{
    Value* result;
    Value* addr;
    Value* rd;

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    result = readMemory(addr, TYPE_I16);
    rd = builder.createZExt(result, TYPE_I64);

    setGPR(code.ra, addr);
    setGPR(code.rd, rd);
}

void Translator::lhzx(Instruction code)
{
    Value* result;
    Value* addr = getGPR(code.rb);
    Value* rd;

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }
    result = readMemory(addr, TYPE_I16);
    rd = builder.createZExt(result, TYPE_I64);

    setGPR(code.rd, rd);
}

void Translator::lmw(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::lswi(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::lswx(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::lwa(Instruction code)
{
    Value* addr = builder.getConstantI64(code.ds << 2);
    Value* rd;

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }

    auto rd_i32 = readMemory(addr, TYPE_I32);
    rd = builder.createSExt(rd_i32, TYPE_I64);
    setGPR(code.rd, rd);
}

void Translator::lwarx(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rd;

    Value* addr = rb;
    if (code.ra) {
        addr = builder.createAdd(addr, ra);
    }

    // TODO: Reservation

    rd = readMemory(addr, TYPE_I32);
    setGPR(code.rd, rd);
}

void Translator::lwaux(Instruction code)
{
    Value* addr;
    Value* rd;

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    auto rd_i32 = readMemory(addr, TYPE_I32);
    rd = builder.createSExt(rd_i32, TYPE_I64);

    setGPR(code.ra, addr);
    setGPR(code.rd, rd);
}

void Translator::lwax(Instruction code)
{
    Value* addr = getGPR(code.rb);
    Value* rd;

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }
    auto rd_i32 = readMemory(addr, TYPE_I32);
    rd = builder.createSExt(rd_i32, TYPE_I64);

    setGPR(code.rd, rd);
}

void Translator::lwbrx(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::lwz(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* rd;

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }

    auto rd_i32 = readMemory(addr, TYPE_I32);
    rd = builder.createZExt(rd_i32, TYPE_I64);
    setGPR(code.rd, rd);
}

void Translator::lwzu(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* rd;

    addr = builder.createAdd(addr, getGPR(code.ra));
    auto rd_i32 = readMemory(addr, TYPE_I32);
    rd = builder.createZExt(rd_i32, TYPE_I64);

    setGPR(code.ra, addr);
    setGPR(code.rd, rd);
}

void Translator::lwzux(Instruction code)
{
    Value* addr;
    Value* rd;

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    auto rd_i32 = readMemory(addr, TYPE_I32);
    rd = builder.createZExt(rd_i32, TYPE_I64);

    setGPR(code.ra, addr);
    setGPR(code.rd, rd);
}

void Translator::lwzx(Instruction code)
{
    Value* addr = getGPR(code.rb);
    Value* rd;

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }
    auto rd_i32 = readMemory(addr, TYPE_I32);
    rd = builder.createZExt(rd_i32, TYPE_I64);

    setGPR(code.rd, rd);
}

void Translator::stb(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* rs = getGPR(code.rs, TYPE_I8);

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }

    writeMemory(addr, rs);
}

void Translator::stbu(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* rs = getGPR(code.rs, TYPE_I8);

    addr = builder.createAdd(addr, getGPR(code.ra));
    writeMemory(addr, rs);

    setGPR(code.ra, addr);
}

void Translator::stbux(Instruction code)
{
    Value* addr;
    Value* rs = getGPR(code.rs, TYPE_I8);

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    writeMemory(addr, rs);

    setGPR(code.ra, addr);
}

void Translator::stbx(Instruction code)
{
    Value* addr = getGPR(code.rb);
    Value* rs = getGPR(code.rs, TYPE_I8);

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }
    writeMemory(addr, rs);
}

void Translator::std(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* rs = getGPR(code.rs);

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }

    writeMemory(addr, rs);
}

void Translator::stdcx_(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rs = getGPR(code.rs);

    Value* addr = rb;
    if (code.ra) {
        addr = builder.createAdd(addr, ra);
    }

    // TODO: Reservation

    // TODO: Is this correct?
    setCRField(0, builder.getConstantI8(2));

    writeMemory(addr, rs);
}

void Translator::stdu(Instruction code)
{
    Value* addr = builder.getConstantI64(code.ds << 2);
    Value* rs = getGPR(code.rs);

    addr = builder.createAdd(addr, getGPR(code.ra));
    writeMemory(addr, rs);

    setGPR(code.ra, addr);
}

void Translator::stdux(Instruction code)
{
    Value* addr;
    Value* rs = getGPR(code.rs);

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    writeMemory(addr, rs);

    setGPR(code.ra, addr);
}

void Translator::stdx(Instruction code)
{
    Value* addr = getGPR(code.rb);
    Value* rs = getGPR(code.rs);

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }
    writeMemory(addr, rs);
}

void Translator::stfd(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* frs = getFPR(code.frs);

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }

    writeMemory(addr, frs);
}

void Translator::stfdu(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* frs = getFPR(code.frs);

    addr = builder.createAdd(addr, getGPR(code.ra));
    writeMemory(addr, frs);

    setGPR(code.ra, addr);
}

void Translator::stfdux(Instruction code)
{
    Value* addr;
    Value* frs = getFPR(code.frs);

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    writeMemory(addr, frs);

    setGPR(code.ra, addr);
}

void Translator::stfdx(Instruction code)
{
    Value* addr = getGPR(code.rb);
    Value* frs = getFPR(code.frs);

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }
    writeMemory(addr, frs);
}

void Translator::stfiwx(Instruction code)
{
    Value* addr = getGPR(code.rb);
    Value* frs = getFPR(code.frs);

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }

    frs = builder.createCast(frs, TYPE_I64);
    frs = builder.createTrunc(frs, TYPE_I32);
    writeMemory(addr, frs);
}

void Translator::stfs(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* frs = getFPR(code.frs);

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }

    auto frs_f32 = builder.createConvert(frs, TYPE_F32);
    writeMemory(addr, frs_f32);
}

void Translator::stfsu(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* frs = getFPR(code.frs);

    addr = builder.createAdd(addr, getGPR(code.ra));
    auto frs_f32 = builder.createConvert(frs, TYPE_F32);
    writeMemory(addr, frs_f32);

    setGPR(code.ra, addr);
}

void Translator::stfsux(Instruction code)
{
    Value* addr;
    Value* frs = getFPR(code.frs);

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    auto frs_f32 = builder.createConvert(frs, TYPE_F32);
    writeMemory(addr, frs_f32);

    setGPR(code.ra, addr);
}

void Translator::stfsx(Instruction code)
{
    Value* addr = getGPR(code.rb);
    Value* frs = getFPR(code.frs);

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }
    auto frs_f32 = builder.createConvert(frs, TYPE_F32);
    writeMemory(addr, frs_f32);
}

void Translator::sth(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* rs = getGPR(code.rs, TYPE_I16);

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }

    writeMemory(addr, rs);
}

void Translator::sthbrx(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::sthu(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* rs = getGPR(code.rs, TYPE_I16);

    addr = builder.createAdd(addr, getGPR(code.ra));
    writeMemory(addr, rs);

    setGPR(code.ra, addr);
}

void Translator::sthux(Instruction code)
{
    Value* addr;
    Value* rs = getGPR(code.rs, TYPE_I16);

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    writeMemory(addr, rs);

    setGPR(code.ra, addr);
}

void Translator::sthx(Instruction code)
{
    Value* addr = getGPR(code.rb);
    Value* rs = getGPR(code.rs, TYPE_I16);

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }
    writeMemory(addr, rs);
}

void Translator::stmw(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::stswi(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::stswx(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::stw(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* rs = getGPR(code.rs, TYPE_I32);

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }

    writeMemory(addr, rs);
}

void Translator::stwbrx(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::stwcx_(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rs = getGPR(code.rs, TYPE_I32);

    Value* addr = rb;
    if (code.ra) {
        addr = builder.createAdd(addr, ra);
    }

    // TODO: Reservation

    // TODO: Is this correct?
    setCRField(0, builder.getConstantI8(2));

    writeMemory(addr, rs);
}

void Translator::stwu(Instruction code)
{
    Value* addr = builder.getConstantI64(code.d);
    Value* rs = getGPR(code.rs, TYPE_I32);

    addr = builder.createAdd(addr, getGPR(code.ra));
    writeMemory(addr, rs);

    setGPR(code.ra, addr);
}

void Translator::stwux(Instruction code)
{
    Value* addr;
    Value* rs = getGPR(code.rs, TYPE_I32);

    addr = builder.createAdd(getGPR(code.ra), getGPR(code.rb));
    writeMemory(addr, rs);

    setGPR(code.ra, addr);
}

void Translator::stwx(Instruction code)
{
    Value* addr = getGPR(code.rb);
    Value* rs = getGPR(code.rs, TYPE_I32);

    if (code.ra) {
        addr = builder.createAdd(addr, getGPR(code.ra));
    }
    writeMemory(addr, rs);
}

void Translator::eieio(Instruction code)
{
    builder.createMemFence();
}

void Translator::sync(Instruction code)
{
    builder.createMemFence();
}

void Translator::isync(Instruction code)
{
    // TODO
}

}  // namespace ppu
}  // namespace frontend
}  // namespace cpu
