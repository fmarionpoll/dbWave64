#include "pch.h"
#include <gtest/gtest.h>
#include "../dbWave64/dbView2/RowCache.h"

class FakeProvider : public IDbWaveDataProvider
{
public:
	int getRecordsCount() const override { return 100; }
	RowMeta getRowMeta(int index) override
	{
		RowMeta m; m.index = index; m.insect_id = 42; return m;
	}
};

TEST(RowCacheTests, InitializesAndReadsRange)
{
	FakeProvider p;
	RowCache cache(&p);
	cache.setVisibleRange(0, 9);
	EXPECT_EQ(cache.getFirstIndex(), 0);
	EXPECT_EQ(cache.getSize(), 10);
	EXPECT_EQ(cache.at(0).index, 0);
	EXPECT_EQ(cache.at(9).index, 9);
}

TEST(RowCacheTests, ScrollsForwardReusesOverlap)
{
	FakeProvider p;
	RowCache cache(&p);
	RowCacheUpdatePlan plan{};
	cache.setVisibleRange(0, 9, &plan);
	EXPECT_EQ(plan.newCount, 10);
	cache.setVisibleRange(5, 14, &plan);
	EXPECT_GT(plan.shiftCount, 0);
	EXPECT_GE(cache.getFirstIndex(), 5);
	EXPECT_EQ(cache.at(0).index, 5);
}

TEST(RowCacheTests, ScrollsBackwardReusesOverlap)
{
	FakeProvider p;
	RowCache cache(&p);
	RowCacheUpdatePlan plan{};
	cache.setVisibleRange(20, 29, &plan);
	cache.setVisibleRange(15, 24, &plan);
	EXPECT_GE(plan.shiftCount, 0);
	EXPECT_EQ(cache.getFirstIndex(), 15);
	EXPECT_EQ(cache.at(0).index, 15);
}
