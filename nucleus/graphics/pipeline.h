/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/graphics/format.h"
#include "nucleus/graphics/primitive.h"
#include "nucleus/graphics/shader.h"
#include "nucleus/graphics/texture.h"

namespace gfx {

// Input Assembler
enum InputClassification {
    INPUT_CLASSIFICATION_PER_VERTEX,
    INPUT_CLASSIFICATION_PER_INSTANCE,
};

struct InputElement {
    U32 semanticIndex;
    Format format;
    U32 inputSlot;
    U32 offset;
    U32 stride;
    U32 size;
    InputClassification inputClassification;
    U32 instanceStepRate;
};

struct IAState {
    PrimitiveTopology topology;
    std::vector<InputElement> inputLayout;
};

// Tesselator
struct TessState {
};

// Rasterizer
enum FillMode {
    FILL_MODE_SOLID,      // Fill triangles formed by vertices
    FILL_MODE_WIREFRAME,  // Draw lines connecting vertices
};

enum CullMode {
    CULL_MODE_NONE,   // Draw all triangles
    CULL_MODE_FRONT,  // Draw front-facing triangles
    CULL_MODE_BACK,   // Draw back-facing triangles
};

enum ComparisonFunc {
    COMPARISON_FUNC_NEVER          = 1,
    COMPARISON_FUNC_LESS           = 2,
    COMPARISON_FUNC_EQUAL          = 3,
    COMPARISON_FUNC_LESS_EQUAL     = 4,
    COMPARISON_FUNC_GREATER        = 5,
    COMPARISON_FUNC_NOT_EQUAL      = 6,
    COMPARISON_FUNC_GREATER_EQUAL  = 7,
    COMPARISON_FUNC_ALWAYS         = 8,
};

enum DepthWriteMask {
    DEPTH_WRITE_MASK_ZERO  = 0,
    DEPTH_WRITE_MASK_ALL   = 1
};

enum StencilOp {
    STENCIL_OP_KEEP,
    STENCIL_OP_ZERO,
    STENCIL_OP_REPLACE,
    STENCIL_OP_INCR_SAT,
    STENCIL_OP_DECR_SAT,
    STENCIL_OP_INVERT,
    STENCIL_OP_INCR,
    STENCIL_OP_DECR,
};

struct StencilOpDesc {
    StencilOp stencilOpFail;
    StencilOp stencilOpZFail;
    StencilOp stencilOpPass;
    ComparisonFunc stencilFunc;
};

struct RSState {
    FillMode fillMode;
    CullMode cullMode;
    bool frontFaceClockwise;

    // Depth-Stencil state
    bool depthEnable;
    DepthWriteMask depthWriteMask;
    ComparisonFunc depthFunc;
    bool stencilEnable;
    U08 stencilReadMask;
    U08 stencilWriteMask;
    StencilOpDesc frontFace;
    StencilOpDesc backFace;
};

// Color Blender
enum Blend {
    BLEND_ZERO,
    BLEND_ONE,
    BLEND_SRC_COLOR,
    BLEND_INV_SRC_COLOR,
    BLEND_SRC_ALPHA,
    BLEND_INV_SRC_ALPHA,
    BLEND_DEST_ALPHA,
    BLEND_INV_DEST_ALPHA,
    BLEND_DEST_COLOR,
    BLEND_INV_DEST_COLOR,
    BLEND_SRC_ALPHA_SAT,
    BLEND_BLEND_FACTOR,
    BLEND_INV_BLEND_FACTOR,
    BLEND_SRC1_COLOR,
    BLEND_INV_SRC1_COLOR,
    BLEND_SRC1_ALPHA,
    BLEND_INV_SRC1_ALPHA
};

enum BlendOp {
    BLEND_OP_ADD,
    BLEND_OP_SUBTRACT,
    BLEND_OP_REV_SUBTRACT,
    BLEND_OP_MIN,
    BLEND_OP_MAX
};

enum LogicOp {
    LOGIC_OP_COPY,
    LOGIC_OP_CLEAR,
    LOGIC_OP_AND,
    LOGIC_OP_AND_REVERSE,
    LOGIC_OP_AND_INVERTED,
    LOGIC_OP_NOOP,
    LOGIC_OP_XOR,
    LOGIC_OP_OR,
    LOGIC_OP_NOR,
    LOGIC_OP_EQUIV,
    LOGIC_OP_INVERT,
    LOGIC_OP_OR_REVERSE,
    LOGIC_OP_COPY_INVERTED,
    LOGIC_OP_OR_INVERTED,
    LOGIC_OP_NAND,
    LOGIC_OP_SET,
};

using ColorWriteMask = U08;
enum : ColorWriteMask {
    COLOR_WRITE_ENABLE_RED    = (1 << 0),
    COLOR_WRITE_ENABLE_GREEN  = (1 << 1),
    COLOR_WRITE_ENABLE_BLUE   = (1 << 2),
    COLOR_WRITE_ENABLE_ALPHA  = (1 << 3),
    COLOR_WRITE_ENABLE_ALL    = COLOR_WRITE_ENABLE_RED | COLOR_WRITE_ENABLE_GREEN | COLOR_WRITE_ENABLE_BLUE | COLOR_WRITE_ENABLE_ALPHA,
};

struct CBState {
    bool enableAlphaToCoverage;
    bool enableIndependentBlend;
    struct ColorTargetBlendDesc {
        bool enableBlend;
        bool enableLogicOp;
        Blend srcBlend;
        Blend destBlend;
        BlendOp blendOp;
        Blend srcBlendAlpha;
        Blend destBlendAlpha;
        BlendOp blendOpAlpha;
        LogicOp logicOp;
        ColorWriteMask colorWriteMask;
    } colorTarget[8];
};

struct DBState {
};

/**
 * Pipeline Description
 * ====================
 * Consists of:
 * +----+    +====+    +====+    +====+    +====+    +----+    +====+    +----+----+
 * | IA | -> | VS | -> | HS | -> | DS | -> | GS | -> | RS | -> | PS | -> | CB | DB |
 * +----+    +====+    +====+    +====+    +====+    +----+    +====+    +----+----+
 *                    \________________/
 *                           Tess                            ---- Fixed stage
 *                                                           ==== Programmable stage
 */
class PipelineDesc {
public:
    Shader* vs;  // Vertex Shader
    Shader* hs;  // Hull Shader
    Shader* ds;  // Domain Shader
    Shader* gs;  // Geometry Shader
    Shader* ps;  // Pixel Shader

    IAState iaState;      // Input assembler static pipeline state
    TessState tessState;  // Tessellator static pipeline state
    RSState rsState;      // Rasterizer static pipeline state
    CBState cbState;      // Color blender and output static pipeline state
    DBState dbState;      // Depth-stencil static pipeline state

    std::vector<Sampler> samplers;
    Format formatDSV;
    Size numCBVs;
    Size numSRVs;
};

class Pipeline {
public:
};

}  // namespace gfx
