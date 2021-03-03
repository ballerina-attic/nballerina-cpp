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

// Rust Library

extern crate num;
extern crate num_derive;

mod type_checker;

use std::ffi::CStr;
use std::mem;
use std::os::raw::c_char;

/*
 * To checking whether typecast is possible from source to destination
 * Example of a type string - "AI041024"
 * Index 0 - represents data structure
 * Index 1 - represents type
 * Index 2 and 3 - represents bits in hex followed by its size in decimal
 *
 * Algorithm -
 * a. Convert null terminated string to rust string.
 * b. Return true if type strings are same.
 * c. Return false if data structure is different.
 * d. Compute type size - type_size()
 * e. Return false if source type size > destination type size
 * g. Compute data structure length - compute_length()
 *
 * Data Structure Notations -
 * A - Array
 * M - Map
 * */
#[no_mangle]
pub extern "C" fn is_same_type(src_type: *const c_char, dest_type: *const c_char) -> bool {
    assert!(!src_type.is_null());
    assert!(!dest_type.is_null());
    //Conversion to CStr
    let source_cstr: &CStr = unsafe { CStr::from_ptr(src_type) };
    let dest_cstr: &CStr = unsafe { CStr::from_ptr(dest_type) };
    //Conversion to rust strings
    let source: String = source_cstr.to_str().unwrap().to_owned();
    let destination: String = dest_cstr.to_str().unwrap().to_owned();
    //If type strings are same
    if source == destination {
        return true;
    }
    //Index 0 represents type of data structure
    if source.chars().nth(type_checker::BASE_TYPE_INDEX)
        != destination.chars().nth(type_checker::BASE_TYPE_INDEX)
    {
        return false;
    }
    match source.chars().nth(type_checker::BASE_TYPE_INDEX) {
        Some('A') => {
            if source.chars().nth(type_checker::ARRAY_MEMBER_TYPE_INDEX)
                != destination
                    .chars()
                    .nth(type_checker::ARRAY_MEMBER_TYPE_INDEX)
            {
                let src_type_size: i32 = type_checker::type_size(&source);
                let dest_type_size: i32 = type_checker::type_size(&destination);
                if src_type_size > dest_type_size {
                    return false;
                }
            }
            // Compute total number of elements present in the data structure
            let src_size: i32 = type_checker::compute_size(&source);
            let dest_size: i32 = type_checker::compute_size(&destination);
            if src_size > dest_size {
                return false;
            }
        }
        _ => return false,
    }
    //If all the checks are passed, type cast from source to destination is valid
    return true;
}

// Prints 64 bit signed integer
#[no_mangle]
pub extern "C" fn print64(num64: i64) {
    println!("{}", num64);
}

// Prints 32 bit signed integer
#[no_mangle]
pub extern "C" fn print32(num32: i32) {
    println!("{}", num32);
}

// Prints 16 bit signed integer
#[no_mangle]
pub extern "C" fn print16(num16: i16) {
    println!("{}", num16);
}

// Prints 8 bit signed integer
#[no_mangle]
pub extern "C" fn print8(num8: i8) {
    println!("{}", num8);
}

// Prints 64 bit unsigned integer
#[no_mangle]
pub extern "C" fn printu64(num64: u64) {
    println!("{}", num64);
}

// Prints 32 bit unsigned integer
#[no_mangle]
pub extern "C" fn printu32(num32: u32) {
    println!("{}", num32);
}

// Prints 16 bit unsigned integer
#[no_mangle]
pub extern "C" fn printu16(num16: u16) {
    println!("{}", num16);
}

// Prints 8 bit unsigned integer
#[no_mangle]
pub extern "C" fn printu8(num8: u8) {
    println!("{}", num8);
}

// Prints 64 bit float
#[no_mangle]
pub extern "C" fn printf64(num64: f64) {
    println!("{}", num64);
}

// Prints 32 bit float
#[no_mangle]
pub extern "C" fn printf32(num32: f32) {
    println!("{}", num32);
}

#[no_mangle]
pub extern "C" fn new_int_array(size: i32) -> *mut Vec<*mut i32> {
    let mut size_t = size;
    if size < 0 {
        size_t = 8;
    }
    let size_t = size_t as usize;
    let foo: Box<Vec<*mut i32>> = Box::new(Vec::with_capacity(size_t));
    let vec_pointer = Box::into_raw(foo);
    return vec_pointer as *mut Vec<*mut i32>;
}

#[no_mangle]
pub extern "C" fn int_array_store(arr_ptr: *mut Vec<*mut i32>, n: i32, ref_ptr: *mut i32) {
    let mut arr = unsafe { Box::from_raw(arr_ptr) };
    let n_size = n as usize;
    let len = n_size + 1;
    if arr.len() < len {
        arr.resize(len, 0 as *mut i32);
    }
    arr[n_size] = ref_ptr;
    mem::forget(arr);
}

#[no_mangle]
pub extern "C" fn int_array_load(arr_ptr: *mut Vec<*mut i32>, n: i32) -> *mut i32 {
    let arr = unsafe { Box::from_raw(arr_ptr) };
    let n_size = n as usize;
    let return_val = arr[n_size];
    mem::forget(arr);
    return return_val;
}
