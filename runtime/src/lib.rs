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

use std::ffi::CStr;
use std::ffi::CString;
use std::mem;
use std::os::raw::c_char;

pub const BASE_TYPE_INDX: usize = 0;
pub const ARRAY_MEMBER_TYPE_INDX: usize = 1;
pub const ARRAY_SIZE_LSB_INDX: usize = 2;
pub const ARRAY_SIZE_MSB_INDX: usize = 3;

// Compute total elements present in the data structure
pub fn compute_size(type_string: &str) -> i32 {
    let mut hex_bits = String::new();
    //Index 2 and 3 represent the size
    hex_bits.push(type_string.chars().nth(ARRAY_SIZE_LSB_INDX).unwrap());
    hex_bits.push(type_string.chars().nth(ARRAY_SIZE_MSB_INDX).unwrap());
    let bits: std::result::Result<i32, std::num::ParseIntError> =
        i32::from_str_radix(&hex_bits, 32);
    let total_bits: i32 = bits.unwrap();
    let mut length_bits = String::new();
    let index = 4;
    for i in 0..total_bits {
        let iterator = i as usize;
        length_bits.push(type_string.chars().nth(iterator + index).unwrap());
    }
    //Convert size to i32 for integer comparision
    let size: i32 = length_bits.parse().unwrap();
    return size;
}

/*
 * Returns size of the type
 * Type Notations -
 * C represents character
 * B represents boolean
 * F represents float
 * I represents integer
 * S represents string
 * X represents any*/
pub fn type_size(type_string: &str) -> Result<i32, &'static str> {
    let size: i32;
    match type_string.chars().nth(ARRAY_MEMBER_TYPE_INDX) {
        Some('C') | Some('B') => {
            size = 1;
            Ok(size)
        }
        Some('F') | Some('I') => {
            size = 8;
            Ok(size)
        }
        Some('S') => {
            size = 12;
            Ok(size)
        }
        Some('X') => {
            size = 16;
            Ok(size)
        }
        _ => Err("Unsupported type"),
    }
}

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
    if source.chars().nth(BASE_TYPE_INDX) != destination.chars().nth(BASE_TYPE_INDX) {
        return false;
    }
    let mut src_type_size: Result<i32, &'static str> = Ok(0);
    let mut dest_type_size: Result<i32, &'static str> = Ok(0);
    match source.chars().nth(BASE_TYPE_INDX) {
        Some('A') => {
            if source.chars().nth(ARRAY_MEMBER_TYPE_INDX)
                != destination.chars().nth(ARRAY_MEMBER_TYPE_INDX)
            {
                src_type_size = type_size(&source);
                dest_type_size = type_size(&destination);
                if src_type_size.is_err() || dest_type_size.is_err() {
                    panic!("Unsupported type");
                }
            }
            // If source type is bigger than destination, type cast is not possible
            if src_type_size.unwrap() > dest_type_size.unwrap() {
                return false;
            }
            // Compute total number of elements present in the data structure
            let src_size: i32 = compute_size(&source);
            let dest_size: i32 = compute_size(&destination);
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

#[test]
fn src_type_size_less_than_dest() {
    let src = CString::new("AF03122").unwrap();
    let dest = CString::new("AX03122").unwrap();
    assert_eq!(
        is_same_type(
            src.as_ptr() as *const c_char,
            dest.as_ptr() as *const c_char
        ),
        true
    );
}

#[test]
fn src_type_size_greater_than_dest() {
    let src = CString::new("AS03122").unwrap();
    let dest = CString::new("AB03122").unwrap();
    assert_eq!(
        is_same_type(
            src.as_ptr() as *const c_char,
            dest.as_ptr() as *const c_char
        ),
        false
    );
}

#[test]
fn src_elements_less_than_dest() {
    let src = CString::new("AB041023").unwrap();
    let dest = CString::new("AX041024").unwrap();
    assert_eq!(
        is_same_type(
            src.as_ptr() as *const c_char,
            dest.as_ptr() as *const c_char
        ),
        true
    );
}

#[test]
fn src_elements_greater_than_dest() {
    let src = CString::new("AC0519999").unwrap();
    let dest = CString::new("AX03199").unwrap();
    assert_eq!(
        is_same_type(
            src.as_ptr() as *const c_char,
            dest.as_ptr() as *const c_char
        ),
        false
    );
}

#[test]
fn src_type_string_equal_to_dest() {
    let src = CString::new("AF0599999").unwrap();
    let dest = CString::new("AF0599999").unwrap();
    assert_eq!(
        is_same_type(
            src.as_ptr() as *const c_char,
            dest.as_ptr() as *const c_char
        ),
        true
    );
}

#[test]
fn map_test() {
    let src = CString::new("MX0599999").unwrap();
    let dest = CString::new("MF0599999").unwrap();
    assert_eq!(
        is_same_type(
            src.as_ptr() as *const c_char,
            dest.as_ptr() as *const c_char
        ),
        false
    );
}
