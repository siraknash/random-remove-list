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

#ifndef	LN_RANDOM_REMOVE_LIST_R__H__
#define	LN_RANDOM_REMOVE_LIST_R__H__


#include	"randomremovelist.h"
#include	<pthread.h>


//////////////////////////////////////////////////////////////////////////////
//
// randomremovelist_r
//
// Re-entrant version of randomremove list (see 'randomremovelist.h`)
//
//////////////////////////////////////////////////////////////////////////////

template<typename ETYPE>
class randomremovelist_r : public randomremovelist<ETYPE>
{
public:
	/*
	 * Constructor
	 */
	randomremovelist_r(uint32_t flags) : randomremovelist<ETYPE>(flags)
	{
		m_mutex = PTHREAD_MUTEX_INITIALIZER;
	}

	/*
	 * Destructor
	 */
	virtual ~randomremovelist_r()
	{
		::pthread_mutex_destroy(&m_mutex);
	}

private:
	// virtual function which locks mutex for re-entrant version
	virtual void _LIST_ACCESS_BEGIN()
	{
		::pthread_mutex_lock(&m_mutex);
	}

	// virtual function which locks mutex for re-entrant version
	virtual void _LIST_ACCESS_END()
	{
		::pthread_mutex_unlock(&m_mutex);
	}

private:
	pthread_mutex_t		m_mutex;
};

#endif

