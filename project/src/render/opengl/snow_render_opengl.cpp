/*
    Copyright Sven Bergström, Hugh Sanderson and other contributors 2013, 2014
    Portions adapted from https://github.com/haxenme/nme/
    adapted and cleaned up for snow https://github.com/underscorediscovery/snow
    MIT license
*/



#if defined(HX_WINDOWS) || defined(HX_MACOS) || defined(HX_LINUX)
// Include neko glue....
#define NEKO_COMPATIBLE
#endif


#include <hx/CFFI.h>

#ifdef ANDROID
#include <android/log.h>
#endif

#include "common/snow_hx.h"
#include "snow_hx_bindings.h"
#include "snow_core.h"

#include "render/opengl/snow_opengl.h"

#include <string>

namespace snow {

        void snow_gl_set_context_attributes(
            bool alpha, bool depth, bool stencil,
            bool antialias, bool premultipliedAlpha, bool preserveDrawingBuffer);

    namespace render {


        //called from window code after making
        //a window, to set the valid context state

        void set_context_attributes(
            int red_bits, int green_bits, int blue_bits, int alpha_bits,
            int depth_bits, int stencil_bits, int antialiasing) {

            snow::snow_gl_set_context_attributes(
                alpha_bits != 0,
                depth_bits != 0,
                stencil_bits != 0,
                antialiasing != 0,
                false, false
            );

        } //set_context_attributes

    }

    #define INT(a) val_int(arg[a])


// --- General -------------------------------------------


    value snow_gl_get_error() {

        return alloc_int( glGetError() );

    } DEFINE_PRIM(snow_gl_get_error,0);


    value snow_gl_finish() {

        glFinish();

        return alloc_null();

    } DEFINE_PRIM(snow_gl_finish,0);


    value snow_gl_flush() {

        glFlush();

        return alloc_null();

    } DEFINE_PRIM(snow_gl_flush,0);


    value snow_gl_version() {

        const char* gl_ver = (const char*)glGetString(GL_VERSION);
        const char* gl_sl  = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
        const char* gl_ren = (const char*)glGetString(GL_RENDERER);
        const char* gl_ven = (const char*)glGetString(GL_VENDOR);

        std::string glver   ( gl_ver ? gl_ver   : "GL version null" );
        std::string glslver ( gl_sl ? gl_sl     : "GL SL version null" );
        std::string glren   ( gl_ren ? gl_ren   : "GL renderer version null" );
        std::string glven   ( gl_ven ? gl_ven   : "GL vendor version null" );

        std::string res = "/ " + glver + " / " + glslver + " / " + glren + " / " + glven + " /";

        return alloc_string( res.c_str() );

    } DEFINE_PRIM(snow_gl_version,0);


    value snow_gl_enable(value inCap) {

        glEnable(val_int(inCap));

        return alloc_null();

    } DEFINE_PRIM(snow_gl_enable,1);


    value snow_gl_disable(value inCap) {

        glDisable(val_int(inCap));

        return alloc_null();

    } DEFINE_PRIM(snow_gl_disable,1);


    value snow_gl_hint(value inTarget, value inValue) {

        glHint(val_int(inTarget),val_int(inValue));

        return alloc_null();

    } DEFINE_PRIM(snow_gl_hint,2);


    value snow_gl_line_width(value inWidth) {

        glLineWidth(val_number(inWidth));

        return alloc_null();

    } DEFINE_PRIM(snow_gl_line_width,1);

    struct gl_context_attributes {
        gl_context_attributes()
            : alpha(false), depth(false), stencil(false), antialias(false),
              premultipliedAlpha(false), preserveDrawingBuffer(false) {}

        bool alpha;
        bool depth;
        bool stencil;
        bool antialias;
        bool premultipliedAlpha;
        bool preserveDrawingBuffer;

    };

    static gl_context_attributes attr;
    static value gl_context_val;

    void snow_gl_set_context_attributes(
        bool alpha, bool depth, bool stencil,
        bool antialias, bool premultipliedAlpha, bool preserveDrawingBuffer) {

            attr.alpha = alpha;
            attr.depth = depth;
            attr.stencil = stencil;
            attr.antialias = antialias;
            attr.premultipliedAlpha = premultipliedAlpha;
            attr.preserveDrawingBuffer = preserveDrawingBuffer;

        gl_context_val = alloc_null(); //kill existing for gc
        gl_context_val = alloc_empty_object();

           alloc_field(gl_context_val,val_id("alpha"),alloc_bool(alpha));
           alloc_field(gl_context_val,val_id("depth"),alloc_bool(depth));
           alloc_field(gl_context_val,val_id("stencil"),alloc_bool(stencil));
           alloc_field(gl_context_val,val_id("antialias"),alloc_bool(antialias));
           alloc_field(gl_context_val,val_id("premultipliedAlpha"),alloc_bool(premultipliedAlpha));
           alloc_field(gl_context_val,val_id("preserveDrawingBuffer"),alloc_bool(preserveDrawingBuffer));

    } //snow_gl_set_context_attributes

    value snow_gl_get_context_attributes() {

        return gl_context_val;

    } DEFINE_PRIM(snow_gl_get_context_attributes,0);


    value snow_gl_get_supported_extensions(value ioList) {

        const char *ext = (const char *)glGetString(GL_EXTENSIONS);

        if (ext && *ext) {

            while(true) {

                const char *next = ext;

                while(*next && *next!=' ') {
                    next++;
                }

                val_array_push( ioList, alloc_string_len(ext, next-ext) );

                if (!*next || !next[1]) {
                    break;
                }

                ext = next+1;

            } //while true

        } //if ext and *ext

        return alloc_null();

    } DEFINE_PRIM(snow_gl_get_supported_extensions,1);


    value snow_gl_front_face(value inFace) {

        glFrontFace(val_int(inFace));

        return alloc_null();

    } DEFINE_PRIM(snow_gl_front_face,1);


