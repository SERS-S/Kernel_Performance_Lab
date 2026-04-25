use serde::Deserialize;
use std::ffi::{CStr, CString};
use std::os::raw::{c_char, c_int, c_void};

#[repr(C)]
pub struct KplBackendApiC {
    pub ctx: *mut c_void,
    pub run_start: extern "C" fn(ctx: *mut c_void, scenario_name: *const c_char) -> c_int,
    pub run_list: extern "C" fn(ctx: *mut c_void) -> *mut c_char,
    pub string_free: extern "C" fn(ctx: *mut c_void, str_ptr: *mut c_char),
}

#[derive(Deserialize, Debug, Clone)]
pub struct RunItem {
    pub id: i32,
    pub scenario: String,
    pub status: String,
}

pub struct BackendApi {
    api_c: *const KplBackendApiC,
}

impl BackendApi {
    pub fn new(api_c: *const KplBackendApiC) -> Self {
        Self { api_c }
    }

    fn get_json_and_free<T: for<'a> Deserialize<'a>>(&self, json_ptr: *mut c_char) -> Option<T> {
        if json_ptr.is_null() {
            return None;
        }

        let result = unsafe {
            let c_str = CStr::from_ptr(json_ptr);
            let rust_str = c_str.to_str().unwrap_or("");

            let parsed: Option<T> = serde_json::from_str(rust_str).ok();

            let api = &*self.api_c;
            (api.string_free)(api.ctx, json_ptr);

            parsed
        };

        result
    }

    pub fn list_runs(&self) -> Vec<RunItem> {
        unsafe {
            let api = &*self.api_c;
            let json_ptr = (api.run_list)(api.ctx);
            self.get_json_and_free(json_ptr).unwrap_or_default()
        }
    }

    pub fn start_run(&self, scenario: &str) -> i32 {
        unsafe {
            let api = &*self.api_c;
            let c_scenario = CString::new(scenario).unwrap();
            (api.run_start)(api.ctx, c_scenario.as_ptr())
        }
    }
}

unsafe impl Send for BackendApi {}
unsafe impl Sync for BackendApi {}
