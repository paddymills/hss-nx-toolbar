
mod settings;
pub use settings::SettingsApp;

#[no_mangle]
pub extern "C" fn run() {
    SettingsApp::run();
}