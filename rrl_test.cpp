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

#include	<cstdlib>
#include	<cstdio>
#include	<vector>

#include	"randomremovelist_r.h"


static bool
test_int(bool dup_allowed);


int
main(int argc, char **argv)
{
	printf("\n*** Test with Duplicates\n\n");
	test_int(true);
	printf("\n*** Test without Duplicates\n\n");
	test_int(false);
	return 0;
}


static bool
test_int(bool dup_allowed)
{
	randomremovelist_r<int>		list_int(dup_allowed ? 0 : RRLFlags::NO_DUPLICATES);
	std::vector<int>			v_int = { 2, 4, 6, 8, 10 };
	size_t						el_count = 0;
	size_t						added_el_count = 0;

	list_int.add(-1);
	++el_count;
	// List has: -1
	list_int.add(v_int.size(), v_int.data());
	el_count += v_int.size();
	// List has: -1 2 4 6 8 10
	list_int.add(v_int.begin(), v_int.end());
	el_count += dup_allowed ? v_int.size() : 0;
	// List has: -1 2 2 4 4 6 6 8 8 10 10
	//		or:  -1 2 4 6 8 10 (duplicates / no duplicates)
	list_int.add_range(100, 10, 10);
	el_count += 10;
	// List has: -1 2 2 4 4 6 6 8 8 10 10 100 110 120 130 140 150 160 170 180 190
	//		or:  -1 2 4 6 8 10 100 110 120 130 140 150 160 170 180 190 (duplicates / no duplicates)

	if(list_int.count_total() == el_count && list_int.count_remaining() == el_count) {
		printf("List has [%zi] total/remaining elements\n", list_int.count_total());
	}
	else {
		printf("ERROR: List has [%zi] total/remaining elements\n", list_int.count_total());
		return false;
	}

	for(int i = 0; i < el_count; ++i) {
		printf("%5i", list_int.get());
	}

	printf("\n");

	if(list_int.count_total() == el_count && list_int.count_remaining() == 0) {
		printf("List has [%zi] total and [%zi] remaining elements\n", list_int.count_total(), list_int.count_remaining());
	}
	else {
		printf("ERROR: List has [%zi] total and [%zi] remaining elements\n", list_int.count_total(), list_int.count_remaining());
		return false;
	}

	printf("\n");

	printf("Add 5 identical elements...\n");

	list_int.add_range(999, 5, 0);
	added_el_count = dup_allowed ? 5 : 1;
	el_count += added_el_count;

	printf("\n");

	if(list_int.count_total() == el_count && list_int.count_remaining() == added_el_count) {
		printf("List has [%zi] total and [%zi] remaining elements\n", list_int.count_total(), list_int.count_remaining());
	}
	else {
		printf("ERROR: List has [%zi] total and [%zi] remaining elements\n", list_int.count_total(), list_int.count_remaining());
		return false;
	}

	for(int i = 0; i < added_el_count; ++i) {
		printf("%5i", list_int.get());
	}

	printf("\n");

	if(list_int.count_total() == el_count && list_int.count_remaining() == 0) {
		printf("List has [%zi] total and [%zi] remaining elements\n", list_int.count_total(), list_int.count_remaining());
	}
	else {
		printf("ERROR: List has [%zi] total and [%zi] remaining elements\n", list_int.count_total(), list_int.count_remaining());
		return false;
	}

	printf("\nGetting one element for recycle...\n\n");

	list_int.get();

	if(list_int.count_total() == el_count && list_int.count_remaining() == (el_count - 1)) {
		printf("List has [%zi] total and [%zi] remaining elements\n", list_int.count_total(), list_int.count_remaining());
	}
	else {
		printf("ERROR: List has [%zi] total and [%zi] remaining elements\n", list_int.count_total(), list_int.count_remaining());
		return false;
	}

	printf("\n");
	return true;
}

