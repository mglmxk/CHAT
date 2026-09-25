#pragma once

#include "message.hpp"
#include <map>
#include <vector>
#include <string>
#include <functional>

class ChatStore {
public: 
    void create_room(const std::string& room_name) { 
        rooms_[room_name] = {};
    }

    void post(const std::string& room_name, const Message& msg) {
        rooms_[room_name].push_back(msg);
    }

    void post(const std::string& room_name, Message&& msg){
        rooms_[room_name].push_back(std::move(msg));
    }

    const std::vector<Message>* history(const std::string& room_name) const {
        auto it = rooms_.find(room_name);
        if (it != rooms_.end()){
            return &(it->second);
        }
        return nullptr;
    }

    size_t room_count() const {
        return rooms_.size();
    }

    template<typename T>
    void for_each_room(T t) {
        for (auto& [name, messages] : rooms_){
            t(name, messages); 
        }
    }

private: 
    std::map<std::string, std::vector<Message>> rooms_;
};