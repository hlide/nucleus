/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_prx.h"
#include "nucleus/emulator.h"
#include "nucleus/syscalls/callback.h"
#include "nucleus/syscalls/lv2.h"
#include "nucleus/syscalls/lv2/sys_process.h"
#include "nucleus/loader/self.h"

s32 sys_prx_load_module(s8* path, u64 flags, sys_prx_load_module_option_t* pOpt)
{
    SELFLoader self;
    if (!self.open(path)) {
        return CELL_PRX_ERROR_UNKNOWN_MODULE;
    }

    // Load PRX into memory
    auto* prx = new sys_prx_t();
    if (!self.load_prx(prx)) {
        delete prx;
        return CELL_PRX_ERROR_ILLEGAL_LIBRARY;
    }

    // Obtain module start/stop/exit functions (set to 0 if not present)
    auto& metaLib = prx->exported_libs[0];
    prx->func_start = metaLib.exports[0xBC9A0086];
    prx->func_stop = metaLib.exports[0xAB779874];
    prx->func_exit = metaLib.exports[0x3AB9A95E];
    prx->path = path;

    const s32 id = nucleus.lv2.objects.add(prx, SYS_PRX_OBJECT);
    return id;
}

s32 sys_prx_load_module_list(s32 count, be_t<u32>* pathList, u64 flags, void* pOpt, be_t<u32>* idList)
{
    for (s32 i = 0; i < count; i++) {
        auto* path = (s8*)((u64)nucleus.memory.getBaseAddr() + pathList[i]);

        const s32 ret = sys_prx_load_module(path, flags, nullptr);
        if (ret <= 0) {
            return ret;
        }

        idList[i] = ret;
    }

    return CELL_OK;
}

s32 sys_prx_start_module(s32 id, u64 flags, sys_prx_start_module_option_t* pOpt)
{
    auto* prx = nucleus.lv2.objects.get<sys_prx_t>(id);
    const sys_prx_param_t& prx_param = nucleus.lv2.prx_param;

    // Update ELF import table
    u32 offset = prx_param.libstubstart;
    while (offset < prx_param.libstubend) {
        const auto& importedLibrary = nucleus.memory.ref<sys_prx_library_info_t>(offset);
        offset += importedLibrary.size;

        for (const auto& lib : prx->exported_libs) {
            if (lib.name != nucleus.memory.ptr<s8>(importedLibrary.name_addr)) {
                continue;
            }
            for (u32 i = 0; i < importedLibrary.num_func; i++) {
                const u32 fnid = nucleus.memory.read32(importedLibrary.fnid_addr + 4*i);
                nucleus.memory.write32(importedLibrary.fstub_addr + 4*i, lib.exports.at(fnid));
            }
        }
    }

    if (prx->func_start) {
        pOpt->entry = prx->func_start;
    } else {
        pOpt->entry = 0xFFFFFFFFFFFFFFFFULL;
    }

    return CELL_OK;
}

s32 sys_prx_1CE()
{
    return CELL_OK;
}

s32 sys_prx_get_module_list()
{
    return CELL_OK;
}

s32 sys_prx_register_library(u32 lib_addr)
{
    return CELL_OK;
}

s32 sys_prx_register_module()
{
    return CELL_OK;
}