    value snow_gl_get_parameter(value pname_val) {

        int floats = 0;
        int ints = 0;
        int strings = 0;
        int pname = val_int(pname_val);

        switch(pname) {

            case GL_ALIASED_LINE_WIDTH_RANGE:
            case GL_ALIASED_POINT_SIZE_RANGE:
            case GL_DEPTH_RANGE:
                floats = 2;
            break;

            case GL_BLEND_COLOR:
            case GL_COLOR_CLEAR_VALUE:
                floats = 4;
            break;

            case GL_COLOR_WRITEMASK:
                ints = 4;
            break;

            //case GL_COMPRESSED_TEXTURE_FORMATS  null

            case GL_MAX_VIEWPORT_DIMS:
                ints = 2;
            break;

            case GL_SCISSOR_BOX:
            case GL_VIEWPORT:
                ints = 4;
            break;

            case GL_ARRAY_BUFFER_BINDING:
            case GL_CURRENT_PROGRAM:
            case GL_ELEMENT_ARRAY_BUFFER_BINDING:
            case GL_FRAMEBUFFER_BINDING:
            case GL_RENDERBUFFER_BINDING:
            case GL_TEXTURE_BINDING_2D:
            case GL_TEXTURE_BINDING_CUBE_MAP:
            case GL_DEPTH_CLEAR_VALUE:
            case GL_LINE_WIDTH:
            case GL_POLYGON_OFFSET_FACTOR:
            case GL_POLYGON_OFFSET_UNITS:
            case GL_SAMPLE_COVERAGE_VALUE:
                ints = 1;
            break;

            case GL_BLEND:
            case GL_DEPTH_WRITEMASK:
            case GL_DITHER:
            case GL_CULL_FACE:
            case GL_POLYGON_OFFSET_FILL:
            case GL_SAMPLE_COVERAGE_INVERT:
            case GL_STENCIL_TEST:
          //case GL_UNPACK_FLIP_Y_WEBGL:
          //case GL_UNPACK_PREMULTIPLY_ALPHA_WEBGL:
                ints = 1;
            break;

            case GL_ALPHA_BITS:
            case GL_ACTIVE_TEXTURE:
            case GL_BLEND_DST_ALPHA:
            case GL_BLEND_DST_RGB:
            case GL_BLEND_EQUATION_ALPHA:
            case GL_BLEND_EQUATION_RGB:
            case GL_BLEND_SRC_ALPHA:
            case GL_BLEND_SRC_RGB:
            case GL_BLUE_BITS:
            case GL_CULL_FACE_MODE:
            case GL_DEPTH_BITS:
            case GL_DEPTH_FUNC:
            case GL_DEPTH_TEST:
            case GL_FRONT_FACE:
            case GL_GENERATE_MIPMAP_HINT:
            case GL_GREEN_BITS:
            case GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS:
            case GL_MAX_CUBE_MAP_TEXTURE_SIZE:
          //case GL_MAX_FRAGMENT_UNIFORM_VECTORS:
          //case GL_MAX_RENDERBUFFER_SIZE:
            case GL_MAX_TEXTURE_IMAGE_UNITS:
            case GL_MAX_TEXTURE_SIZE:
          //case GL_MAX_VARYING_VECTORS:
            case GL_MAX_VERTEX_ATTRIBS:
            case GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS:
            case GL_MAX_VERTEX_UNIFORM_VECTORS:
            case GL_NUM_COMPRESSED_TEXTURE_FORMATS:
            case GL_PACK_ALIGNMENT:
            case GL_RED_BITS:
            case GL_SAMPLE_BUFFERS:
            case GL_SAMPLES:
            case GL_SCISSOR_TEST:
            case GL_SHADING_LANGUAGE_VERSION:
            case GL_STENCIL_BACK_FAIL:
            case GL_STENCIL_BACK_FUNC:
            case GL_STENCIL_BACK_PASS_DEPTH_FAIL:
            case GL_STENCIL_BACK_PASS_DEPTH_PASS:
            case GL_STENCIL_BACK_REF:
            case GL_STENCIL_BACK_VALUE_MASK:
            case GL_STENCIL_BACK_WRITEMASK:
            case GL_STENCIL_BITS:
            case GL_STENCIL_CLEAR_VALUE:
            case GL_STENCIL_FAIL:
            case GL_STENCIL_FUNC:
            case GL_STENCIL_PASS_DEPTH_FAIL:
            case GL_STENCIL_PASS_DEPTH_PASS:
            case GL_STENCIL_REF:
            case GL_STENCIL_VALUE_MASK:
            case GL_STENCIL_WRITEMASK:
            case GL_SUBPIXEL_BITS:
            case GL_UNPACK_ALIGNMENT:
          //case GL_UNPACK_COLORSPACE_CONVERSION_WEBGL:
                ints = 1;
            break;

            case GL_VENDOR:
            case GL_VERSION:
            case GL_RENDERER:
                strings = 1;
            break;

        } //switch pname

        if (ints==1) {

            int val;
            glGetIntegerv(pname,&val);

            return alloc_int(val);

        } else if (strings==1) {

            return alloc_string((const char *)glGetString(pname));

        } else if (floats==1) {

            float f;
            glGetFloatv(pname,&f);

            return alloc_float(f);

        } else if (ints>0) {

            int vals[4];
            glGetIntegerv(pname,vals);
            value  result = alloc_array(ints);

            for(int i=0;i<ints;i++) {
                val_array_set_i(result,i,alloc_int(vals[i]));
            }

            return result;

        } else if (floats>0) {

            float vals[4];
            glGetFloatv(pname,vals);
            value  result = alloc_array(ints);

            for(int i=0;i<ints;i++) {
                val_array_set_i(result,i,alloc_int(vals[i]));
            }

            return result;

        }

        return alloc_null();

    } DEFINE_PRIM(snow_gl_get_parameter,1);


// --- Is -------------------------------------------

#ifdef NATIVE_TOOLKIT_GLEW
    #define HAS_EXT_framebuffer_object GLEW_EXT_framebuffer_object
#else
    #define HAS_EXT_framebuffer_object true
#endif

