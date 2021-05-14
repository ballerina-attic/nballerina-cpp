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
mod type_checker;
pub use type_checker::BalType;

use std::convert::TryFrom;
use std::ffi::c_void;
use std::ffi::CStr;
use std::io::{self, Write};
use std::mem;
use std::os::raw::c_char;

mod bal_array;
pub use bal_array::dynamic_array::DynamicBalArray;

#[repr(C)]
pub struct BalAsciiString {
    header: u64,
    n_bytes: u64,
    value: *const u8,
}

#[no_mangle]
pub extern "C" fn print_string(ascii_ptr: *const BalAsciiString) {
    assert!(!ascii_ptr.is_null());
    let ptr = unsafe { (*ascii_ptr).value };
    assert!(!ptr.is_null());
    let _slice = unsafe { std::slice::from_raw_parts(ptr, (*ascii_ptr).n_bytes as usize) };
    let _string: &str = std::str::from_utf8(_slice).unwrap();
    print!("{}", _string);
    io::stdout().flush().unwrap();
}

// To check whether typecast is possible from source to destination
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
    debug_assert!(&source[..2] == "__" && &destination[..2] == "__");
    debug_assert!(source.len() > 2 && destination.len() > 2);

    let filtered_source = &source[2..];
    let filtered_destination = &destination[2..];
    return type_checker::same_type(filtered_source, filtered_destination);
}

// Prints 64 bit signed integer
#[no_mangle]
pub extern "C" fn print_integer(num64: i64) {
    println!("{}", num64);
}

// Prints 8 bit signed byte
#[no_mangle]
pub extern "C" fn print_byte(num8: i8) {
    println!("{}", num8);
}

// Prints 64 bit float
#[no_mangle]
pub extern "C" fn printf64(num64: f64) {
    println!("{}", num64);
}

// Prints boolean
#[no_mangle]
pub extern "C" fn print_boolean(num8: i8) {
    if num8 == 0 {
        println!("false");
    } else {
        println!("true");
    }
}

#[no_mangle]
pub extern "C" fn array_init_int(size: i64) -> *mut DynamicBalArray<i64> {
    let array: Box<DynamicBalArray<i64>> = Box::new(DynamicBalArray::<i64>::new(size, 3));
    let array_pointer = Box::into_raw(array);
    return array_pointer as *mut DynamicBalArray<i64>;
}

#[no_mangle]
pub extern "C" fn array_init_byte(size: i64) -> *mut DynamicBalArray<i64> {
    let array: Box<DynamicBalArray<i8>> = Box::new(DynamicBalArray::<i8>::new(size, 0));
    let array_pointer = Box::into_raw(array);
    return array_pointer as *mut DynamicBalArray<i64>;
}

#[no_mangle]
pub extern "C" fn array_init_float(size: i64) -> *mut Vec<f64> {
    let size_t = if size > 0 { size } else { 8 };
    let size_t = size_t as usize;
    let foo: Box<Vec<f64>> = Box::new(Vec::with_capacity(size_t));
    let vec_pointer = Box::into_raw(foo);
    return vec_pointer as *mut Vec<f64>;
}

#[no_mangle]
pub extern "C" fn array_init_bool(size: i64) -> *mut Vec<bool> {
    let size_t = if size > 0 { size } else { 8 };
    let size_t = size_t as usize;
    let foo: Box<Vec<bool>> = Box::new(Vec::with_capacity(size_t));
    let vec_pointer = Box::into_raw(foo);
    return vec_pointer as *mut Vec<bool>;
}

#[no_mangle]
pub extern "C" fn array_init_string(size: i64) -> *mut Vec<*mut BalAsciiString> {
    let size_t = if size > 0 { size } else { 8 };
    let size_t = size_t as usize;
    let foo: Box<Vec<*mut BalAsciiString>> = Box::new(Vec::with_capacity(size_t));
    let vec_pointer = Box::into_raw(foo);
    return vec_pointer as *mut Vec<*mut BalAsciiString>;
}

#[no_mangle]
pub extern "C" fn array_init_anydata(size: i32) -> *mut Vec<*mut c_void> {
    let size_t = if size > 0 { size } else { 8 };
    let size_t = size_t as usize;
    let vec: Box<Vec<*mut c_void>> = Box::new(Vec::with_capacity(size_t));
    let vec_pointer = Box::into_raw(vec);
    return vec_pointer;
}

