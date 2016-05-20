/*
 *   This file is part of SRS project.
 *
 *   SRS is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   SRS is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with SRS. If not, see <http://www.gnu.org/licenses/>.
 *
 *   Created by: Yifang Sun, Jianbin Qin
 *   Last modified by: Yifang Sun, Jianbin Qin
 */

#include <stdlib.h>

// v_array will be called in SRS_Cover_Tree
template<class T> class v_array {
public:
	int index;
	int length;
	T* elements;

	T last() {
		return elements[index - 1];
	}
	void decr() {
		index--;
	}
	v_array() {
		index = 0;
		length = 0;
		elements = NULL;
	}
	T& operator[](unsigned int i) {
		return elements[i];
	}
};

template<class T> void push(v_array<T>& v, const T &new_ele) {
	while (v.index >= v.length) {
		v.length = 2 * v.length + 3;
		v.elements = (T *) realloc(v.elements, sizeof(T) * v.length);
	}
	v[v.index++] = new_ele;
}

template<class T> void alloc(v_array<T>& v, int length) {
	v.elements = (T *) realloc(v.elements, sizeof(T) * length);
	v.length = length;
}

template<class T> v_array<T> pop(v_array<v_array<T> > &stack) {
	if (stack.index > 0)
		return stack[--stack.index];
	else
		return v_array<T>();
}
