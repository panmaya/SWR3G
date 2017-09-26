


#include "adl_global.h"
#include "middleValue_sort.h"



u16 middleValue_Sort(u16 a[], u8 n) {

//  GET FROM http://www.cs.princeton.edu/~ah/alg_anim/gawain-4.0/BubbleSort.html
//  TEST BY MASAYOS

	static int i, j;
	static int tmp;
	static int middle_value;

	for (i = 0; i < n - 1; i++) {
		for (j = 0; j < n - 1 - i; j++) {
			if (a[j + 1] < a[j]) {  // compare the two neighbors
				tmp = a[j];         // swap a[j] and a[j+1]
				a[j] = a[j + 1];
				a[j + 1] = tmp;
			}
		}
	}
	middle_value = a[(int) (n / 2)]; // find value  middle position (after sort
	return (middle_value);
}