#[no_mangle]
pub extern "C" fn array_store_int(arr_ptr: *mut DynamicBalArray<i64>, index: i64, ref_ptr: i64) {
    let mut arr = unsafe { Box::from_raw(arr_ptr) };
    let type_header = arr.get_header() & 3;
    if type_header == 3 {
        arr.set_element(index, ref_ptr);
        mem::forget(arr);
    } else if type_header == 0 {
        if ref_ptr < 0 || ref_ptr > 255 {
            panic!("{} value is incompatible with byte arrays", ref_ptr);
        }
        let new_arr_ptr: *mut DynamicBalArray<i8> = arr_ptr as *mut DynamicBalArray<i8>;
        mem::forget(arr);
        let mut new_arr = unsafe { Box::from_raw(new_arr_ptr) };
        new_arr.set_element(index, i8::try_from(ref_ptr).unwrap());
        mem::forget(new_arr);
    }
}

// #[no_mangle]
// pub extern "C" fn array_load_int(arr_ptr: *mut DynamicBalArray<i64>, index: i64) -> i64 {
//     let arr = unsafe { Box::from_raw(arr_ptr) };
//     let value = arr.get_element(index);
//     mem::forget(arr);
//     return value;
// }

#[no_mangle]
pub extern "C" fn array_store_byte(arr_ptr: *mut DynamicBalArray<i8>, index: i64, ref_ptr: i8) {
    if ref_ptr < 0 {
        panic!("{} value is incompatible with byte arrays", ref_ptr);
    }
    let mut arr = unsafe { Box::from_raw(arr_ptr) };
    let type_header = arr.get_header() & 3;
    if type_header == 0 {
        arr.set_element(index, ref_ptr);
        mem::forget(arr);
    } else if type_header == 3 {
        let new_arr_ptr: *mut DynamicBalArray<i64> = arr_ptr as *mut DynamicBalArray<i64>;
        mem::forget(arr);
        let mut new_arr = unsafe { Box::from_raw(new_arr_ptr) };
        new_arr.set_element(index, i64::try_from(ref_ptr).unwrap());
        mem::forget(new_arr);
    }
}

// #[no_mangle]
// pub extern "C" fn array_load_byte(arr_ptr: *mut DynamicBalArray<i8>, index: i64) -> i8 {
//     let arr = unsafe { Box::from_raw(arr_ptr) };
//     let value = arr.get_element(index);
//     mem::forget(arr);
//     return value;
// }

#[no_mangle]
pub extern "C" fn array_store_float(arr_ptr: *mut Vec<f64>, index: i64, ref_ptr: f64) {
    let mut arr = unsafe { Box::from_raw(arr_ptr) };
    let index_n = index as usize;
    let len = index_n + 1;
    if arr.len() < len {
        arr.resize(len, 0.0);
    }
    arr[index_n] = ref_ptr;
    mem::forget(arr);
}

#[no_mangle]
pub extern "C" fn array_load_anydata(arr_ptr: *mut Vec<*mut c_void>, index: i32) -> *mut c_void {
    let arr = unsafe { Box::from_raw(arr_ptr) };
    let index_n = index as usize;
    // check the out of bounds.
    assert!(arr.len() > index_n);
    let return_val = arr[index_n];
    mem::forget(arr);
    return return_val;
}

#[no_mangle]
pub extern "C" fn array_store_anydata(
    arr_ptr: *mut Vec<*mut c_void>,
    index: i32,
    ref_ptr: *mut c_void,
) {
    let mut arr = unsafe { Box::from_raw(arr_ptr) };
    let index_n = index as usize;
    let len = index_n + 1;
    if arr.len() < len {
        arr.resize(len, 0 as *mut c_void);
    }
    arr[index_n] = ref_ptr;
    mem::forget(arr);
}

#[no_mangle]
pub extern "C" fn array_load_float(arr_ptr: *mut Vec<f64>, index: i64) -> f64 {
    let arr = unsafe { Box::from_raw(arr_ptr) };
    let index_n = index as usize;
    // check the out of bounds.
    assert!(arr.len() > index_n);
    let return_val = arr[index_n];
    mem::forget(arr);
    return return_val;
}

