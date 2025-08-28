#pragma once

#include <vector>
#include "DbWaveDataProvider.h"

struct RowCacheUpdatePlan
{
	int shiftSourceStart { 0 };
	int shiftDestStart { 0 };
	int shiftCount { 0 };
	int shiftDelta { 0 };
	int newStartOffset { 0 };
	int newCount { 0 };
	int firstIndex { 0 };
	int size { 0 };
};

class RowCache
{
public:
	explicit RowCache(IDbWaveDataProvider* provider)
		: provider_(provider) {}

	void setVisibleRange(int first, int last)
	{
		RowCacheUpdatePlan plan{};
		setVisibleRange(first, last, &plan);
	}

	void setVisibleRange(int first, int last, RowCacheUpdatePlan* out_plan)
	{
		if (last < first)
			last = first;
		int size = last - first + 1;
		if (size < 0) size = 0;

		RowCacheUpdatePlan plan{};
		plan.firstIndex = first;
		plan.size = size;

		const bool had_rows = !rows_.empty();
		const int old_first = first_index_;
		const int old_size = static_cast<int>(rows_.size());

		if (!had_rows || size <= 0)
		{
			plan.newStartOffset = 0;
			plan.newCount = size;
		}
		else
		{
			const int offset = first - old_first;
			if (offset > 0 && offset < old_size)
			{
				// scroll forward
				plan.shiftDelta = 1;
				plan.shiftCount = old_size - offset;
				plan.shiftSourceStart = offset;
				plan.shiftDestStart = 0;
				plan.newStartOffset = plan.shiftCount;
				plan.newCount = size - plan.shiftCount;
			}
			else if (offset < 0 && -offset < old_size)
			{
				// scroll backward
				plan.shiftDelta = -1;
				plan.shiftCount = old_size + offset;
				plan.shiftSourceStart = plan.shiftCount - 1;
				plan.shiftDestStart = old_size - 1;
				plan.newStartOffset = 0;
				plan.newCount = size - plan.shiftCount;
			}
			else
			{
				// full rebuild
				plan.newStartOffset = 0;
				plan.newCount = size;
			}
		}

		// Build new row window reusing old ones when indices overlap
		std::vector<RowMeta> new_rows;
		new_rows.resize(static_cast<size_t>(size));
		const int old_last = old_first + old_size - 1;
		for (int i = 0; i < size; ++i)
		{
			const int global_index = first + i;
			if (had_rows && global_index >= old_first && global_index <= old_last)
			{
				const int old_idx = global_index - old_first;
				new_rows[static_cast<size_t>(i)] = rows_[static_cast<size_t>(old_idx)];
			}
			else
			{
				new_rows[static_cast<size_t>(i)] = provider_->get_row_meta(global_index);
			}
		}

		rows_.swap(new_rows);
		first_index_ = first;

		if (out_plan)
			*out_plan = plan;
	}

	int getFirstIndex() const { return first_index_; }
	int getSize() const { return static_cast<int>(rows_.size()); }
	const RowMeta& at(int i) const { return rows_[static_cast<size_t>(i)]; }

private:
	IDbWaveDataProvider* provider_ { nullptr };
	int first_index_ { 0 };
	std::vector<RowMeta> rows_ {};
};


