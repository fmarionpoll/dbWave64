#include "stdafx.h"
#include "NiceUnit.h"
#include "algorithmUtils.h"

TCHAR NiceUnit::cs_unit_[] = { _T("GM  m\u00B5pf  ") };  // \u00B5 is Unicode micro sign µ
int NiceUnit::units_power_[] = { 9, 6, 0, 0, -3, -6, -9, -12, 0 };
int NiceUnit::max_index_ = 8;
int NiceUnit::nice_intervals_[] = { 1, 5, 10, 20, 25, 30, 40, 50, 75, 100, 200, 250, 300, 400, 500, 0 };

int NiceUnit::nice_unit(const float y)
{
	auto i = 0;
	const auto i_value = static_cast<int>(y);
	do
	{
		if (i_value <= nice_intervals_[i])
		{
			i++;
			break;
		}
		i++;
	} while (nice_intervals_[i] > 0);
	return nice_intervals_[i - 1];
}


float NiceUnit::change_unit(float x_val, CString* x_unit, float* x_scale_factor)
{
	// avoid division by zero error
	if (x_val == 0)
	{
		*x_scale_factor = 1.0f;
		if (x_unit != nullptr)
		{
			// Ensure the string has at least one character before assignment
			x_unit->SetString(_T(" "), 1);
		}
		return 0.0f;
	}

	// take absolute value of xVal and save sign
	int i;
	int i_sign = 1;
	if (x_val < 0)
	{
		i_sign = -1;
		x_val = -x_val;
	}
	// get power of 10 of the value
	const auto i_log10_upper = static_cast<int>(log10(x_val)); // log10 of value (upper limit)
	if (i_log10_upper <= 0 && x_val < 1.) // perform extra checking if ip_rec <= 0
		i = 4 - i_log10_upper / 3; // change equation if Units values change
	else
		i = 3 - i_log10_upper / 3; // change equation if Units values change
	// clip index to valid range using dbw::clamp_value
	i = dbw::clamp_value<int>(i, 0, max_index_);
	// return data
	*x_scale_factor = static_cast<float>(pow(10.0f, units_power_[i])); // convert & store
	if (x_unit != nullptr)
	{
		// Safely set single-character unit string
		const TCHAR ch = cs_unit_[i];
		x_unit->SetString(&ch, 1);
	}
	return x_val * i_sign / *x_scale_factor; // return value/scale_factor
}