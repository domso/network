#pragma once

#include <cstring>
#include <type_traits>
#include <optional>

namespace network {
    class memory_region {
    public:
        memory_region() : m_data(nullptr), m_size(0) {}
        memory_region(uint8_t* data, size_t size) : m_data(data), m_size(size) {}
        
        template<typename T>
        void use(T& container) {
            m_data = reinterpret_cast<uint8_t*>(container.data());
            m_size = container.size();
        }    
        
        template<typename T>
        void use(T* data, const size_t size) {
            m_data = reinterpret_cast<uint8_t*>(data);
            m_size = size;
        }    
        
        uint8_t* data() {
            return m_data;
        }
        
        const uint8_t* data() const {
            return m_data;
        }
        
        size_t size() const {
            return m_size;
        }
        
        template<typename T>
        std::optional<const T*> cast_to() const {
            static_assert(std::is_trivially_copyable<T>::value);
            if (m_size == sizeof(T)) {
                return reinterpret_cast<T*>(m_data);
            }
            
            return std::nullopt;
        }
        
        template<typename T>
        std::optional<T*> cast_to() {
            static_assert(std::is_trivially_copyable<T>::value);
            if (m_size == sizeof(T)) {
                return reinterpret_cast<T*>(m_data);
            }
            
            return std::nullopt;
        }    
        
        memory_region splice(const size_t pos, const size_t n) const {            
            if (pos + n <= m_size) {
                return memory_region(m_data + pos, n);
            }       
            
            if (pos < m_size) {
                return memory_region(m_data + pos, m_size - pos);
            }
            
            return memory_region(nullptr, 0);
        }
        
        memory_region offset_front(const size_t offset) const {
            return splice(offset, m_size - offset);
        }
            
        memory_region offset_back(const size_t offset) const {
            return splice(0, m_size - offset);
        }
        
        bool contains(const memory_region& region) const {
            return m_data <= region.m_data && (region.m_data + region.m_size) <= (m_data + m_size);
        }
        
        bool contains(const void* ptr) const {
            return m_data <= ptr && ptr < m_data + m_size;
        }
        
        bool compare(const memory_region& region) const {
            if (m_size == region.size()) {
                return std::memcmp(region.data(), m_data, m_size) == 0;
            }            
            
            return false;
        }
        
        bool overlap(const memory_region& region) const {
            return contains(reinterpret_cast<const void*>(region.data())) || 
                   contains(reinterpret_cast<const void*>(region.data() + region.size() - 1)) ||
                   region.contains(reinterpret_cast<const void*>(data())) ||
                   region.contains(reinterpret_cast<const void*>(data() + size()));
        }
        
        void move(const memory_region& region) {
            std::memmove(m_data, region.m_data, std::min(m_size, region.size()));
        }
        
        template<typename T>
        void push_back_into(T& container) const {
            for (size_t i = 0; i < m_size; i++) {
                container.push_back(m_data[i]);
            }
        }
        
        class iterator {
        public:
            iterator() : m_data(nullptr) {};
            iterator(uint8_t* data) : m_data(data) {};
            
            bool operator==(const iterator& it) const {return m_data == it.m_data;}
            bool operator!=(const iterator& it) const {return m_data != it.m_data;}
            bool operator<(const iterator& it) const {return m_data < it.m_data;}
            bool operator>(const iterator& it) const {return m_data > it.m_data;}
            bool operator<=(const iterator& it) const {return m_data <= it.m_data;}
            bool operator>=(const iterator& it) const {return m_data >= it.m_data;}
            iterator operator++(int) {return iterator(m_data++);}
            iterator operator--(int) {return iterator(m_data--);}
            iterator& operator++() {m_data++; return *this;}
            iterator& operator--() {m_data--; return *this;}      
            
            uint8_t& operator*() {return *m_data;}
        private:
            uint8_t* m_data;
        };
        
        iterator begin() {
            return iterator(m_data);
        }
        
        iterator rbegin() {
            return iterator(m_data + m_size - 1);
        }
        
        iterator end() {
            return iterator(m_data + m_size);
        }
        
        iterator rend() {
            return iterator(m_data - 1);
        }        
    private:
        uint8_t* m_data;
        size_t m_size;
    };
}
