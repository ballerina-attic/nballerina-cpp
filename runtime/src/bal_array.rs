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

pub mod dynamic_array{
    const GROWTH_FACTOR:i64 = 2;
    use libc;
    use std::mem;
    use crate::BalType;
    use std::convert::TryFrom;

    pub struct DynamicBalArray {
        header: i64,
        inherentType: BalType,
        length: i64,
        capacity: i64,
        array: DynamicArray
    }

    struct DynamicArray {
        header: u64,
        values: *mut i64,
    }

    impl DynamicBalArray{
        pub fn new(size: i64)->DynamicBalArray{
            let size_t = if size > 0 { size } else { 8 };
            let dynamic_array = DynamicArray{
                header:0,
                values: DynamicBalArray::allocate_memory(size_t)
            };
            return DynamicBalArray{
                header:0,
                inherentType:BalType::Int,
                length:0,
                capacity: size_t,
                array: dynamic_array
            }
        }

        fn allocate_memory(size: i64)-> *mut i64{
            unsafe{
                let mem_ptr: *mut i64 = libc::calloc(usize::try_from(size).unwrap(),mem::size_of::<i64>()) as *mut i64;
                if mem_ptr.is_null() {
                    panic!("Failed to allocate memory");
                }
                return mem_ptr;
            }
        }

        fn copy_memory(source: *const i64, dest:*mut i64, size: i64){
            unsafe {
                std::ptr::copy(source, dest, usize::try_from(size).unwrap());
            }
        }

        fn deallocate_memory(mem_ptr: *mut i64){
            unsafe{
                libc::free(mem_ptr as *mut libc::c_void);
            }
        }

        pub fn get_element(&self, index:i64)-> i64{
            if self.length <= index {
                panic!("Index out of range");
            } else {
                unsafe{
                    return *self.array.values.offset(isize::try_from(index).unwrap());
                }
            }
        }

        pub fn set_element(&mut self, index:i64, value:i64){
            if self.capacity <= index {
               let new_size = self.capacity * GROWTH_FACTOR;
               let new_array = DynamicBalArray::allocate_memory(new_size); 
               DynamicBalArray::copy_memory(self.array.values, new_array, self.capacity);
               DynamicBalArray::deallocate_memory(self.array.values);
               self.array.values = new_array;
               self.capacity = new_size;
            }
            unsafe {
                *self.array.values.offset(isize::try_from(index).unwrap()) = value;
            }
            if self.length <= index {

               self.length = index+1;
            }

        }
    }

}