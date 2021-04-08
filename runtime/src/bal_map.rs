pub mod map {
    use std::collections::HashMap;

    pub struct BalMapInt {
        map: HashMap<String, i32>,
    }

    impl BalMapInt {
        pub fn new() -> BalMapInt {
            BalMapInt {
                map: HashMap::new(),
            }
        }
        // Map get
        pub fn get(&self, key: &str) -> Option<&i32> {
            self.map.get(key)
        }
        // Map insert
        pub fn insert(&mut self, key: &str, member: i32) {
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
}
