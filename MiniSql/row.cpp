// #include "record/row.h"
// #include <iostream>
// uint32_t Row::SerializeTo(char *buf, Schema *schema) const {
//   char * s = buf;
//   MACH_WRITE_INT32(s,fields_.size());
//   buf+=sizeof(int);
//   for(auto i:fields_){
//     MACH_WRITE_TO(bool,buf,i->IsNull());
//     buf+=sizeof(bool);
//   }
//   for(auto i:fields_){
//     if(!i->IsNull()){
//       buf+=i->SerializeTo(buf);
//     }
//   }
//   return buf-s;
// }

// uint32_t Row::DeserializeFrom(char *buf, Schema *schema) {
//   char*s = buf;
//   int size = MACH_READ_INT32(buf);
//   buf+=sizeof(int);
//   bool*isbool = new bool[size];
//   for(int i = 0;i<size;++i){
//     isbool[i] = MACH_READ_FROM(bool,buf);
//     buf+=sizeof(bool);
//   }
//   for(uint32_t i = 0;i<size;++i){
//     if(!isbool[i]){
//       fields_.push_back(nullptr);
//       buf+=fields_[i]->DeserializeFrom(buf,schema->GetColumn(i)->GetType(),&fields_[i],false,heap_);
//     }else{
//       Field* field = reinterpret_cast<Field*>(ALLOC_P(heap_,Field)(schema->GetColumn(i)->GetType()));
//       fields_.push_back(field);
//     }
//   }
//   return buf-s;
// }

// uint32_t Row::GetSerializedSize(Schema *schema) const {
//   uint32_t size = sizeof(int)+sizeof(bool)*fields_.size();
//   for(uint32_t i = 0;i<fields_.size();++i){
//     if(!fields_[i]->IsNull())size+=fields_[i]->GetSerializedSize();
//   }
//   return size;
// }

#include "row.h"
uint32_t Row::SerializeTo(char* buf, Schema* schema) const {
    char* temp = buf;
    // size
    MACH_WRITE_UINT32(temp, fields_.size());
    temp += sizeof(uint32_t);
    // judge ifn exist
    for (size_t i = 0; i < fields_.size(); i++) {
        MACH_WRITE_TO(bool, temp, fields_[i]->IsNull());
        temp += sizeof(bool);
    }
    // data
    for (size_t i = 0; i < fields_.size(); i++) {
        if (fields_[i]->IsNull() == false) {
            fields_[i]->SerializeTo(temp);
            temp += fields_[i]->GetSerializedSize();
        }
    }
    // how many bytes forward
    return temp - buf;
}

uint32_t Row::DeserializeFrom(char* buf, Schema* schema) {
    char* temp = buf;
    // size
    uint32_t size = MACH_READ_UINT32(temp);
    temp += sizeof(uint32_t);
    // ifn exist
    bool* null_map = new bool[size];
    for (size_t i = 0; i < size; i++) {
        null_map[i] = MACH_READ_FROM(bool, temp);
        temp += sizeof(bool);
    }
    // read data
    for (size_t i = 0; i < schema->GetColumnCount(); ++i) {
        if (null_map[i] == false) {
            fields_.push_back(nullptr);
            fields_[i]->DeserializeFrom(temp, schema->GetColumn(i)->GetType(), &(fields_[i]), null_map[i], heap_);
            temp += fields_[i]->GetSerializedSize();
        }
        else {
            void* mem_alloc = heap_->Allocate(sizeof(Field));
            new (mem_alloc) Field(schema->GetColumn(i)->GetType());
            fields_.push_back((Field*)mem_alloc);
        }
    }
    return temp - buf;
}

uint32_t Row::GetSerializedSize(Schema* schema) const {
    uint32_t Size = sizeof(uint32_t) + sizeof(bool) * fields_.size();
    for (size_t i = 0; i < fields_.size(); ++i) {
        if (fields_[i]->IsNull() == false) Size += fields_[i]->GetSerializedSize();
    }
    return Size;
}
