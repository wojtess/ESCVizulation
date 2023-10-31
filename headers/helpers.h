#pragma once
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <mutex>

template <class T>
class Buffer {
private:
    T* array;
    int size;
    int head_index = 0;
    std::mutex lock;
public:
    Buffer(int size) {
        this->size = size;
        this->array = (T*)calloc(size, sizeof(T));
    }
    ~Buffer() {
        free(this->array);
    }
    Buffer(Buffer& other) {
        std::scoped_lock guard{other.lock, lock};
        this->size = other.size;
        this->head_index = other.head_index;
        this->array = (T*)calloc(size, sizeof(T));
        memcpy(this->array, other.array, sizeof(T) * size);
    }
    void push(T data) {
        std::lock_guard<std::mutex> guard(lock);
        memcpy(&array[head_index], &data, sizeof(data));
        // array[head_index] = data;
        head_index++;
        if(head_index >= size) {
            head_index = 0;
        }
    }
    std::vector<T> getVector() {
        std::lock_guard<std::mutex> guard(lock);
        T* array_out = (T*)malloc(sizeof(T) * size);

        //copy from array + head_index, array is base pointer with defined type(this is why no *sizeof(T)), head_index is offset
        //size to copy is diffrence beetwen head and end of array, size is always greater that head_index. Then it is multiplayed by sizeof(T), beacuse memcpy need to know how many bytes is going te be copied not how many elements
        memcpy(array_out, array + head_index, (size - head_index) * sizeof(T));
        //target dont have multiplcation sizeof(T), beacuse array_out is pointer of type(not (void*)), so we need only to know how many elements we need to add, not how many bytes
        //then copy everythin that is from start of array to head_index
        memcpy(array_out + (size - head_index), array, head_index * sizeof(T));
        std::vector<T> out(array_out, array_out + size);
        free(array_out);
        return out;
    }
    std::vector<T> getRawData() {
        std::lock_guard<std::mutex> guard(lock);
        T* array_out = (T*)malloc(sizeof(T) * size);

        memcpy(array_out, array, size * sizeof(T));
        std::vector<T> out(array_out, array_out + size);
        free(array_out);
        return out;
    }
    void setSize(int newSize) {
        std::lock_guard<std::mutex> guard(lock);
        if(newSize == size) {
            return;
        }
        if(newSize <= 0) {
            //wtf, dude use normal numbers not negative shit. "please give me -4 apples" what that even mean?
            return;
        }
        T* sorted_array = (T*)malloc(sizeof(T) * size);

        //copy from array + head_index, array is base pointer with defined type(this is why no *sizeof(T)), head_index is offset
        //size to copy is diffrence beetwen head and end of array, size is always greater that head_index. Then it is multiplayed by sizeof(T), beacuse memcpy need to know how many bytes is going te be copied not how many elements
        memcpy(sorted_array, array + head_index, (size - head_index) * sizeof(T));
        //target dont have multiplcation sizeof(T), beacuse array_out is pointer of type(not (void*)), so we need only to know how many elements we need to add, not how many bytes
        //then copy everythin that is from start of array to head_index
        memcpy(sorted_array + (size - head_index), array, head_index * sizeof(T));
        
        array = (T*)realloc(array, newSize * sizeof(T));

        if(newSize < size) {
            //shrink
            memcpy(array, sorted_array, newSize * sizeof(T));
        } else {
            //grow
            memcpy(array, sorted_array, size * sizeof(T));
            //set last value to all new empty entries
            //example: [1,2,3] --setSize(5)--> [1,2,3,3,3]
            for(int i = size; i < newSize; i++) {
                array[i] = array[size - 1];
            }
        }

        free(sorted_array);

        head_index = 0;
        size = newSize;
    }
    int getSize() {
        return size;
    }
};
