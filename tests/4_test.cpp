#include "4_smart_pointers.h"
#include "gtest/gtest.h"
#include "MemoryLeakDetector.h"

TEST(LinkedListWithSmartPtrs, DefaultCtor)
{
    MemoryLeakDetector d;
    SmartPtrLinkedList<int> list;
    EXPECT_EQ(list.size(), 0);
    EXPECT_TRUE(list.empty());
}

TEST(LinkedListWithSmartPtrs, InitializerListCtor)
{
    MemoryLeakDetector d;
    SmartPtrLinkedList<int> list {1,2,3,4,5};
    EXPECT_EQ(list.size(), 5);
}

TEST(LinkedListWithSmartPtrs, EmptyListsAreEqual)
{
    MemoryLeakDetector d;
    SmartPtrLinkedList<int> list1;
    SmartPtrLinkedList<int> list2;
    EXPECT_EQ(list1, list2);
}

TEST(LinkedListWithSmartPtrs, ListIsEqualToItself)
{
    MemoryLeakDetector d;
    SmartPtrLinkedList<int> list;
    EXPECT_EQ(list, list);
}

TEST(LinkedListWithSmartPtrs, ChecksForEqualityAndInequality)
{
    MemoryLeakDetector d;
    SmartPtrLinkedList<int> list1 {1,2,3,4,5};
    SmartPtrLinkedList<int> list2 {1,2,3,4,5};
    SmartPtrLinkedList<int> list3 {1,2,3,4};
    SmartPtrLinkedList<int> list4 {5,4,3,2,1};
    SmartPtrLinkedList<int> list5 {};

    EXPECT_EQ(list1, list2);
    EXPECT_NE(list1, list3);
    EXPECT_NE(list1, list4);
    EXPECT_NE(list1, list5);
    EXPECT_NE(list2, list3);
    EXPECT_NE(list2, list4);
    EXPECT_NE(list2, list5);
    EXPECT_NE(list3, list4);
    EXPECT_NE(list3, list5);
    EXPECT_NE(list4, list5);
}

TEST(LinkedListWithSmartPtrs, CorrectlyInserts)
{
    MemoryLeakDetector d;
    SmartPtrLinkedList<int> list;
    SmartPtrLinkedList<int> afterPushFront {1};
    list.pushFront(1);
    EXPECT_EQ(list, afterPushFront);
    auto it = list.insertAfter(list.begin(), 2);
    SmartPtrLinkedList<int> afterInsert {1, 2};
    EXPECT_EQ(list, afterInsert);
    auto expected = list.begin();
    ++expected;
    EXPECT_EQ(it, expected);
}

TEST(LinkedListWithSmartPtrs, CorrectlyErases)
{
    MemoryLeakDetector d;
    SmartPtrLinkedList<int> list {1,2,3,4,5};
    SmartPtrLinkedList<int> afterPopFront {2,3,4,5};
    list.popFront();
    EXPECT_EQ(list, afterPopFront);
}

TEST(LinkedListWithSmartPtrs, VeryBigListCorrectlyDeletes)
{
    MemoryLeakDetector d;
    SmartPtrLinkedList<int> list;
    for (int i = 0; i < 100000; i++) {
        list.pushFront(i);
    }
}

TEST(LinkedListWithSmartPtrs, CorrectlyCopiesAndAssignes)
{
    MemoryLeakDetector d;
    SmartPtrLinkedList<int> list1 {2,3,4};
    SmartPtrLinkedList<int> list2 (list1);
    EXPECT_EQ(list1, list2);
    EXPECT_NE(list1.begin(), list2.begin());
    list1 = {5,6,7};
    SmartPtrLinkedList<int> list3 {5,6,7};
    EXPECT_EQ(list1, list3);
    list2 = std::move(list1);
    EXPECT_EQ(list2, list3);
    EXPECT_TRUE(list1.empty());
}