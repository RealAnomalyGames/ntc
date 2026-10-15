
const fileInput = document.getElementById("source-file");
const fileInfo = document.getElementById("file-info");
const compileButton = document.getElementById("compile-button");
const clearButton = document.getElementById("clear-button");
const compilerOutput = document.getElementById("compiler-output");
const statusText = document.getElementById("status-text");
const downloadLink = document.getElementById("download-link");

let selectedFile = null;

fileInput.addEventListener("change", () => {
    selectedFile = fileInput.files[0] || null;
    downloadLink.hidden = true;

    if (!selectedFile) {
        fileInfo.textContent = "No source file selected.";
        compileButton.disabled = true;
        statusText.textContent = "Ready";
        return;
    }

    if (!selectedFile.name.toLowerCase().endsWith(".ntc")) {
        selectedFile = null;
        fileInput.value = "";
        fileInfo.textContent = "Please select a file ending in .ntc.";
        compilerOutput.textContent =
            "ERROR: Unsupported file type.\nPlease select an NTC source file.";
        compileButton.disabled = true;
        statusText.textContent = "Invalid file";
        return;
    }

    fileInfo.textContent =
        `Selected: ${selectedFile.name} (${selectedFile.size} bytes)`;

    compilerOutput.textContent =
        "Source file selected successfully.\n\n" +
        "The WebAssembly compiler will be connected in the next step.";

    compileButton.disabled = false;
    statusText.textContent = "Source selected";
});

clearButton.addEventListener("click", () => {
    fileInput.value = "";
    selectedFile = null;
    compileButton.disabled = true;
    downloadLink.hidden = true;

    fileInfo.textContent = "No source file selected.";
    compilerOutput.textContent =
        "Welcome to the NTC Web Compiler.\n\n" +
        "Select a .ntc source file to get started.";

    statusText.textContent = "Ready";
});

compileButton.addEventListener("click", async () => {
    if (!selectedFile) return;

    compilerOutput.textContent =
        "The WebAssembly compiler is not connected yet.\n" +
        "No executable has been generated.";

    statusText.textContent = "Awaiting compiler setup";
});
