#ifndef smart_ptr_h
#define smart_ptr_h
# include <iostream>
#include "semaphore.h"

class smart_ptr {
public:
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    smart_ptr() {
    }
    // copy constructor
    smart_ptr(const smart_ptr& that) {
        dataPTR_ = that.dataPTR_;
        sem_post(& (dataPTR_->semaphore));
    }
    // default destructor
    ~smart_ptr() {
        if (dataPTR_ != nullptr) {
            sem_wait(& (dataPTR_->semaphore));
            int tmp = 0;
            sem_getvalue(& (dataPTR_->semaphore), &tmp);
            if (tmp <= 0) {
                delete dataPTR_;
            }
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // assignment for smartPointer
    void operator =(const smart_ptr& that) {
        if (dataPTR_ != nullptr) {
            sem_wait(& (dataPTR_->semaphore));
            int tmp = 0;
            sem_getvalue(& (dataPTR_->semaphore), &tmp);
            if (tmp <= 0) {
                delete dataPTR_;
            }
        }
        dataPTR_ = that.dataPTR_;
        sem_post(& (dataPTR_->semaphore));
    }
protected:
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // data-struct for smartPointer
    struct smart_ptr_data {
        smart_ptr_data() {
            sem_init(&semaphore, 0, 1);
        }
        ~smart_ptr_data() {
            sem_destroy(&semaphore);
        }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // semaphore for smartPointer
        sem_t semaphore;
    };
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void setNewData(smart_ptr_data* data) {
        dataPTR_ = data;
    }
    //
    smart_ptr_data* getData() {
        return dataPTR_;
    }
private:
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // ptr to internal data
    smart_ptr_data* dataPTR_;
};


#endif




