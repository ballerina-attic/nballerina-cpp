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

use crate::is_same_type;

use num::FromPrimitive;
use num_derive::FromPrimitive;
use std::ffi::CString;
use std::os::raw::c_char;

pub const BASE_TYPE_INDEX: usize = 0;
pub const ARRAY_MEMBER_TYPE_INDEX: usize = 1;
pub const ARRAY_SIZE_LSB_INDEX: usize = 2;
pub const ARRAY_SIZE_MSB_INDEX: usize = 3;

#[derive(Debug, PartialEq, FromPrimitive)]
#[repr(u32)]
enum BalType {
    Nil = 'N' as u32,
    Boolean = 'B' as u32,
    Int = 'I' as u32,
    Float = 'F' as u32,
    Decimal = 'D' as u32,
    String = 'S' as u32,
    Array = 'A' as u32,
    Any = 'X' as u32,
}

// Compute total elements present in the data structure
pub fn compute_size(type_string: &str) -> i32 {
    let mut hex_bits = String::new();
    //Index 2 and 3 represent the size
    hex_bits.push(type_string.chars().nth(ARRAY_SIZE_LSB_INDEX).unwrap());
    hex_bits.push(type_string.chars().nth(ARRAY_SIZE_MSB_INDEX).unwrap());
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

pub fn type_size(type_string: &str) -> i32 {
    let type_char = type_string
        .chars()
        .nth(ARRAY_MEMBER_TYPE_INDEX)
        .unwrap_or_else(|| panic!("Illegal type descriptor '{}', wrong length", type_string));
    let bal_type = FromPrimitive::from_u32(type_char as u32).unwrap_or_else(|| {
        panic!(
            "Illegal type tag '{}' in type descriptor '{}'",
            type_char, type_string
        )
    });
    match bal_type {
        BalType::Boolean => 1,
        BalType::Int | BalType::Float => 8,
        BalType::String => 12,
        BalType::Any => 16,
        _ => {
            unimplemented!("type_size for '{:?}'", bal_type)
        }
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
pub extern "C" fn same_type(source: &str, destination: &str) -> bool {
    //If type strings are same
    if source == destination {
        return true;
    }
    //Index 0 represents type of data structure
    if source.chars().nth(BASE_TYPE_INDEX) != destination.chars().nth(BASE_TYPE_INDEX) {
        return false;
    }
    match source.chars().nth(BASE_TYPE_INDEX) {
        Some('A') => {
            return is_same_array_type(source, destination);
        }
        Some('M') => {
            return is_same_map_type(source, destination);
        }
        _ => return false,
    }
}

fn is_same_array_type(source: &str, destination: &str) -> bool {
    if source.chars().nth(ARRAY_MEMBER_TYPE_INDEX)
        != destination.chars().nth(ARRAY_MEMBER_TYPE_INDEX)
    {
        let src_type_size: i32 = type_size(source);
        let dest_type_size: i32 = type_size(destination);
        if src_type_size > dest_type_size {
            return false;
        }
    }
    // Compute total number of elements present in the data structure
    let src_size: i32 = compute_size(source);
    let dest_size: i32 = compute_size(destination);
    return if src_size > dest_size { false } else { true };
}

fn is_same_map_type(source: &str, destination: &str) -> bool {
    let filtered_source = &source[1..];
    let filtered_destination = &destination[1..];
    return is_same_map_constraint_type(filtered_source, filtered_destination);
}

fn is_same_map_constraint_type(source: &str, destination: &str) -> bool {
    return same_type(source, destination);
}

#[test]
fn src_type_size_less_than_dest() {
    let src = CString::new("__AF03122").unwrap();
    let dest = CString::new("__AX03122").unwrap();
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
    let src = CString::new("__AS03122").unwrap();
    let dest = CString::new("__AB03122").unwrap();
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
    let src = CString::new("__AB041023").unwrap();
    let dest = CString::new("__AX041024").unwrap();
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
    let src = CString::new("__AB0519999").unwrap();
    let dest = CString::new("__AX03199").unwrap();
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
    let src = CString::new("__AF0599999").unwrap();
    let dest = CString::new("__AF0599999").unwrap();
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
    let src = CString::new("__MX0599999").unwrap();
    let dest = CString::new("__MF0599999").unwrap();
    assert_eq!(
        is_same_type(
            src.as_ptr() as *const c_char,
            dest.as_ptr() as *const c_char
        ),
        false
    );
}

#[test]
fn without_underscore_test() {
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
