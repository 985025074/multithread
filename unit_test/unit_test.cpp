#include <gtest/gtest.h>
#include "safe_stack.h"
#include "safe_queue.h"

TEST(queue, try_pop) {
    threadsafe_queue<int> q;
    q.push(1);
    q.push(2);
    q.push(3);
    int x = *q.try_pop();
    EXPECT_EQ(x, 1);
    x = *q.try_pop();
    EXPECT_EQ(x, 2);
    x = *q.try_pop();
    EXPECT_EQ(x, 3);
}
TEST(queue, try_pop_two) {
    threadsafe_queue<int> q;
    q.push(1);
    q.push(2);
    q.push(3);
    int x;
    q.try_pop(x);
    EXPECT_EQ(x, 1);
    q.try_pop(x);
    EXPECT_EQ(x, 2);
    q.try_pop(x);
    EXPECT_EQ(x, 3);
}
TEST(queue, try_wait) {
    threadsafe_queue<int> q;
    q.push(1);
    q.push(2);
    q.push(3);
    int x;
    q.wait_and_pop(x);
    EXPECT_EQ(x, 1);
    q.wait_and_pop(x);
    EXPECT_EQ(x, 2);
    q.wait_and_pop(x);
    EXPECT_EQ(x, 3);
}

TEST(queue, try_wait_two) {
    threadsafe_queue<int> q;
    q.push(1);
    q.push(2);
    q.push(3);
    int x = *q.wait_and_pop();
    EXPECT_EQ(x, 1);
    x = *q.wait_and_pop();
    EXPECT_EQ(x, 2);
    x = *q.wait_and_pop();
    EXPECT_EQ(x, 3);
}
TEST(queue,multithread_pop){
    threadsafe_queue<int> q;
    std::map<int,int> check;
    int ok_result = 0;
    std::thread a([&](){
        for(int i=0;i<100000;i++){
        q.push(i);
    }
    });
    std::thread b([&](){
        for(int i=0;i<100000;i++){
        int data;
        data = *q.wait_and_pop();
        check[data]++;
    }
    });
    a.join();
    b.join();
    for(int i=0;i<100000;i++){
        if(check[i]==1){
            ok_result++;
        }
    }

    EXPECT_EQ(ok_result,100000);
}