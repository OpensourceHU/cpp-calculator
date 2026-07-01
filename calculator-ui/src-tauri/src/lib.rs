use std::process::Command;
use std::path::PathBuf;
use tauri::Manager;

// Tauri command to call the C++ CLI calculator backend
#[tauri::command]
fn evaluate_expression(app: tauri::AppHandle, expression: String) -> Result<String, String> {
    let mut exe_path = PathBuf::new();
    
    // Check local C++ build folder during development
    let dev_path = PathBuf::from(r"C:\Users\Admin\dev\cpp\calculator-cli\build\calculator-cli.exe");
    if dev_path.exists() {
        exe_path = dev_path;
    } else {
        // Fallback to bundled resource path in production
        if let Ok(resource_path) = app.path().resource_dir() {
            let prod_path = resource_path.join("bin").join("calculator-cli.exe");
            if prod_path.exists() {
                exe_path = prod_path;
            }
        }
    }

    // Default fallback to PATH search
    if !exe_path.exists() {
        exe_path = PathBuf::from("calculator-cli.exe");
    }

    let output = Command::new(exe_path)
        .arg(&expression)
        .output()
        .map_err(|e| format!("Failed to execute C++ CLI backend: {}", e))?;

    let stdout = String::from_utf8_lossy(&output.stdout).to_string();
    Ok(stdout)
}

#[cfg_attr(mobile, tauri::mobile_entry_point)]
pub fn run() {
    tauri::Builder::default()
        .plugin(tauri_plugin_opener::init())
        .invoke_handler(tauri::generate_handler![evaluate_expression])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}

