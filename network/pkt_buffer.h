#ifndef pkt_buffer_h
#define pkt_buffer_h

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <stdint.h>
#include "network/ip_addr.h"

namespace network {
    //______________________________________________________________________________________________________
    //
    // Simple buffer for packet-IO
    //______________________________________________________________________________________________________
    class pkt_buffer {
    public:
        //______________________________________________________________________________________________________
        //
        // Description:
        // - initialize the buffer with a capacity of 'bufferSize'
        // Parameter:
        // - bufferCapacity: init buffer-capacity
        //______________________________________________________________________________________________________
        pkt_buffer(int bufferCapacity) : m_msgBuffer(bufferCapacity), m_msgLength(0), m_objCastIndex(0) {

        }
        //______________________________________________________________________________________________________
        //
        // Description:
        // - gets the next valid pointer to n casted CAST_TYPE-objects
        // - a valid object was either inserted by usering pushNext() or directly modifying the buffer with data() and setMsgLen()
        // - every object is NOT a new instance, but a reference to the internal buffer
        // - the lifetime last until the next resize()-call
        // - any other modification with the internal data (-> data()-call) will have side-effects
        // Parameter:
        // - n: number of CAST_TYPE-instances to cast
        // Return:
        // - pointer to an instance of CAST_TYPE
        // - null-pointer | if no instance is available
        //______________________________________________________________________________________________________
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
        //______________________________________________________________________________________________________
        //
        // Description:
        // - reserves the memory for n CAST_TYPE-objects in the buffer
        // - every object is NOT a new instance, but a reference to the internal buffer
        // - the lifetime last until the next resize()-call
        // - any other modification with the internal data (-> data()-call) will have side-effects
        // Parameter:
        // - n: number of CAST_TYPE-instances to cast
        // Return:
        // - pointer to an instance of CAST_TYPE
        // - null-pointer | if no instance is available
        //______________________________________________________________________________________________________
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
        //______________________________________________________________________________________________________
        //
        // Description:
        // - pushes a std::string in the buffer
        // Return:
        // - true  | on success
        // - false | on error
        //______________________________________________________________________________________________________
        bool push_string(const std::string& input) {
            char* dest = push_next<char>(input.length());
            
            if (dest != nullptr) {
                std::strncpy(dest, input.c_str(),  input.length());
                return true;
            }

            return false;
        }
        //______________________________________________________________________________________________________
        //
        // Description:
        // - resets the offset/index from castNext
        //______________________________________________________________________________________________________
        void reset() {
            m_objCastIndex = 0;
        }
        //______________________________________________________________________________________________________
        //
        // Return:
        // - pointer to the internal buffer
        //______________________________________________________________________________________________________
        int8_t* data() {
            return m_msgBuffer.data();
        }
        //______________________________________________________________________________________________________
        //
        // Return:
        // - pointer to the remaining internal buffer
        //______________________________________________________________________________________________________
        int8_t* remaining_data() {
            return m_msgBuffer.data() + m_msgLength;
        }
        //______________________________________________________________________________________________________
        //
        // Return:
        // - pointer to the current position in the internal buffer
        //______________________________________________________________________________________________________
        int8_t* data_offset() {
            return m_msgBuffer.data() + m_objCastIndex;
        }
        //______________________________________________________________________________________________________
        //
        // Return:
        // - current buffer-capacity
        //______________________________________________________________________________________________________
        size_t capacity() const {
            return m_msgBuffer.size();
        }
        //______________________________________________________________________________________________________
        //
        // Description:
        // - resizes the internal buffer
        // Parameter:
        // - size: new size of the buffer
        //______________________________________________________________________________________________________
        void resize(int size) {
            m_msgBuffer.resize(size);
            m_msgLength = 0;
            m_objCastIndex = 0;
        }
        //______________________________________________________________________________________________________
        //
        // Return:
        // - current message-length
        //______________________________________________________________________________________________________
        size_t msg_length() const {
            return m_msgLength;
        }
        //______________________________________________________________________________________________________
        //
        // Return:
        // - remaining message-length
        //______________________________________________________________________________________________________
        size_t remaining_msg_length() const {
            return m_msgLength - m_objCastIndex;
        }
        //______________________________________________________________________________________________________
        //
        // Return:
        // - remaining capacity
        //______________________________________________________________________________________________________
        size_t remaining_capacity() const {
            return m_msgBuffer.size() - m_msgLength;
        }
        //______________________________________________________________________________________________________
        //
        // Description:
        // - sets the new valid length for the message
        // - resets the offset/index for castNext
        // Parameter:
        // - len: new length | needs to be smaller than the capacity
        //______________________________________________________________________________________________________
        void set_msg_length(int length) {
            if (length < 0) {
                length = 0;
            }

            m_msgLength = length;
            m_objCastIndex = 0;
        }
        //______________________________________________________________________________________________________
        //
        // Description:
        // - sets the new valid length for the message
        // - reserves this space and excludes it from any getNext()
        // - sets the offset/index to len
        // Parameter:
        // - len: new length | needs to be smaller than the capacity
        //______________________________________________________________________________________________________
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

