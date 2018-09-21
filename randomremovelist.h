///////////////////////////////////////////////////////////////////////////
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
///////////////////////////////////////////////////////////////////////////

#ifndef	LN_RANDOM_REMOVE_LIST__H__
#define	LN_RANDOM_REMOVE_LIST__H__

#include	<cstdlib>
#include	<cstdint>
#include	<stdexcept>
#include	<iterator>
#include	<vector>
#include	<unordered_set>
#include	<random>
#include	<algorithm>


//////////////////////////////////////////////////////////////////////////////
//
// randomremovelist
//
// Elements added to the list are randomly removed from the list, using get(),
// until all values have been returned once.  The list is then re-initialized
// with all elements that have been added and are again randomly removed from
// the list, once each.
//
//////////////////////////////////////////////////////////////////////////////

//
// Constructor flags
//
namespace RRLFlags
{
	// Adding an value that already exists anywhere in list has no effect
	static const uint32_t	NO_DUPLICATES	= 0x0001;

	// Elements are added, but will not be available for return until next recycle
	static const uint32_t	ADD_AS_USED		= 0x0002;
};


template<typename ETYPE>
class randomremovelist
{
public:
	/*
	 * Constructor
	 */
	randomremovelist(uint32_t flags = 0)
	{
		m_flags = flags;

		std::random_device	rd;

		this->random_seed(rd());
	}

	/*
	 * Use default destructor
	 */
	virtual ~randomremovelist() = default;

	/*
	 * add()
	 *
	 * Add a single value to list
	 */
	void add(ETYPE value)
	{
		this->add(1, &value);
	}

	/*
	 * add()
	 *
	 * Add an array of values to list
	 */
	void add(size_t count, const ETYPE *values)
	{
		this->add(values, values + count);
	}

	/*
	 * add()
	 *
	 * Add a vector of values to list
	 */
	void add(const std::vector<ETYPE> &values)
	{
		this->add(values.begin(), values.end());
	}

	/*
	 * add()
	 *
	 * Add an iterable list of values
	 */
	template<class InputIt>
	void add(InputIt it_start, InputIt it_end)
	{
		_LIST_ACCESS_BEGIN();

		// List value should be added to
		auto	*list = _is_flag_set(RRLFlags::ADD_AS_USED) ? &m_used : &m_remaining;

		if(_is_flag_set(RRLFlags::NO_DUPLICATES)) {
			// Only perform add if value is not in either used or remaining list
			for(auto add_it = it_start; add_it != it_end; ++add_it) {
				if(m_remaining.find(*add_it) == m_remaining.end() && m_used.find(*add_it) == m_used.end()) {
					list->insert(*add_it);
				}
			}
		}
		else {
			list->insert(it_start, it_end);
		}

		_LIST_ACCESS_END();
	}

	/*
	 * add_range()
	 *
	 * *** Only enabled for integer, floating-point, and pointer types
	 *
	 * Add a range of floating-point values to list
	 *	start_value - first value to add
	 *	count - number of values to add
	 *	step_size - increment value between each added value
	 *
	 * Example: addRange(1.0, 5, 0.1) will add values: 1.0, 1.1, 1.2, 1.3, 1.4
	 *			addRange(10, 10) will add values 10 --> 19
	 */
	void add_range(ETYPE start_value, size_t value_count, ETYPE step_size = ETYPE())
	{
		static_assert(std::is_arithmetic<ETYPE>::value || std::is_pointer<ETYPE>::value,
					  "add_range() called with non-arithetic and non-pointer element type");

		std::vector<ETYPE>	temp;

		temp.reserve(value_count);

		while(value_count-- > 0) {
			temp.push_back(start_value);
			start_value += step_size;
		}

		this->add(temp.begin(), temp.end());
	}

	/*
	 * get()
	 *
	 * Get a random value from list.  If all values have already been used,
	 * the list is recycled (all existing values are considered un-used)
	 */
	ETYPE get()
	{
		_LIST_ACCESS_BEGIN();

		if(m_remaining.size() == 0) {
			if(m_used.size() == 0) {
				// Empty list
				throw std::underflow_error("List has 0 total elements");
			}

			// Remaining size is 0, do a recycle
			this->_recycle();
		}

		// Get a random index into the remaining values list
		size_t	i = m_rand() % m_remaining.size();
		auto	it = m_remaining.begin();		
		ETYPE	ret;

		std::advance(it, i);
		ret = *it;
		m_remaining.erase(it);
		m_used.insert(ret);

		_LIST_ACCESS_END();
		return ret;
	}

	/*
	 * count_remaining()
	 *
	 * Returns the count of values remaining in the list
	 */
	size_t count_remaining()
	{
		size_t		ret;

		_LIST_ACCESS_BEGIN();
		ret = m_remaining.size();
		_LIST_ACCESS_END();

		return ret;
	}

	/*
	 * count_used()
	 *
	 * Returns the count of values that have been used since last recycle
	 */
	size_t count_used()
	{
		size_t		ret;

		_LIST_ACCESS_BEGIN();
		ret = m_used.size();
		_LIST_ACCESS_END();

		return ret;
	}

	/*
	 * count_total()
	 *
	 * Returns the total count of value in the list
	 */
	size_t count_total()
	{
		size_t		ret;

		_LIST_ACCESS_BEGIN();
		ret = m_remaining.size() + m_used.size();
		_LIST_ACCESS_END();

		return ret;
	}

	/*
	 * recycle()
	 *
	 * Force a recycle, make already used values unused
	 */
	void recycle()
	{
		_LIST_ACCESS_BEGIN();
		this->_recycle();
		_LIST_ACCESS_END();
	}

	/*
	 * clear()
	 *
	 * Clear all values from list, list will have 0 total values
	 */
	void clear()
	{
		_LIST_ACCESS_BEGIN();

		m_remaining.clear();
		m_used.clear();

		_LIST_ACCESS_END();
	}

	/*
	 * random_seed()
	 *
	 * Override random seed used during construction.
	 */
	void random_seed(uint64_t seed_value)
	{
		m_rand.seed(seed_value);
	}

private:
	void _recycle()
	{
		m_remaining.insert(m_used.begin(), m_used.end());
		m_used.clear();
	}

	bool _is_flag_set(uint32_t f)
	{
		return (m_flags & f);
	}

	// virtual functions for accessing lists, overloaded for re-entrant version
	virtual void _LIST_ACCESS_BEGIN()	{ }
	virtual void _LIST_ACCESS_END()		{ }

protected:
	std::unordered_multiset<ETYPE>		m_remaining;
	std::unordered_multiset<ETYPE>		m_used;
	uint32_t							m_flags;
	std::minstd_rand					m_rand;
};

#endif

