pub mod map {
    use std::collections::HashMap;
    use std::ffi::c_void;

    pub struct BalMapInt {
        map: HashMap<String, i64>,
    }

    impl BalMapInt {
        pub fn new() -> BalMapInt {
            BalMapInt {
                map: HashMap::new(),
            }
        }
        // Map get
        pub fn get(&self, key: &str) -> Option<&i64> {
            self.map.get(key)
        }
        // Map insert
        pub fn insert(&mut self, key: &str, member: i64) {
            self.map.insert(String::from(key), member);
        }

        // Insert with spread field expression
        pub fn insert_spread_field(&mut self, expr: &BalMapInt) {
            self.map.extend(expr.map.clone().into_iter());
        }

        // test functions
        pub fn length(&self) -> usize {
            self.map.len()
        }
    }

    #[repr(C)]
    pub struct SmtPtr {
        pub str_table_offset: i64,
        pub val: *mut c_void,
    }

    pub struct BalMapAnyData {
        map: HashMap<String, *mut SmtPtr>,
    }

    impl BalMapAnyData {
        pub fn new() -> BalMapAnyData {
            BalMapAnyData {
                map: HashMap::new(),
            }
        }

        // Map insert
        pub fn insert(&mut self, key: &str, member: *mut SmtPtr) {
            self.map.insert(String::from(key), member);
        }

        pub fn get(&self, key: &str) -> Option<&*mut SmtPtr> {
            self.map.get(key)
        }

        pub fn insert_spread_field(&mut self, expr: &BalMapAnyData) {
            self.map.extend(expr.map.clone().into_iter());
        }

        // test functions
        pub fn length(&self) -> usize {
            self.map.len()
        }
    }
}