#[no_mangle]
pub extern "C" fn array_store_bool(arr_ptr: *mut Vec<bool>, index: i64, ref_ptr: bool) {
    let mut arr = unsafe { Box::from_raw(arr_ptr) };
    let index_n = index as usize;
    let len = index_n + 1;
    if arr.len() < len {
        arr.resize(len, false);
    }
    arr[index_n] = ref_ptr;
    mem::forget(arr);
}

#[no_mangle]
pub extern "C" fn array_load_bool(arr_ptr: *mut Vec<bool>, index: i64) -> bool {
    let arr = unsafe { Box::from_raw(arr_ptr) };
    let index_n = index as usize;
    // check the out of bounds.
    assert!(arr.len() > index_n);
    let return_val = arr[index_n];
    mem::forget(arr);
    return return_val;
}

#[no_mangle]
pub extern "C" fn array_store_string(
    arr_ptr: *mut Vec<*mut BalAsciiString>,
    index: i64,
    ref_ptr: *mut BalAsciiString,
) {
    let mut arr = unsafe { Box::from_raw(arr_ptr) };
    let index_n = index as usize;
    let len = index_n + 1;
    let emptystr = BalAsciiString { header: 0, n_bytes: 0, value: 0 as *const u8};
    let emptystr_ptr = Box::into_raw(Box::new(emptystr));
    if arr.len() < len {
        arr.resize(len, emptystr_ptr as *mut BalAsciiString);
    }
    arr[index_n] = ref_ptr;
    mem::forget(arr);
}

#[no_mangle]
pub extern "C" fn array_load_string(arr_ptr: *mut Vec<*mut BalAsciiString>, index: i64) -> *mut BalAsciiString {
    let arr = unsafe { Box::from_raw(arr_ptr) };
    let index_n = index as usize;
    // check the out of bounds.
    assert!(arr.len() > index_n);
    let return_val = arr[index_n];
    mem::forget(arr);
    return return_val;
}

#[no_mangle]
pub extern "C" fn array_deinit_int(ptr: *mut Vec<i64>) {
    if ptr.is_null() {
        return;
    }
    unsafe {
        Box::from_raw(ptr);
    }
}

#[no_mangle]
pub extern "C" fn array_deinit_float(ptr: *mut Vec<f64>) {
    if ptr.is_null() {
        return;
    }
    unsafe {
        Box::from_raw(ptr);
    }
}

#[no_mangle]
pub extern "C" fn array_deinit_bool(ptr: *mut Vec<bool>) {
    if ptr.is_null() {
        return;
    }
    unsafe {
        Box::from_raw(ptr);
    }
}

#[no_mangle]
pub extern "C" fn array_deinit_string(ptr: *mut Vec<*mut BalAsciiString>) {
    if ptr.is_null() {
        return;
    }
    unsafe {
        Box::from_raw(ptr);
    }
}

#[no_mangle]
pub extern "C" fn box_bal_int(val: i64) -> *mut i64 {
    Box::into_raw(Box::new(val))
}

#[no_mangle]
pub extern "C" fn unbox_bal_int(ptr: *mut i64) {
    if ptr.is_null() {
        return;
    }
    unsafe {
        Box::from_raw(ptr);
    }
}

#[no_mangle]
pub extern "C" fn box_bal_float(val: f64) -> *mut f64 {
    Box::into_raw(Box::new(val))
}

#[no_mangle]
pub extern "C" fn unbox_bal_double(ptr: *mut f64) {
    if ptr.is_null() {
        return;
    }
    unsafe {
        Box::from_raw(ptr);
    }
}

#[no_mangle]
pub extern "C" fn box_bal_bool(val: i8) -> *mut i8 {
    Box::into_raw(Box::new(val))
}

#[no_mangle]
pub extern "C" fn unbox_bal_bool(ptr: *mut f64) {
    if ptr.is_null() {
        return;
    }
    unsafe {
        Box::from_raw(ptr);
    }
}

#[no_mangle]
pub extern "C" fn echo_rust(val: i64) -> i64 {
    val
}
