#include <gtest/gtest.h>
#include "message.hpp"
#include "owning_buffer.hpp"
#include "chat_store.hpp"

// Тесты Message
TEST(MessageTest, CopyDoesNotInvokeMove) {
    Message::copy_ctor_calls = 0;
    Message::move_ctor_calls = 0;

    Message msg1("Alice", "Hello");
    Message msg2(msg1);  

    EXPECT_GT(Message::copy_ctor_calls, 0);
    EXPECT_EQ(Message::move_ctor_calls, 0);
}

TEST(MessageTest, MoveDoesNotInvokeCopy) {
    Message::copy_ctor_calls = 0;
    Message::move_ctor_calls = 0;

    Message msg1("Alice", "Hello");
    Message msg2(std::move(msg1));  

    EXPECT_EQ(Message::copy_ctor_calls, 0);
    EXPECT_GT(Message::move_ctor_calls, 0);
}

TEST(MessageTest, AddTemporaryToRoomDoesNotCopy) {
    Message::copy_ctor_calls = 0;
    Message::move_ctor_calls = 0;

    ChatStore store;
    store.create_room("test");
    store.post("test", Message("Alice", "Hello"));  

    EXPECT_EQ(Message::copy_ctor_calls, 0);
    EXPECT_GT(Message::move_ctor_calls, 0);
}

// OwningBuffer
TEST(OwningBufferTest, SelfAssignmentDoesNotDestroy) {
    OwningBuffer buf(10);
    buf.data()[0] = 'A';
    
    buf = buf;  
    
    EXPECT_EQ(buf.size(), 10);
    EXPECT_EQ(buf.data()[0], 'A');
}

TEST(OwningBufferTest, CopyCreatesIndependentMemory) {
    OwningBuffer buf1(5);
    buf1.data()[0] = 'X';
    
    OwningBuffer buf2(buf1);
    buf2.data()[0] = 'Y';
    
    EXPECT_EQ(buf1.data()[0], 'X');  
    EXPECT_EQ(buf2.data()[0], 'Y');
}

TEST(OwningBufferTest, MovedObjectInValidState) {
    OwningBuffer buf1(5);
    buf1.data()[0] = 'Z';
    
    OwningBuffer buf2(std::move(buf1));
    
    EXPECT_EQ(buf2.size(), 5);
    EXPECT_EQ(buf2.data()[0], 'Z');
    EXPECT_EQ(buf1.size(), 0);  
    EXPECT_EQ(buf1.data(), nullptr);
}

//ChatStore
TEST(ChatStoreTest, CreateAndPost) {
    ChatStore store;
    store.create_room("test");
    store.post("test", Message("Alice", "Hello"));
    
    auto history = store.history("test");
    ASSERT_NE(history, nullptr);
    EXPECT_EQ(history->size(), 1);
    EXPECT_EQ(history->at(0).author(), "Alice");
}

TEST(ChatStoreTest, HistoryReturnsNullForNonexistentRoom) {
    ChatStore store;
    EXPECT_EQ(store.history("nonexistent"), nullptr);
}

TEST(ChatStoreTest, RoomCount) {
    ChatStore store;
    EXPECT_EQ(store.room_count(), 0);
    
    store.create_room("room1");
    store.create_room("room2");
    EXPECT_EQ(store.room_count(), 2);
}