#pragma once

#include <CL/cl.h>
#include <CL/opencl.h>

namespace ezg
{
    //Error
    /////////////////////////////////////////////////////////////////////////////////
    class Error : public std::exception
    {
        cl_int m_errCode;
        const char* m_msg;

    public:

        Error(cl_int code, const char* msg) noexcept
            : m_errCode(code)
            , m_msg    (msg)
        {}

        const char* what() const noexcept override
        {
            return (m_msg == nullptr) ? "empty" : m_msg;
        }

        cl_int code() const noexcept { return m_errCode; }

    public:

        static cl_int handler(cl_int code, const char* msg = nullptr)
        {
            if (code != CL_SUCCESS) {
                throw Error(code, msg);
            }
            return code;
        }
    };//class Error
    /////////////////////////////////////////////////////////////////////////////////


    //ReferenceHandler
    /////////////////////////////////////////////////////////////////////////////////
    template< typename T >
    struct ReferenceHandler { /*empty*/ };

    template< >
    struct ReferenceHandler< cl_device_id >
    {
        static cl_int retain (cl_device_id device) { return clRetainDevice (device); }
        static cl_int release(cl_device_id device) { return clReleaseDevice(device); }
    };

    template <>
    struct ReferenceHandler< cl_context >
    {
        static cl_int retain (cl_context context) { return clRetainContext (context); }
        static cl_int release(cl_context context) { return clReleaseContext(context); }
    };

    template <>
    struct ReferenceHandler< cl_command_queue >
    {
        static cl_int retain (cl_command_queue queue) { return clRetainCommandQueue (queue); }
        static cl_int release(cl_command_queue queue) { return clReleaseCommandQueue(queue); }
    };

    template <>
    struct ReferenceHandler< cl_mem >
    {
        static cl_int retain (cl_mem memory) { return clRetainMemObject (memory); }
        static cl_int release(cl_mem memory) { return clReleaseMemObject(memory); }
    };

    template <>
    struct ReferenceHandler< cl_program >
    {
        static cl_int retain (cl_program program) { return clRetainProgram (program); }
        static cl_int release(cl_program program) { return clReleaseProgram(program); }
    };

    template <>
    struct ReferenceHandler< cl_kernel >
    {
        static cl_int retain (cl_kernel kernel) { return clRetainKernel (kernel); }
        static cl_int release(cl_kernel kernel) { return clReleaseKernel(kernel); }
    };

    template <>
    struct ReferenceHandler< cl_event >
    {
        static cl_int retain (cl_event event) { return clRetainEvent (event); }
        static cl_int release(cl_event event) { return clReleaseEvent(event); }
    };
    /////////////////////////////////////////////////////////////////////////////////


    //Wrapper
    /////////////////////////////////////////////////////////////////////////////////
    template< typename T >
    class Wrapper
    {
    public:
        using cl_type = T;

    protected:
        cl_type m_object;

    public:

        Wrapper()
            : m_object(nullptr)
        {}

        Wrapper(const cl_type& obj, bool NeedRetain = false)
            : m_object(obj)
        {
            if(NeedRetain) {
                Error::handler(retain(), "retain error");
            }
        }

        Wrapper(const Wrapper< cl_type >& that)
            : m_object(that.m_object)
        {
            Error::handler(retain(), "retain error");
        }

        Wrapper(Wrapper< cl_type >&& that) noexcept
        {
            std::swap(m_object, that.m_object);
        }

        Wrapper< cl_type >& operator = (const Wrapper< cl_type >& that)
        {
            Error::handler(that.retain(), "retain error");
            Error::handler(release(), "release error");
            m_object = that.m_object;
        }

        ~Wrapper()
        {
            if (m_object != nullptr) { release(); }
        }


        cl_type get() const { return m_object; }

    protected:

        cl_int retain() const
        {
            if (m_object == nullptr) {
                return CL_SUCCESS;
            }
            return ReferenceHandler< cl_type >::retain(m_object);
        }

        cl_int release() const
        {
            if (m_object == nullptr) {
                return CL_SUCCESS;
            }
            return ReferenceHandler< cl_type >::release(m_object);
        }
    };
    /////////////////////////////////////////////////////////////////////////////////


    //Device
    /////////////////////////////////////////////////////////////////////////////////
/*    class Device : public Wrapper< cl_device_id >
    {

    };*/
    /////////////////////////////////////////////////////////////////////////////////
}//namespace ezg