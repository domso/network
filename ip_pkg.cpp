#include "ip_pkg.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
network::ip_pkg::ip_pkg(const std::vector<char>& buffer, const int len, const ip_addr& addr, const char defaultBufferValue) {
    dataPTR_ = new ip_pkg_data(buffer, len, addr, defaultBufferValue);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
network::ip_pkg::ip_pkg(const int len, const network::ip_addr& addr, const char defaultBufferValue) {
    dataPTR_ = new ip_pkg_data(len, addr, defaultBufferValue);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
network::ip_pkg::ip_pkg(const network::ip_pkg& that) {
    dataPTR_ = that.dataPTR_;
    sem_post(& (dataPTR_->semaphore));
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
network::ip_pkg::~ip_pkg() {
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
void network::ip_pkg::operator= (const network::ip_pkg that) {
    dataPTR_ = that.dataPTR_;
    sem_post(& (dataPTR_->semaphore));
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
std::string network::ip_pkg::toString() const {
    return "[Package][" + std::to_string(dataPTR_->data.size()) + " Bytes][IPv4:" 
    + static_cast<ipv4_addr*>(&dataPTR_->addr)->getIP() + ":" + std::to_string(static_cast<ipv4_addr*>(&dataPTR_->addr)->getPort())
    
    + "][IPv6:"
    
    + static_cast<ipv6_addr*>(&dataPTR_->addr)->getIP() + ":" + std::to_string(static_cast<ipv6_addr*>(&dataPTR_->addr)->getPort())
    
    +"][Age: " + std::to_string(getAge()) + "]";
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
std::vector< char >& network::ip_pkg::getData() const {
    return dataPTR_->data;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
network::ip_addr& network::ip_pkg::getAddr() const {
    return dataPTR_->addr;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int network::ip_pkg::getAge() const {
    std::chrono::high_resolution_clock::time_point t = std::chrono::high_resolution_clock::now();
    int tmp = std::chrono::duration_cast<std::chrono::microseconds>(t - dataPTR_->time).count();
    return tmp;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
network::ip_pkg::ip_pkg_data::ip_pkg_data(const std::vector< char >& buffer, const int len, const network::ip_addr& addr, const char defaultBufferValue) {
    sem_init(&semaphore, 0, 1);
    data.resize(len, defaultBufferValue);
    int startIndex = len - buffer.size();
    if (startIndex < 0) { startIndex = 0; }
    int msgLen;
    
    if (len > buffer.size()){
        msgLen = buffer.size();
    }else{
        msgLen = len;        
    }

    for (int i = 0; i < msgLen ; i++) {
        data[startIndex + i] = buffer[i];
    }
    time = std::chrono::high_resolution_clock::now();
    this->addr = addr;
    this->addr.update();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
network::ip_pkg::ip_pkg_data::ip_pkg_data(const int len, const network::ip_addr& addr, const char defaultBufferValue) {
    sem_init(&semaphore, 0, 1);
    data.resize(len, defaultBufferValue);
    time = std::chrono::high_resolution_clock::now();
    this->addr = addr;
    this->addr.update();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
network::ip_pkg::ip_pkg_data::~ip_pkg_data() {
    sem_destroy(&semaphore);
}

