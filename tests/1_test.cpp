#include "gtest/gtest.h"
#include "1_deducing_types.h"

TEST(C1_deducing_types, ReferenceDeducesValue)
{
    A a;
    ref(a); // object deduced as ref
    EXPECT_EQ(1, a.i);
}

TEST(C1_deducing_types, ReferenceDeducesRef)
{
    A a;
    A& ra = a;
    ref(ra); // ref deduced as ref 
    EXPECT_EQ(1, a.i);
}

TEST(C1_deducing_types, ReferenceDeducesConstRef)
{
    A a;
    const A& cra = a;
    ref(cra);   // cref deduced as cref
                // const-qualified method must be called
    EXPECT_EQ(1, a.m);
    EXPECT_EQ(0, a.i);
}

TEST(C1_deducing_types, UniversalReferenceDeducesValue)
{
    A a;
    uref(a); // object deduced as lvalue ref
    EXPECT_EQ(1, a.i);
}

TEST(C1_deducing_types, UniversalReferenceDeducesReference)
{
    A a;
    A& ra = a;
    uref(ra); // ref deduced as lvalue ref
    EXPECT_EQ(1, a.i);
}

TEST(C1_deducing_types, UniversalReferenceDeducesConstReference)
{
    A a;
    const A& ra = a;
    uref(ra); // cref deduced as lvalue cref 
    EXPECT_EQ(0, a.i);
    EXPECT_EQ(1, a.m);
}

TEST(C1_deducing_types, UniversalReferenceDeducesRValueRef)
{
    A a;
    uref(std::move(a)); // passing rvalue, must compile
    EXPECT_EQ(1, a.i);
}

TEST(C1_deducing_types, ValueCopiesOnValue)
{
    A a;
    val(a); // copy
    EXPECT_EQ(0, a.i);
    EXPECT_TRUE(a.copied);
}

TEST(C1_deducing_types, ValueCopiesOnRef)
{
    A a;
    A& ra = a;
    val(ra); // reference is copyed
    EXPECT_EQ(0, a.i);
    EXPECT_TRUE(a.copied);
    EXPECT_FALSE(a.constcopied);
}

TEST(C1_deducing_types, ValueCopiesOnCRef)
{
    A a;
    const A& ra = a;
    val(ra); // copied as const
    EXPECT_EQ(0, a.i);
    EXPECT_FALSE(a.copied);
    EXPECT_TRUE(a.constcopied);
}

TEST(C1_deducing_types, PointerContinuesPointingToConstWhenCopied)
{
    A a;
    const A* const pa = &a;
    val(pa); // deduced const A*
    EXPECT_EQ(1, a.m);
    EXPECT_FALSE(a.copied);
    EXPECT_FALSE(a.constcopied);
}

TEST(C1_deducing_types, AutoDropsReference)
{
    A a;
    // param deduced as A&, no copying
    // increment returns A&
    // auto deduces A, thus copy
    auto ret = autoReturn(a);
    EXPECT_TRUE(a.copied);
    EXPECT_EQ(1, a.i);
}

TEST(C1_deducing_types, DecltypeAutoSavesReference)
{
    A a;
    // param deduced as A&, no copying
    // increment returns A&
    // decltype(auto) deduces A&, thus NO copy
    auto& ret = decltypeAutoReturn(a);
    EXPECT_FALSE(a.copied);
    EXPECT_EQ(&a, &ret);
}