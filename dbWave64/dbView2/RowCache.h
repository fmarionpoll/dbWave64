#pragma once

#include <vector>
#include "DbWaveDataProvider.h"

class RowCache
{
public:
	explicit RowCache(IDbWaveDataProvider* provider)
		: provider_(provider) {}

	void setVisibleRange(int first, int last)
	{
		if (first < 0) first = 0;
		const int count = provider_ ? provider_->getRecordsCount() : 0;
		if (last >= count) last = count - 1;
		if (last < first) last = first;

		const int size = last - first + 1;
		if (size < 0) { rows_.clear(); first_index_ = 0; return; }

		rows_.resize(static_cast<size_t>(size));
		first_index_ = first;
		for (int i = 0; i < size; ++i)
		{
			rows_[static_cast<size_t>(i)] = provider_->getRowMeta(first + i);
		}
	}

	int getFirstIndex() const { return first_index_; }
	int getSize() const { return static_cast<int>(rows_.size()); }
	const RowMeta& at(int i) const { return rows_[static_cast<size_t>(i)]; }

private:
	IDbWaveDataProvider* provider_ { nullptr };
	int first_index_ { 0 };
	std::vector<RowMeta> rows_ {};
};


