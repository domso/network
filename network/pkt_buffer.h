#ifndef pkt_buffer_h
#define pkt_buffer_h

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <stdint.h>
#include "network/ip_addr.h"

namespace network {
/**
* @brief Simple buffer for packet-IO
*
*/
class pkt_buffer {
public:
    /**
    * @brief initialize the buffer with a capacity of 'bufferSize'
    *
    * @param bufferCapacity p_bufferCapacity: init buffer-capacity
    */
    pkt_buffer(int bufferCapacity) : m_msgBuffer(bufferCapacity), m_msgLength(0), m_objCastIndex(0) {

    }
    
    /**
    * @brief
    * - gets the next valid pointer to n casted CAST_TYPE-objects
    * - a valid object was either inserted by usering pushNext() or directly modifying the buffer with     data() and setMsgLen()
    *
    * @param n p_n: number of CAST_TYPE-instances to cast
    * @return CAST_TYPE*: pointer to an instance of CAST_TYPE or nullptr
    */
    template <typename CAST_TYPE>
    CAST_TYPE* get_next(const int n = 1) {
        CAST_TYPE* result = nullptr;

        // if the next object is still inside the valid-buffer
        if (m_objCastIndex + sizeof(CAST_TYPE)*n <= m_msgLength) {
            // create pointer to simulated instance
            result = (CAST_TYPE*)(m_msgBuffer.data() + m_objCastIndex);
            // increase offset by the size of the simulated instance
            m_objCastIndex += sizeof(CAST_TYPE) * n;

        }

        return result;
    }
    
    /**
    * @brief
    * - reserves the memory for n CAST_TYPE-objects in the buffer
    * - every object is NOT a new instance, but a reference to the internal buffer
    * - the lifetime last until the next resize()-call
    * - any other modification with the internal data (-> data()-call) will have side-effects
    *
    * @param n p_n: number of CAST_TYPE-instances to cast
    * @return CAST_TYPE* pointer to an instance of CAST_TYPE or nullptr
    */
    
    template <typename CAST_TYPE>
    CAST_TYPE* push_next(const int n = 1) {
        CAST_TYPE* result = nullptr;

        // if the next object is still inside the buffer
        if (m_objCastIndex + sizeof(CAST_TYPE)*n <= m_msgBuffer.size()) {
            // create pointer to simulated instance
            result = (CAST_TYPE*)(m_msgBuffer.data() + m_objCastIndex);
            // increase offset by the size of the simulated instance
            m_objCastIndex += sizeof(CAST_TYPE) * n;

            if (m_msgLength < m_objCastIndex) {
                m_msgLength = m_objCastIndex;
            }

            memset(result, 0, sizeof(CAST_TYPE)*n);
        }

        return result;
    }
    
    /**
    * @brief pushes a std::string in the buffer
    *
    * @param input p_input: input data
    * @return bool: success
    */
    bool push_string(const std::string& input) {
        char* dest = push_next<char>(input.length());

        if (dest != nullptr) {
            std::strncpy(dest, input.c_str(),  input.length());
            return true;
        }

        return false;
    }
    
    /**
    * @brief resets the offset/index from castNext
    *
    */
    void reset() {
        m_objCastIndex = 0;
    }
    
    /**
    * @brief pointer to the internal buffer
    */
    int8_t* data() {
        return m_msgBuffer.data();
    }
    
    /**
    * @brief pointer to the remaining internal buffer
    */
    int8_t* remaining_data() {
        return m_msgBuffer.data() + m_msgLength;
    }

    /**
    * @brief pointer to the current position in the internal buffer
    */
    int8_t* data_offset() {
        return m_msgBuffer.data() + m_objCastIndex;
    }
    
    /**
    * @brief current buffer-capacity
    */
    size_t capacity() const {
        return m_msgBuffer.size();
    }
    
    /**
    * @brief resizes the internal buffer
    * 
    * @return size_t: new size of the buffer
    */
    void resize(int size) {
        m_msgBuffer.resize(size);
        m_msgLength = 0;
        m_objCastIndex = 0;
    }
    
    /**
    * @brief current message-length
    */
    size_t msg_length() const {
        return m_msgLength;
    }
    
    /**
    * @brief remaining message-length
    */ 
    size_t remaining_msg_length() const {
        return m_msgLength - m_objCastIndex;
    }
    
    /**
    * @brief remaining capacity of the underlying buffer
    */ 
    size_t remaining_capacity() const {
        return m_msgBuffer.size() - m_msgLength;
    }
    
    /**
    * @brief 
    * - sets the new valid length for the message
    * - resets the offset/index for castNext
    * @param length: new length | needs to be smaller than the capacity
    */
    void set_msg_length(int length) {
        if (length < 0) {
            length = 0;
        }

        m_msgLength = length;
        m_objCastIndex = 0;
    }
    
    /**
    * @brief 
    * - sets the new valid length for the message
    * - reserves this space and excludes it from any getNext()
    * - sets the offset/index to len
    * 
    * @param length new length | needs to be smaller than the capacity
    */    
    void reserve_length(int length) {
        if (length < 0) {
            length = 0;
        }

        m_msgLength = length;
        m_objCastIndex = length;
    }
private:
    // internal buffer for network-IO
    std::vector<int8_t> m_msgBuffer;
    // len of the real message
    int m_msgLength;
    // address-offset to the next posible object
    int m_objCastIndex;
};
}

#endif

