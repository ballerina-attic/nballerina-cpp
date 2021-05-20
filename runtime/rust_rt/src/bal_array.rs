/*
 * Copyright (c) 2021, WSO2 Inc. (http://www.wso2.org) All Rights Reserved.
 *
 * WSO2 Inc. licenses this file to you under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except
 * in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 */

pub mod dynamic_array {
    const GROWTH_FACTOR: i64 = 2;
    use crate::BalType;
    use std::alloc::{alloc_zeroed, realloc, Layout};
    use std::convert::TryFrom;
    use std::mem;

    #[repr(C)]
    pub struct DynamicBalArray<T: Copy> {
        header: i64,
        inherent_type: BalType,
        length: i64,   // largest index of an element stored in the array + 1
        capacity: i64, // amount of memory actually allocated for
        array: *mut DynamicArray<T>,
    }

    #[repr(C)]
    struct DynamicArray<T: Copy> {
        header: u64,
        values: [T],
    }

    pub enum ArrayHeader {
        Int = 3,
        Byte = 0,
    }

    impl<T: Copy> DynamicBalArray<T> {
        pub fn new(size: i64, header: ArrayHeader) -> DynamicBalArray<T> {
            let size_t = if size > 0 { size } else { 8 };
            let layout = DynamicBalArray::<T>::get_layout(size_t);
            let raw_ptr;
            unsafe {
                raw_ptr = alloc_zeroed(layout);
            }
            if raw_ptr.is_null() {
                panic!("Array initialization failed");
            }
            let dynamic_array =
                std::ptr::slice_from_raw_parts_mut(raw_ptr, layout.size()) as *mut DynamicArray<T>;
            return DynamicBalArray {
                header: header as i64,
                inherent_type: BalType::Int,
                length: 0,
                capacity: size_t,
                array: dynamic_array,
            };
        }

        fn get_layout(capacity: i64) -> Layout {
            let header_layout = Layout::new::<i64>();
            let align = mem::align_of::<T>();
            let size = usize::try_from(capacity).unwrap() * mem::size_of::<T>();
            let values_layout = Layout::from_size_align(size, align).unwrap();
            return header_layout.extend(values_layout).unwrap().0;
        }

        fn grow_array(&mut self, old_capacity: i64, new_capacity: i64) {
            let old_layout = DynamicBalArray::<T>::get_layout(old_capacity);
            let new_layout = DynamicBalArray::<T>::get_layout(new_capacity);
            unsafe {
                let raw_ptr = realloc(self.array as *mut u8, old_layout, new_layout.size());
                if raw_ptr.is_null() {
                    panic!("Array resizing failed");
                }
                let dynamic_array = std::ptr::slice_from_raw_parts_mut(raw_ptr, new_layout.size())
                    as *mut DynamicArray<T>;
                self.array = dynamic_array;
            }
        }

        pub fn get_element(&self, index: i64) -> T {
            if self.length <= index {
                panic!("Index out of range");
            } else {
                unsafe {
                    return (*self.array).values[usize::try_from(index).unwrap()];
                }
            }
        }

        pub fn set_element(&mut self, index: i64, value: T) {
            if self.capacity <= index {
                let repeat = (index / (self.capacity * GROWTH_FACTOR)) + 1;
                let new_size = self.capacity * GROWTH_FACTOR * repeat;
                self.grow_array(self.capacity, new_size);
                self.capacity = new_size;
            }
            unsafe {
                (*self.array).values[usize::try_from(index).unwrap()] = value;
            }
            if self.length <= index {
                self.length = index + 1;
            }
        }

        pub fn get_header(&mut self) -> i64 {
            return self.header;
        }
    }
}
