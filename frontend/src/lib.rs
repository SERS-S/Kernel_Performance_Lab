pub mod app;
pub mod backend_api;

use crate::backend_api::{BackendApi, KplBackendApiC};
use eframe::egui;
use std::os::raw::c_int;

#[no_mangle]
pub extern "C" fn kpl_frontend_run(api_ptr: *const KplBackendApiC) -> c_int {
    if api_ptr.is_null() {
        eprintln!("Fatal Error: Backend API pointer is null");
        return -1;
    }

    let _api = BackendApi::new(api_ptr);

    let result = eframe::run_simple_native(
        "Kernel Performance Lab",
        eframe::NativeOptions::default(),
        |ctx, _frame| {
            egui::CentralPanel::default().show(ctx, |ui| {
                ui.heading("Kernel Performance Lab");
                ui.label("Welcome! The application is running.");
            });
        },
    );

    match result {
        Ok(_) => 0,
        Err(_) => -2
    }
}