/**
 * @file performance_event_buffer.h
 * @author Krisna Pranav
 * @brief performance eventing buffer
 * @version 6.0
 * @date 2023-09-01
 * 
 * @copyright Copyright (c) 2021 - 2023 pranaOS Developers, Krisna Pranav
 * 
 */

#pragma once 

#include <kernel/kbuffer.h>
#include <kernel/kresult.h>

namespace Kernel
{

    struct [[gnu::packed]] MallocPerformanceEvent
    {
        size_t size;
        FlatPtr ptr;
    }; // MallocPerformanceEvent 

    struct [[gnu::packed]] FreePerformanceEvent
    {
        size_t size;
        FlatPtr ptr;
    }; // FreePerformanceEvent 

    class PerformanceEventBuffer
    {
    public:
        PerformanceEventBuffer();

        /**
         * @param type 
         * @param arg 
         * @param arg2 
         * @return KResult 
         */
        KResult append(int type, FlatPtr arg, FlatPtr arg2);

        /**
         * @return size_t 
         */
        size_t capacity() const
        {
            return m_buffer.size();
        }

        /**
         * @return size_t 
         */
        size_t count() const
        {
            return m_count;
        }

        /**
         * @param index 
         * @return const PerformanceEvent& 
         */
        const PerformanceEvent& at(size_t index) const 
        {
            return const_cast<PerformanceEventBuffer&>(*this).at(index);
        }

        /**
         * @param executable_path 
         * @return KBuffer 
         */
        KBuffer to_json(ProcessID, const String& executable_path) const;

    private:
        PerformanceEventBuffer& at(size_t index);

        size_t m_count { 0 };
        
        KBuffer m_buffer;

    }; // class PerformanceEventBuffer

} // namespace Kernel