    #define GL_IS(name,Name) \
        value snow_gl_is_##name(value val) { return alloc_bool(glIs##Name(val_int(val))); } \
        DEFINE_PRIM(snow_gl_is_##name,1);

    GL_IS(buffer,Buffer)
    GL_IS(enabled,Enabled)
    GL_IS(program,Program)
    GL_IS(shader,Shader)
    GL_IS(texture,Texture)

        //since these are plausibly unsupported and need checks, they are done below
    // GL_IS(framebuffer,Framebuffer)
    // GL_IS(renderbuffer,Renderbuffer)

    value snow_gl_is_framebuffer(value val) {

        if (HAS_EXT_framebuffer_object) {
            return alloc_bool(glIsFramebuffer(val_int(val)));
        } else {
            snow::log(1, "snow / framebuffer object extension not found. / IsFramebuffer");
        }

        return alloc_bool(false);

    } DEFINE_PRIM(snow_gl_is_framebuffer,1);


    value snow_gl_is_renderbuffer(value val) {

        if (HAS_EXT_framebuffer_object) {
            return alloc_bool( glIsRenderbuffer(val_int(val)) );
        } else {
            snow::log(1, "snow / framebuffer object extension not found. / IsRenderbuffer");
        }

        return alloc_bool(false);

    } DEFINE_PRIM(snow_gl_is_renderbuffer,1);


// --- Stencil -------------------------------------------


    value snow_gl_stencil_func(value func, value ref, value mask) {

        glStencilFunc(val_int(func),val_int(ref),val_int(mask));

        return alloc_null();

    } DEFINE_PRIM(snow_gl_stencil_func,3);


    value snow_gl_stencil_func_separate(value face,value func, value ref, value mask) {

        glStencilFuncSeparate(val_int(face),val_int(func),val_int(ref),val_int(mask));

        return alloc_null();

    } DEFINE_PRIM(snow_gl_stencil_func_separate,4);


    value snow_gl_stencil_mask(value mask) {

        glStencilMask(val_int(mask));

        return alloc_null();

    } DEFINE_PRIM(snow_gl_stencil_mask,1);


    value snow_gl_stencil_mask_separate(value face,value mask) {

        glStencilMaskSeparate(val_int(face),val_int(mask));

        return alloc_null();

    } DEFINE_PRIM(snow_gl_stencil_mask_separate,2);


    value snow_gl_stencil_op(value fail,value zfail, value zpass) {

        glStencilOp(val_int(fail),val_int(zfail),val_int(zpass));

        return alloc_null();

    } DEFINE_PRIM(snow_gl_stencil_op,3);


    value snow_gl_stencil_op_separate(value face,value fail,value zfail, value zpass) {

        glStencilOpSeparate(val_int(face),val_int(fail),val_int(zfail),val_int(zpass));

        return alloc_null();

    } DEFINE_PRIM(snow_gl_stencil_op_separate,4);


// --- Blend -------------------------------------------


    value snow_gl_blend_color(value r, value g, value b, value a) {

        glBlendColor(val_number(r),val_number(g),val_number(b), val_number(a));

        return alloc_null();

    } DEFINE_PRIM(snow_gl_blend_color,4);


    value snow_gl_blend_equation(value mode) {

        glBlendEquation(val_int(mode));

        return alloc_null();

    } DEFINE_PRIM(snow_gl_blend_equation,1);


    value snow_gl_blend_equation_separate(value rgb, value a) {

        glBlendEquationSeparate(val_int(rgb), val_int(a));

        return alloc_null();

    } DEFINE_PRIM(snow_gl_blend_equation_separate,2);


    value snow_gl_blend_func(value s, value d) {

        glBlendFunc(val_int(s), val_int(d));

        return alloc_null();

    } DEFINE_PRIM(snow_gl_blend_func,2);


    value snow_gl_blend_func_separate(value srgb, value drgb, value sa, value da) {

        glBlendFuncSeparate(val_int(srgb), val_int(drgb), val_int(sa), val_int(da) );

        return alloc_null();

    } DEFINE_PRIM(snow_gl_blend_func_separate,4);


// --- Program -------------------------------------------


    value snow_gl_create_program() {

        int result = glCreateProgram();

        return alloc_int(result);

    } DEFINE_PRIM(snow_gl_create_program,0);


    value snow_gl_link_program(value inId) {

        int id = val_int(inId);

        glLinkProgram(id);

        return alloc_null();

    } DEFINE_PRIM(snow_gl_link_program,1);


    value snow_gl_validate_program(value inId) {

        int id = val_int(inId);

        glValidateProgram(id);

        return alloc_null();

    } DEFINE_PRIM(snow_gl_validate_program,1);


    value snow_gl_get_program_info_log(value inId) {

        char buf[1024];
        int id = val_int(inId);

        glGetProgramInfoLog(id,1024,0,buf);

        return alloc_string(buf);

    } DEFINE_PRIM(snow_gl_get_program_info_log,1);


    value snow_gl_delete_program(value inId) {

        int id = val_int(inId);

        glDeleteProgram(id);

        return alloc_null();

    } DEFINE_PRIM(snow_gl_delete_program,1);


    value snow_gl_bind_attrib_location(value inId,value inSlot,value inName) {

        int id = val_int(inId);

        glBindAttribLocation(id,val_int(inSlot),val_string(inName));

        return alloc_null();

    } DEFINE_PRIM(snow_gl_bind_attrib_location,3);


    value snow_gl_get_attrib_location(value inId,value inName) {

        int id = val_int(inId);

        return alloc_int(glGetAttribLocation(id,val_string(inName)));

    } DEFINE_PRIM(snow_gl_get_attrib_location,2);


    value snow_gl_get_uniform_location(value inId,value inName) {

        int id = val_int(inId);
        int location = glGetUniformLocation(id, val_string(inName));

        return location < 0 ? alloc_null() : alloc_int(location);

    } DEFINE_PRIM(snow_gl_get_uniform_location,2);


    value snow_gl_get_uniform(value inId,value inLocation) {

        int id = val_int(inId);
        int loc = val_int(inLocation);

        char buf[1];
        GLsizei outLen = 1;
        GLsizei size = 0;
        GLenum  type = 0;

        glGetActiveUniform(id, loc, 1, &outLen, &size, &type, buf);
        int ints = 0;
        int floats = 0;
        int bools = 0;

        switch(type) {

            case  GL_FLOAT: {
                float result = 0;
                glGetUniformfv(id,loc,&result);

                return alloc_float(result);
            }

            case  GL_FLOAT_VEC2: floats=2;
            case  GL_FLOAT_VEC3: floats++;
            case  GL_FLOAT_VEC4: floats++;
                break;

            case  GL_INT_VEC2: ints=2;
            case  GL_INT_VEC3: ints++;
            case  GL_INT_VEC4: ints++;
                break;

            case  GL_BOOL_VEC2: bools = 2;
            case  GL_BOOL_VEC3: bools++;
            case  GL_BOOL_VEC4: bools++;
                break;

            case  GL_FLOAT_MAT2: floats = 4; break;
            case  GL_FLOAT_MAT3: floats = 9; break;
            case  GL_FLOAT_MAT4: floats = 16; break;

            #ifdef HX_MACOS
                case  GL_FLOAT_MAT2x3: floats = 4*3; break;
                case  GL_FLOAT_MAT2x4: floats = 4*4; break;
                case  GL_FLOAT_MAT3x2: floats = 9*2; break;
                case  GL_FLOAT_MAT3x4: floats = 9*4; break;
                case  GL_FLOAT_MAT4x2: floats = 16*2; break;
                case  GL_FLOAT_MAT4x3: floats = 16*3; break;
            #endif

            case  GL_INT:
            case  GL_BOOL:
            case  GL_SAMPLER_2D:

            #ifdef HX_MACOS
                case  GL_SAMPLER_1D:
                case  GL_SAMPLER_3D:
                case  GL_SAMPLER_CUBE:
                case  GL_SAMPLER_1D_SHADOW:
                case  GL_SAMPLER_2D_SHADOW:
            #endif
            {
                int result = 0;
                glGetUniformiv(id,loc,&result);

                return alloc_int(result);
            }
        } //type

        if (ints + bools > 0) {

            int buffer[4];
            glGetUniformiv(id,loc,buffer);

            value result = alloc_array( ints+bools );

            for(int i=0;i<ints+bools;i++) {
                val_array_set_i(result,i,alloc_int(buffer[i]));
            }

            return result;
        }

        if (floats>0) {

            float buffer[16*3];
            glGetUniformfv(id,loc,buffer);

            value result = alloc_array( floats );

            for(int i=0;i<floats;i++) {
                val_array_set_i(result,i,alloc_float(buffer[i]));
            }

            return result;
        }

        return alloc_null();

    } DEFINE_PRIM(snow_gl_get_uniform,2);


    value snow_gl_get_program_parameter(value inId,value inName) {

        int id = val_int(inId);
        int result = 0;

        glGetProgramiv(id, val_int(inName), &result);

        return alloc_int(result);

    } DEFINE_PRIM(snow_gl_get_program_parameter,2);


    value snow_gl_use_program(value inId) {

        int id = val_int(inId);

        glUseProgram(id);

        return alloc_null();

    } DEFINE_PRIM(snow_gl_use_program,1);


    value snow_gl_get_active_attrib(value inProg, value inIndex) {

        int id = val_int(inProg);
        value result = alloc_empty_object( );

        char buf[1024];
        GLsizei outLen = 1024;
        GLsizei size = 0;
        GLenum  type = 0;

        glGetActiveAttrib(id, val_int(inIndex), 1024, &outLen, &size, &type, buf);

        alloc_field(result,val_id("size"),alloc_int(size));
        alloc_field(result,val_id("type"),alloc_int(type));
        alloc_field(result,val_id("name"),alloc_string(buf));

        return result;

    } DEFINE_PRIM(snow_gl_get_active_attrib,2);


    value snow_gl_get_active_uniform(value inProg, value inIndex) {

        int id = val_int(inProg);

        char buf[1024];
        GLsizei outLen = 1024;
        GLsizei size = 0;
        GLenum  type = 0;

        glGetActiveUniform(id, val_int(inIndex), 1024, &outLen, &size, &type, buf);

        value result = alloc_empty_object( );
        alloc_field(result,val_id("size"),alloc_int(size));
        alloc_field(result,val_id("type"),alloc_int(type));
        alloc_field(result,val_id("name"),alloc_string(buf));

        return result;

    } DEFINE_PRIM(snow_gl_get_active_uniform,2);



        //only float arrays accepted
        //passe the full arrays instead of single element
        //if double should be accepted, it is highly important that whole arrays are passed
        // value inLocation, value inTranspose, value inBytes, value inByteLength, value inCount){
    value snow_gl_uniform_matrix(value *arg, int argCount) {

        enum { aLocation, aTranspose, aBytes, aByteOffset, aByteLength, aCount };

        int location = val_int(arg[aLocation]);
        int count = val_int(arg[aCount]);
        int byteLength = val_int(arg[aByteLength]);
        int byteOffset = val_int(arg[aByteOffset]);
        bool transpose = val_bool(arg[aTranspose]);

        const GLfloat* data = (GLfloat*)snow::bytes_from_hx(arg[aBytes]);
        int nbElems = byteLength / sizeof(float);

        switch(count){
            case 2: glUniformMatrix2fv(location, nbElems >> 2 , transpose, data + byteOffset );
            case 3: glUniformMatrix3fv(location, nbElems / 9  , transpose, data + byteOffset );
            case 4: glUniformMatrix4fv(location, nbElems >> 4 , transpose, data + byteOffset );
        }

        return alloc_null();
    } DEFINE_PRIM_MULT(snow_gl_uniform_matrix);


    #define GL_UNFORM_1(TYPE,GET) \
    value snow_gl_uniform1##TYPE(value inLocation, value inV0) \
    { \
       glUniform1##TYPE(val_int(inLocation),GET(inV0)); \
       return alloc_null(); \
    } DEFINE_PRIM(snow_gl_uniform1##TYPE,2);

    #define GL_UNFORM_2(TYPE,GET) \
    value snow_gl_uniform2##TYPE(value inLocation, value inV0,value inV1) \
    { \
       glUniform2##TYPE(val_int(inLocation),GET(inV0),GET(inV1)); \
       return alloc_null(); \
    } DEFINE_PRIM(snow_gl_uniform2##TYPE,3);

    #define GL_UNFORM_3(TYPE,GET) \
    value snow_gl_uniform3##TYPE(value inLocation, value inV0,value inV1,value inV2) \
    { \
       glUniform3##TYPE(val_int(inLocation),GET(inV0),GET(inV1),GET(inV2)); \
       return alloc_null(); \
    } DEFINE_PRIM(snow_gl_uniform3##TYPE,4);

    #define GL_UNFORM_4(TYPE,GET) \
    value snow_gl_uniform4##TYPE(value inLocation, value inV0,value inV1,value inV2,value inV3) \
    { \
       glUniform4##TYPE(val_int(inLocation),GET(inV0),GET(inV1),GET(inV2),GET(inV3)); \
       return alloc_null(); \
    } DEFINE_PRIM(snow_gl_uniform4##TYPE,5);

    GL_UNFORM_1(i,val_int)
    GL_UNFORM_1(f,val_number)
    GL_UNFORM_2(i,val_int)
    GL_UNFORM_2(f,val_number)
    GL_UNFORM_3(i,val_int)
    GL_UNFORM_3(f,val_number)
    GL_UNFORM_4(i,val_int)
    GL_UNFORM_4(f,val_number)

    value snow_gl_uniform1iv(value inLocation, value inBytes, value inByteOffset, value inByteLength) {

        int location = val_int(inLocation);
        int byteOffset = val_int(inByteOffset);
        int byteLength = val_int(inByteLength);

        const GLint* data = (GLint*)snow::bytes_from_hx(inBytes);
        int nbElems = byteLength / sizeof(int);

        glUniform1iv(location, nbElems, data + byteOffset);

        return alloc_null();

    } DEFINE_PRIM(snow_gl_uniform1iv,4);


    value snow_gl_uniform2iv(value inLocation, value inBytes, value inByteOffset, value inByteLength) {

        int location = val_int(inLocation);
        int byteOffset = val_int(inByteOffset);
        int byteLength = val_int(inByteLength);

        const GLint* data = (GLint*)snow::bytes_from_hx(inBytes);
        int nbElems = byteLength / sizeof(int);

        glUniform2iv(location, nbElems>>1, data + byteOffset);

        return alloc_null();

    } DEFINE_PRIM(snow_gl_uniform2iv,4);


    value snow_gl_uniform3iv(value inLocation, value inBytes, value inByteOffset, value inByteLength) {

        int location = val_int(inLocation);
        int byteOffset = val_int(inByteOffset);
        int byteLength = val_int(inByteLength);

        const GLint* data = (GLint*)snow::bytes_from_hx(inBytes);
        int nbElems = byteLength / sizeof(int);

        glUniform3iv(location, nbElems/3, data + byteOffset);

        return alloc_null();

    } DEFINE_PRIM(snow_gl_uniform3iv,4);


    value snow_gl_uniform4iv(value inLocation, value inBytes, value inByteOffset, value inByteLength) {

        int location = val_int(inLocation);
        int byteOffset = val_int(inByteOffset);
        int byteLength = val_int(inByteLength);

        const GLint* data = (GLint*)snow::bytes_from_hx(inBytes);
        int nbElems = byteLength / sizeof(int);

        glUniform4iv(location, nbElems>>2, data + byteOffset);

        return alloc_null();

    } DEFINE_PRIM(snow_gl_uniform4iv,4);


    value snow_gl_uniform1fv(value inLocation, value inBytes, value inByteOffset, value inByteLength) {

        int location = val_int(inLocation);
        int byteOffset = val_int(inByteOffset);
        int byteLength = val_int(inByteLength);

        const GLfloat* data = (GLfloat*)snow::bytes_from_hx(inBytes);
        int nbElems = byteLength / sizeof(float);

        glUniform1fv(location, nbElems, data + byteOffset);

        return alloc_null();

    } DEFINE_PRIM(snow_gl_uniform1fv,4);


    value snow_gl_uniform2fv(value inLocation, value inBytes, value inByteOffset, value inByteLength) {

        int location = val_int(inLocation);
        int byteOffset = val_int(inByteOffset);
        int byteLength = val_int(inByteLength);

        const GLfloat* data = (GLfloat*)snow::bytes_from_hx(inBytes);
        int nbElems = byteLength / sizeof(float);

        glUniform2fv(location, nbElems>>1, data + byteOffset);

        return alloc_null();

    } DEFINE_PRIM(snow_gl_uniform2fv,4);


    value snow_gl_uniform3fv(value inLocation, value inBytes, value inByteOffset, value inByteLength) {

        int location = val_int(inLocation);
        int byteOffset = val_int(inByteOffset);
        int byteLength = val_int(inByteLength);

        const GLfloat* data = (GLfloat*)snow::bytes_from_hx(inBytes);
        int nbElems = byteLength / sizeof(float);

        glUniform3fv(location, nbElems/3, data + byteOffset);

        return alloc_null();

    } DEFINE_PRIM(snow_gl_uniform3fv,4);


    value snow_gl_uniform4fv(value inLocation, value inBytes, value inByteOffset, value inByteLength) {

        int location = val_int(inLocation);
        int byteOffset = val_int(inByteOffset);
        int byteLength = val_int(inByteLength);

        const GLfloat* data = (GLfloat*)snow::bytes_from_hx(inBytes);
        int nbElems = byteLength / sizeof(float);

        glUniform4fv(location, nbElems>>2, data + byteOffset);

        return alloc_null();

    } DEFINE_PRIM(snow_gl_uniform4fv,4);

    // Attrib

    value snow_gl_vertex_attrib1f(value inLocation, value inV0) {

        glVertexAttrib1f(val_int(inLocation),val_number(inV0));

        return alloc_null();

    } DEFINE_PRIM(snow_gl_vertex_attrib1f,2);


    value snow_gl_vertex_attrib2f(value inLocation, value inV0,value inV1) {

        glVertexAttrib2f(val_int(inLocation),val_number(inV0),val_number(inV1));

        return alloc_null();

    } DEFINE_PRIM(snow_gl_vertex_attrib2f,3);


    value snow_gl_vertex_attrib3f(value inLocation, value inV0,value inV1,value inV2) {

        glVertexAttrib3f(val_int(inLocation),val_number(inV0),val_number(inV1),val_number(inV2));

        return alloc_null();

    } DEFINE_PRIM(snow_gl_vertex_attrib3f,4);


    value snow_gl_vertex_attrib4f(value inLocation, value inV0,value inV1,value inV2, value inV3) {

        glVertexAttrib4f(val_int(inLocation),val_number(inV0),val_number(inV1),val_number(inV2),val_number(inV3));

        return alloc_null();

    } DEFINE_PRIM(snow_gl_vertex_attrib4f,5);


    value snow_gl_vertex_attrib1fv(value inLocation, value inBytes, value inByteOffset, value inByteLength) {

        int location = val_int(inLocation);
        int byteOffset = val_int(inByteOffset);
        int byteLength = val_int(inByteLength);

        const GLfloat* data = (GLfloat*)snow::bytes_from_hx(inBytes);

        glVertexAttrib1fv(location, data + byteOffset);

        return alloc_null();

    } DEFINE_PRIM(snow_gl_vertex_attrib1fv,4);


    value snow_gl_vertex_attrib2fv(value inLocation, value inBytes, value inByteOffset, value inByteLength) {

        int location = val_int(inLocation);
        int byteOffset = val_int(inByteOffset);
        int byteLength = val_int(inByteLength);

        const GLfloat* data = (GLfloat*)snow::bytes_from_hx(inBytes);

        glVertexAttrib2fv(location, data + byteOffset);

        return alloc_null();

    } DEFINE_PRIM(snow_gl_vertex_attrib2fv,4);


    value snow_gl_vertex_attrib3fv(value inLocation, value inBytes, value inByteOffset, value inByteLength) {

        int location = val_int(inLocation);
        int byteOffset = val_int(inByteOffset);
        int byteLength = val_int(inByteLength);

        const GLfloat* data = (GLfloat*)snow::bytes_from_hx(inBytes);

        glVertexAttrib3fv(location, data + byteOffset);

        return alloc_null();

    } DEFINE_PRIM(snow_gl_vertex_attrib3fv,4);


    value snow_gl_vertex_attrib4fv(value inLocation, value inBytes, value inByteOffset, value inByteLength) {

        int location = val_int(inLocation);
        int byteOffset = val_int(inByteOffset);
        int byteLength = val_int(inByteLength);

        const GLfloat* data = (GLfloat*)snow::bytes_from_hx(inBytes);

        glVertexAttrib4fv(location, data + byteOffset);

        return alloc_null();

    } DEFINE_PRIM(snow_gl_vertex_attrib4fv,4);


// --- Shader -------------------------------------------


    value snow_gl_create_shader(value inType) {

        return alloc_int(glCreateShader(val_int(inType)));

    } DEFINE_PRIM(snow_gl_create_shader,1);


    value snow_gl_delete_shader(value inId) {

        int id = val_int(inId);

        glDeleteShader(id);

        return alloc_null();

    } DEFINE_PRIM(snow_gl_delete_shader,1);


    value snow_gl_shader_source(value inId,value inSource) {

        int id = val_int(inId);
        const char *source = val_string(inSource);

        glShaderSource(id,1,&source,0);

        return alloc_null();

    } DEFINE_PRIM(snow_gl_shader_source,2);


    value snow_gl_attach_shader(value inProg,value inShader) {

        glAttachShader(val_int(inProg),val_int(inShader));

        return alloc_null();

    } DEFINE_PRIM(snow_gl_attach_shader,2);


    value snow_gl_detach_shader(value inProg,value inShader) {

        glDetachShader(val_int(inProg),val_int(inShader));

        return alloc_null();

    } DEFINE_PRIM(snow_gl_detach_shader,2);


    value snow_gl_compile_shader(value inId) {

        int id = val_int(inId);

        glCompileShader(id);

        return alloc_null();

    } DEFINE_PRIM(snow_gl_compile_shader,1);


    value snow_gl_get_shader_parameter(value inId,value inName) {

        int id = val_int(inId);
        int result = 0;

        glGetShaderiv(id,val_int(inName), & result);

        return alloc_int(result);

    } DEFINE_PRIM(snow_gl_get_shader_parameter,2);


    value snow_gl_get_shader_info_log(value inId) {

        int id = val_int(inId);
        char buf[1024] = "";

        glGetShaderInfoLog(id,1024,0,buf);

        return alloc_string(buf);

    } DEFINE_PRIM(snow_gl_get_shader_info_log,1);


    value snow_gl_get_shader_source(value inId) {

        int id = val_int(inId);

        int len = 0;
        glGetShaderiv(id,GL_SHADER_SOURCE_LENGTH,&len);

        if (len==0) {
            return alloc_null();
        }

        char *buf = new char[len+1];
        glGetShaderSource(id,len+1,0,buf);
        value result = alloc_string(buf);

        delete [] buf;

        return result;

    } DEFINE_PRIM(snow_gl_get_shader_source,1);


    value snow_gl_get_shader_precision_format(value inShader,value inPrec) {

        #ifdef snow_GLES

            int range[2];
            int precision;

            glGetShaderPrecisionFormat(val_int(inShader), val_int(inPrec), range, &precision);

            value result = alloc_empty_object();

                alloc_field(result,val_id("rangeMin"),alloc_int(range[0]));
                alloc_field(result,val_id("rangeMax"),alloc_int(range[1]));
                alloc_field(result,val_id("precision"),alloc_int(precision));

            return result;

        #else

            return alloc_null();

        #endif

    } DEFINE_PRIM(snow_gl_get_shader_precision_format,2);


// --- Buffer -------------------------------------------


    value snow_gl_create_buffer() {

        GLuint buffers;

        glGenBuffers(1,&buffers);

        return alloc_int(buffers);

    } DEFINE_PRIM(snow_gl_create_buffer,0);


    value snow_gl_delete_buffer(value inId) {

        GLuint id = val_int(inId);

        glDeleteBuffers(1,&id);

        return alloc_null();

    } DEFINE_PRIM(snow_gl_delete_buffer,1);


    value snow_gl_bind_buffer(value inTarget, value inId ) {

        glBindBuffer(val_int(inTarget),val_int(inId));

        return alloc_null();

    } DEFINE_PRIM(snow_gl_bind_buffer,2);


    value snow_gl_buffer_data(value inTarget, value inBuffer, value inByteOffset, value inByteLength, value inUsage) {

        int byteLength = val_int(inByteLength);
        int byteOffset = val_int(inByteOffset);

        const unsigned char* data = snow::bytes_from_hx(inBuffer);

        glBufferData( val_int(inTarget), byteLength, data + byteOffset, val_int(inUsage) );

        return alloc_null();

    } DEFINE_PRIM(snow_gl_buffer_data,5);


    value snow_gl_buffer_sub_data(value inTarget, value inOffset, value inBuffer, value inByteOffset, value inByteLength) {

        int byteLength = val_int(inByteLength);
        int byteOffset = val_int(inByteOffset);

        const unsigned char* data = snow::bytes_from_hx(inBuffer);

        glBufferSubData(val_int(inTarget), val_int(inOffset), byteLength, data + byteOffset );

        return alloc_null();

    } DEFINE_PRIM(snow_gl_buffer_sub_data,5);


    value snow_gl_get_vertex_attrib_offset(value index, value name) {

        int result = 0;

        glGetVertexAttribPointerv(val_int(index), val_int(name), (void **)&result);

        return alloc_int(result);

    } DEFINE_PRIM(snow_gl_get_vertex_attrib_offset,2);


    value snow_gl_get_vertex_attrib(value index, value name) {

        int result = 0;

        glGetVertexAttribiv(val_int(index), val_int(name), &result);

        return alloc_int(result);

    } DEFINE_PRIM(snow_gl_get_vertex_attrib,2);


    value snow_gl_vertex_attrib_pointer(value *arg, int nargs) {

        enum { aIndex, aSize, aType, aNormalized, aStride, aOffset, aSIZE };

        glVertexAttribPointer( val_int(arg[aIndex]),
                              val_int(arg[aSize]),
                              val_int(arg[aType]),
                              val_bool(arg[aNormalized]),
                              val_int(arg[aStride]),
                              (void *)(intptr_t)val_int(arg[aOffset]) );

        return alloc_null();

    } DEFINE_PRIM_MULT(snow_gl_vertex_attrib_pointer);


    value snow_gl_enable_vertex_attrib_array(value inIndex) {

        glEnableVertexAttribArray(val_int(inIndex));

        return alloc_null();

    } DEFINE_PRIM(snow_gl_enable_vertex_attrib_array,1);


    value snow_gl_disable_vertex_attrib_array(value inIndex) {

        glDisableVertexAttribArray(val_int(inIndex));

        return alloc_null();

    } DEFINE_PRIM(snow_gl_disable_vertex_attrib_array,1);


    value snow_gl_get_buffer_paramerter(value inTarget, value inPname) {

        int result = 0;

        glGetBufferParameteriv(val_int(inTarget), val_int(inPname),&result);

        return alloc_int(result);

    } DEFINE_PRIM(snow_gl_get_buffer_paramerter,2);


    value snow_gl_get_buffer_parameter(value inTarget, value inIndex) {

        GLint data = 0;

        glGetBufferParameteriv(val_int(inTarget), val_int(inIndex), &data);

        return alloc_int(data);

    } DEFINE_PRIM(snow_gl_get_buffer_parameter,2);


// --- Framebuffer -------------------------------


    value snow_gl_bind_framebuffer(value target, value framebuffer) {

        if (HAS_EXT_framebuffer_object) {
            glBindFramebuffer(val_int(target), val_int(framebuffer) );
        } else {
            snow::log(1, "snow / framebuffer object extension not found. / BindFramebuffer");
        }

       return alloc_null();

    } DEFINE_PRIM(snow_gl_bind_framebuffer,2);


    value snow_gl_bind_renderbuffer(value target, value renderbuffer) {

        if( HAS_EXT_framebuffer_object ) {
            glBindRenderbuffer(val_int(target),val_int(renderbuffer));
        } else {
            snow::log(1, "snow / framebuffer object extension not found. / BindRenderbuffer");
        }

       return alloc_null();

    } DEFINE_PRIM(snow_gl_bind_renderbuffer,2);


    value snow_gl_create_framebuffer() {

        GLuint id = 0;

        if( HAS_EXT_framebuffer_object ) {
            glGenFramebuffers( 1, &id );
        }

        return alloc_int(id);

    } DEFINE_PRIM(snow_gl_create_framebuffer,0);


    value snow_gl_delete_framebuffer(value target) {

        GLuint id = val_int(target);

        if (HAS_EXT_framebuffer_object) {
            glDeleteFramebuffers(1, &id);
        } else {
            snow::log(1, "snow / framebuffer object extension not found. / DeleteFramebuffers");
        }

        return alloc_null();

    } DEFINE_PRIM(snow_gl_delete_framebuffer,1);


    value snow_gl_create_render_buffer( ) {

        GLuint id = 0;

        if( HAS_EXT_framebuffer_object ) {
            glGenRenderbuffers(1,&id);
        } else {
            snow::log(1, "snow / framebuffer object extension not found. / GenRenderbuffers");
        }

        return alloc_int(id);

    } DEFINE_PRIM(snow_gl_create_render_buffer,0);


    value snow_gl_delete_render_buffer(value target) {

        GLuint id = val_int(target);

        if( HAS_EXT_framebuffer_object ) {
            glDeleteRenderbuffers(1, &id);
        } else {
            snow::log(1, "snow / framebuffer object extension not found. / DeleteRenderbuffers");
        }

        return alloc_null();

    } DEFINE_PRIM(snow_gl_delete_render_buffer,1);


    value snow_gl_framebuffer_renderbuffer(value target, value attachment, value renderbuffertarget, value renderbuffer) {

        if( HAS_EXT_framebuffer_object ) {
            glFramebufferRenderbuffer(val_int(target), val_int(attachment), val_int(renderbuffertarget), val_int(renderbuffer) );
        } else {
            snow::log(1, "snow / framebuffer object extension not found. / FramebufferRenderbuffer");
        }

        return alloc_null();

    } DEFINE_PRIM(snow_gl_framebuffer_renderbuffer,4);


    value snow_gl_framebuffer_texture2D(value target, value attachment, value textarget, value texture, value level) {

        if( HAS_EXT_framebuffer_object ) {
            glFramebufferTexture2D( val_int(target), val_int(attachment), val_int(textarget), val_int(texture), val_int(level) );
        } else {
            snow::log(1, "snow / framebuffer object extension not found. / FramebufferTexture2D");
        }

        return alloc_null();

    } DEFINE_PRIM(snow_gl_framebuffer_texture2D,5);


    value snow_gl_renderbuffer_storage(value target, value internalFormat, value width, value height) {

        if( HAS_EXT_framebuffer_object ) {
            glRenderbufferStorage( val_int(target), val_int(internalFormat), val_int(width), val_int(height) );
        } else {
            snow::log(1, "snow / framebuffer object extension not found. / RenderbufferStorage");
        }

        return alloc_null();

    } DEFINE_PRIM(snow_gl_renderbuffer_storage,4);


    value snow_gl_check_framebuffer_status(value inTarget) {

        if( HAS_EXT_framebuffer_object ) {
            return alloc_int( glCheckFramebufferStatus(val_int(inTarget)) );
        } else {
            snow::log(1, "snow / framebuffer object extension not found. / CheckFramebufferStatus");
        }

        return alloc_int(0);

    } DEFINE_PRIM(snow_gl_check_framebuffer_status,1);


    value snow_gl_get_framebuffer_attachment_parameter(value target, value attachment, value pname) {

        GLint result = 0;

        if( HAS_EXT_framebuffer_object ) {
            glGetFramebufferAttachmentParameteriv( val_int(target), val_int(attachment), val_int(pname), &result);
        } else {
            snow::log(1, "snow / framebuffer object extension not found. / GetFramebufferAttachmentParameteriv");
        }

       return alloc_int(result);

    } DEFINE_PRIM(snow_gl_get_framebuffer_attachment_parameter,3);


    value snow_gl_get_render_buffer_parameter(value target, value pname) {

        int result = 0;

        if( HAS_EXT_framebuffer_object ) {
            glGetRenderbufferParameteriv(val_int(target), val_int(pname), &result);
        } else {
            snow::log(1, "snow / framebuffer object extension not found. / GetRenderbufferParameteriv");
        }

        return alloc_int(result);

    } DEFINE_PRIM(snow_gl_get_render_buffer_parameter,2);


// --- Drawing -------------------------------


    value snow_gl_draw_arrays(value inMode, value inFirst, value inCount) {

        glDrawArrays( val_int(inMode), val_int(inFirst), val_int(inCount) );

        return alloc_null();

    } DEFINE_PRIM(snow_gl_draw_arrays,3);


    value snow_gl_draw_elements(value inMode, value inCount, value inType, value inOffset) {

        glDrawElements( val_int(inMode), val_int(inCount), val_int(inType), (void *)(intptr_t)val_int(inOffset) );

        return alloc_null();

    } DEFINE_PRIM(snow_gl_draw_elements,4);



// --- Viewports -------------------------------



    value snow_gl_viewport(value inX, value inY, value inW,value inH) {

        glViewport(val_int(inX),val_int(inY),val_int(inW),val_int(inH));

        return alloc_null();

    } DEFINE_PRIM(snow_gl_viewport,4);


    value snow_gl_scissor(value inX, value inY, value inW,value inH) {

        glScissor(val_int(inX),val_int(inY),val_int(inW),val_int(inH));

        return alloc_null();

    } DEFINE_PRIM(snow_gl_scissor,4);


    value snow_gl_clear(value inMask) {

        glClear(val_int(inMask));

        return alloc_null();

    } DEFINE_PRIM(snow_gl_clear,1);


    value snow_gl_clear_color(value r,value g, value b, value a) {

        glClearColor(val_number(r),val_number(g),val_number(b),val_number(a));

        return alloc_null();

    } DEFINE_PRIM(snow_gl_clear_color,4);


    value snow_gl_clear_depth(value depth) {

        #ifdef SNOW_GLES
            glClearDepthf(val_number(depth));
        #else
            glClearDepth(val_number(depth));
        #endif

        return alloc_null();

    } DEFINE_PRIM(snow_gl_clear_depth,1);


    value snow_gl_clear_stencil(value stencil) {

        glClearStencil(val_int(stencil));

        return alloc_null();

    } DEFINE_PRIM(snow_gl_clear_stencil,1);


    value snow_gl_color_mask(value r,value g, value b, value a) {

        glColorMask(val_bool(r),val_bool(g),val_bool(b),val_bool(a));

        return alloc_null();

    } DEFINE_PRIM(snow_gl_color_mask,4);


    value snow_gl_depth_func(value func) {

        glDepthFunc(val_int(func));

        return alloc_null();

    } DEFINE_PRIM(snow_gl_depth_func,1);


    value snow_gl_depth_mask(value mask) {

        glDepthMask(val_bool(mask));

        return alloc_null();

    } DEFINE_PRIM(snow_gl_depth_mask,1);


    value snow_gl_depth_range(value inNear, value inFar) {

        #ifdef SNOW_GLES
            glDepthRangef(val_number(inNear), val_number(inFar));
        #else
            glDepthRange(val_number(inNear), val_number(inFar));
        #endif

        return alloc_null();

    } DEFINE_PRIM(snow_gl_depth_range,2);


    value snow_gl_cull_face(value mode) {

        glCullFace(val_int(mode));

        return alloc_null();

    } DEFINE_PRIM(snow_gl_cull_face,1);


    value snow_gl_polygon_offset(value factor, value units) {

        glPolygonOffset(val_number(factor), val_number(units));

        return alloc_null();

    } DEFINE_PRIM(snow_gl_polygon_offset,2);


    value snow_gl_read_pixels(value *arg, int argCount) {

        enum { aX, aY, aWidth, aHeight, aFormat, aType, aBytes, aByteOffset, aByteLength };

        int byteOffset = val_int(arg[aByteOffset]);
        int byteLength = val_int(arg[aByteLength]);
        unsigned char* data = snow::bytes_from_hx_rw(arg[aBytes]);

        glReadPixels( val_int(arg[aX]),
                      val_int(arg[aY]),
                      val_int(arg[aWidth]),
                      val_int(arg[aHeight]),
                      val_int(arg[aFormat]),
                      val_int(arg[aType]),
                      data + byteOffset
                    );

        return alloc_null();

    } DEFINE_PRIM_MULT(snow_gl_read_pixels);


    value snow_gl_pixel_storei(value pname, value param) {

       glPixelStorei(val_int(pname), val_int(param));

       return alloc_null();

    } DEFINE_PRIM(snow_gl_pixel_storei,2);


    value snow_gl_sample_coverage(value f, value invert) {

       glSampleCoverage(val_number(f), val_bool(invert));

       return alloc_null();

    } DEFINE_PRIM(snow_gl_sample_coverage,2);



// --- Texture -------------------------------------------



    value snow_gl_create_texture() {

       unsigned int id = 0;
       glGenTextures(1,&id);

       return alloc_int(id);

    } DEFINE_PRIM(snow_gl_create_texture,0);


    value snow_gl_active_texture(value inSlot) {

        glActiveTexture( val_int(inSlot) );

        return alloc_null();

    } DEFINE_PRIM(snow_gl_active_texture,1);


    value snow_gl_delete_texture(value inId) {

        GLuint id = val_int(inId);
        glDeleteTextures(1,&id);

        return alloc_null();

    } DEFINE_PRIM(snow_gl_delete_texture,1);


    value snow_gl_bind_texture(value inTarget, value inTexture) {

        glBindTexture(val_int(inTarget), val_int(inTexture) );

        return alloc_null();

    } DEFINE_PRIM(snow_gl_bind_texture,2);


    value snow_gl_tex_image_2d(value *arg, int argCount) {

        enum { aTarget, aLevel, aInternal, aWidth, aHeight, aBorder, aFormat, aType, aBytes, aByteOffset, aByteLength };

        const unsigned char* data = snow::bytes_from_hx(arg[aBytes]);
        int byteOffset = val_int( arg[aByteOffset] );
        int byteLength = val_int( arg[aByteLength] );

        glTexImage2D( val_int(arg[aTarget]),
                      val_int(arg[aLevel]),
                      val_int(arg[aInternal]),
                      val_int(arg[aWidth]),
                      val_int(arg[aHeight]),
                      val_int(arg[aBorder]),
                      val_int(arg[aFormat]),
                      val_int(arg[aType]),
                      data + byteOffset
                    );

        return alloc_null();

    } DEFINE_PRIM_MULT(snow_gl_tex_image_2d);


    value snow_gl_tex_sub_image_2d(value *arg, int argCount) {

        enum { aTarget, aLevel, aXOffset, aYOffset, aWidth, aHeight, aFormat, aType, aBytes, aByteOffset, aByteLength };

        const unsigned char* data = snow::bytes_from_hx(arg[aBytes]);
        int byteOffset = val_int( arg[aByteOffset] );
        int byteLength = val_int( arg[aByteLength] );

        glTexSubImage2D( val_int(arg[aTarget]),
                         val_int(arg[aLevel]),
                         val_int(arg[aXOffset]),
                         val_int(arg[aYOffset]),
                         val_int(arg[aWidth]),
                         val_int(arg[aHeight]),
                         val_int(arg[aFormat]),
                         val_int(arg[aType]),
                         data + byteOffset );

        return alloc_null();

    } DEFINE_PRIM_MULT(snow_gl_tex_sub_image_2d);


    value snow_gl_compressed_tex_image_2d(value *arg, int argCount) {

        enum { aTarget, aLevel, aInternal, aWidth, aHeight, aBorder, aBytes, aByteOffset, aByteLength };

        const unsigned char* data = snow::bytes_from_hx(arg[aBytes]);
        int byteOffset = val_int( arg[aByteOffset] );
        int byteLength = val_int( arg[aByteLength] );

        glCompressedTexImage2D( val_int(arg[aTarget]),
                                val_int(arg[aLevel]),
                                val_int(arg[aInternal]),
                                val_int(arg[aWidth]),
                                val_int(arg[aHeight]),
                                val_int(arg[aBorder]),
                                byteLength,
                                data + byteOffset
                              );

       return alloc_null();

    } DEFINE_PRIM_MULT(snow_gl_compressed_tex_image_2d);


    value snow_gl_compressed_tex_sub_image_2d(value *arg, int argCount) {

        enum { aTarget, aLevel, aXOffset, aYOffset, aWidth, aHeight, aFormat, aBytes, aByteOffset, aByteLength };

        const unsigned char* data = snow::bytes_from_hx(arg[aBytes]);
        int byteOffset = val_int( arg[aByteOffset] );
        int byteLength = val_int( arg[aByteLength] );

        glCompressedTexSubImage2D( val_int(arg[aTarget]),
                                   val_int(arg[aLevel]),
                                   val_int(arg[aXOffset]),
                                   val_int(arg[aYOffset]),
                                   val_int(arg[aWidth]),
                                   val_int(arg[aHeight]),
                                   val_int(arg[aFormat]),
                                   byteLength,
                                   data + byteOffset
                                );

        return alloc_null();

    } DEFINE_PRIM_MULT(snow_gl_compressed_tex_sub_image_2d);


    value snow_gl_tex_parameterf(value inTarget, value inPName, value inVal) {

        glTexParameterf(val_int(inTarget), val_int(inPName), val_number(inVal) );

        return alloc_null();

    } DEFINE_PRIM(snow_gl_tex_parameterf,3);


    value snow_gl_tex_parameteri(value inTarget, value inPName, value inVal) {

        glTexParameterf(val_int(inTarget), val_int(inPName), val_int(inVal) );

        return alloc_null();

    } DEFINE_PRIM(snow_gl_tex_parameteri,3);


    value snow_gl_copy_tex_image_2d(value *arg, int argCount) {

        enum { aTarget, aLevel, aInternalFormat, aX, aY, aWidth, aHeight, aBorder };

        glCopyTexImage2D( val_int(arg[aTarget]),
                          val_int(arg[aLevel]),
                          val_int(arg[aInternalFormat]),
                          val_int(arg[aX]),
                          val_int(arg[aY]),
                          val_int(arg[aWidth]),
                          val_int(arg[aHeight]),
                          val_int(arg[aBorder])
                        );

        return alloc_null();

    } DEFINE_PRIM_MULT(snow_gl_copy_tex_image_2d);


    value snow_gl_copy_tex_sub_image_2d(value *arg, int argCount) {

        enum { aTarget, aLevel, aXOffset, aYOffset, aX, aY, aWidth, aHeight };

        glCopyTexSubImage2D( val_int(arg[aTarget]),
                             val_int(arg[aLevel]),
                             val_int(arg[aXOffset]),
                             val_int(arg[aYOffset]),
                             val_int(arg[aX]),
                             val_int(arg[aY]),
                             val_int(arg[aWidth]),
                             val_int(arg[aHeight])
                            );

        return alloc_null();

    } DEFINE_PRIM_MULT(snow_gl_copy_tex_sub_image_2d);


    value snow_gl_generate_mipmap(value inTarget) {

        glGenerateMipmap(val_int(inTarget));

        return alloc_null();

    } DEFINE_PRIM(snow_gl_generate_mipmap,1);


    value snow_gl_get_tex_parameter(value inTarget,value inPname) {

        int result = 0;
        glGetTexParameteriv(val_int(inTarget), val_int(inPname), &result);

        return alloc_int(result);

    } DEFINE_PRIM(snow_gl_get_tex_parameter,2);


} //snow namespace


extern "C" int snow_opengl_register_prims() {
    return 0;
}
