#ifndef pkt_buffer_h
#define pkt_buffer_h

#include <vector>
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
        pkt_buffer(int bufferCapacity) : msgBuffer_(bufferCapacity), objCastIndex_(0) {

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
        CAST_TYPE* getNext(const int n = 1) {
            CAST_TYPE* result = nullptr;

            // if the next object is still inside the valid-buffer
            if (objCastIndex_ + sizeof(CAST_TYPE)*n <= msgLen_) {
                // create pointer to simulated instance
                result = (CAST_TYPE*)(msgBuffer_.data() + objCastIndex_);
                // increase offset by the size of the simulated instance
                objCastIndex_ += sizeof(CAST_TYPE) * n;
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
        CAST_TYPE* pushNext(const int n = 1) {
            CAST_TYPE* result = nullptr;

            // if the next object is still inside the buffer
            if (objCastIndex_ + sizeof(CAST_TYPE)*n <= msgBuffer_.size()) {
                // create pointer to simulated instance
                result = (CAST_TYPE*)(msgBuffer_.data() + objCastIndex_);
                // increase offset by the size of the simulated instance
                objCastIndex_ += sizeof(CAST_TYPE) * n;

                if (msgLen_ < objCastIndex_) {
                    msgLen_ = objCastIndex_;
                }
            }

            return result;
        }
        //______________________________________________________________________________________________________
        //
        // Description:
        // - resets the offset/index from castNext
        //______________________________________________________________________________________________________
        void reset() {
            objCastIndex_ = 0;
        }
        //______________________________________________________________________________________________________
        //
        // Return:
        // - pointer to the internal buffer
        //______________________________________________________________________________________________________
        char* data() {
            return msgBuffer_.data();
        }
        //______________________________________________________________________________________________________
        //
        // Return:
        // - pointer to the current position in the internal buffer
        //______________________________________________________________________________________________________
        char* dataOffset() {
            return msgBuffer_.data() + objCastIndex_;
        }
        //______________________________________________________________________________________________________
        //
        // Return:
        // - current buffer-capacity
        //______________________________________________________________________________________________________
        size_t capacity() const {
            return msgBuffer_.size();
        }
        //______________________________________________________________________________________________________
        //
        // Description:
        // - resizes the internal buffer
        // Parameter:
        // - size: new size of the buffer
        //______________________________________________________________________________________________________
        void resize(int size) {
            msgBuffer_.resize(size);
            msgLen_ = 0;
            objCastIndex_ = 0;
        }
        //______________________________________________________________________________________________________
        //
        // Return:
        // - current message-length
        //______________________________________________________________________________________________________
        size_t msgLen() const {
            return msgLen_;
        }
        //______________________________________________________________________________________________________
        //
        // Return:
        // - remaining message-length
        //______________________________________________________________________________________________________
        size_t remainingMsgLen() const {
            return msgLen_ - objCastIndex_;
        }
        //______________________________________________________________________________________________________
        //
        // Description:
        // - sets the new valid length for the message
        // - resets the offset/index for castNext
        // Parameter:
        // - len: new length | needs to be smaller than the capacity
        //______________________________________________________________________________________________________
        void setMsgLen(int len) {
            if (len < 0) {
                len = 0;
            }

            msgLen_ = len;
            objCastIndex_ = 0;
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
        void reserveLen(int len) {
            if (len < 0) {
                len = 0;
            }

            msgLen_ = len;
            objCastIndex_ = len;
        }
    private:
        // internal buffer for network-IO
        std::vector<char> msgBuffer_;
        // len of the real message
        int msgLen_;
        // address-offset to the next posible object
        int objCastIndex_;
    };
}

#endif
