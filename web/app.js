const fileInput = document.getElementById("source-file");
const fileInfo = document.getElementById("file-info");
const compileButton = document.getElementById("compile-button");
const clearButton = document.getElementById("clear-button");
const compilerOutput = document.getElementById("compiler-output");
const statusText = document.getElementById("status-text");
const downloadLink = document.getElementById("download-link");

const MAX_SOURCE_SIZE = 1024 * 1024;

let selectedFile = null;
let downloadUrl = null;
let compiling = false;

function showMessages(messages) {
    compilerOutput.textContent = messages.join("\n") ||
        "The compiler produced no messages.";
}

function releaseDownload() {
    if (downloadUrl) {
        URL.revokeObjectURL(downloadUrl);
        downloadUrl = null;
    }

    downloadLink.removeAttribute("href");
    downloadLink.removeAttribute("download");
    downloadLink.hidden = true;
}

function setBusy(busy) {
    compiling = busy;
    compileButton.disabled = busy || !selectedFile;
    clearButton.disabled = busy;
    fileInput.disabled = busy;
}

fileInput.addEventListener("change", () => {
    releaseDownload();
    selectedFile = fileInput.files[0] || null;

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
            "ERROR: Unsupported file type.\nSelect an NTC source file.";
        compileButton.disabled = true;
        statusText.textContent = "Invalid file";
        return;
    }

    if (selectedFile.size > MAX_SOURCE_SIZE) {
        selectedFile = null;
        fileInput.value = "";
        fileInfo.textContent = "The selected file is too large.";
        compilerOutput.textContent =
            "ERROR: Source files must be 1 MB or smaller.";
        compileButton.disabled = true;
        statusText.textContent = "File too large";
        return;
    }

    fileInfo.textContent =
        `Selected: ${selectedFile.name} (${selectedFile.size} bytes)`;

    compilerOutput.textContent =
        "Source file ready.\nClick Compile Program to continue.";

    compileButton.disabled = compiling;
    statusText.textContent = "Source selected";
});

clearButton.addEventListener("click", () => {
    if (compiling) return;

    fileInput.value = "";
    selectedFile = null;
    releaseDownload();

    compileButton.disabled = true;
    fileInfo.textContent = "No source file selected.";

    compilerOutput.textContent =
        "Welcome to the NTC Web Compiler.\n\n" +
        "Select a .ntc source file to get started.";

    statusText.textContent = "Ready";
});

compileButton.addEventListener("click", async () => {
    if (!selectedFile || compiling) return;

    const sourceFile = selectedFile;
    const messages = [];

    releaseDownload();
    setBusy(true);
    statusText.textContent = "Compiling...";
    compilerOutput.textContent = "Initializing NTC WebAssembly compiler...";

    try {
        const source = await sourceFile.text();

        if (source.includes("\u0000")) {
            throw new Error("The source file contains a null character.");
        }

        messages.push(`NTC Web Compiler — ${sourceFile.name}`);
        messages.push("Initializing compiler...");

        const module = await createNTCModule({
            noInitialRun: true,

            print: (message) => messages.push(String(message)),
            printErr: (message) => messages.push(String(message)),

            locateFile: (path) =>
                new URL(`compiler/${path}`, document.baseURI).href
        });

        module.FS.writeFile("/program.ntc", source);

        let exitCode = 0;

        try {
            module.callMain(["/program.ntc", "/program.exe"]);
        } catch (error) {
            if (error && error.name === "ExitStatus") {
                exitCode = error.status;
            } else {
                throw error;
            }
        }

        if (exitCode !== 0) {
            throw new Error(`Compiler exited with code ${exitCode}.`);
        }

        let executable;

        try {
            executable = module.FS.readFile("/program.exe");
        } catch {
            throw new Error(
                "No executable was generated. Check the compiler messages."
            );
        }

        if (
            executable.length < 2 ||
            executable[0] !== 0x4D ||
            executable[1] !== 0x5A
        ) {
            throw new Error(
                "The output does not have a valid DOS MZ signature."
            );
        }

        const blob = new Blob(
            [executable],
            { type: "application/octet-stream" }
        );

        downloadUrl = URL.createObjectURL(blob);

        const outputName = sourceFile.name.replace(/\.ntc$/i, ".exe");

        downloadLink.href = downloadUrl;
        downloadLink.download = outputName;
        downloadLink.textContent = `Download ${outputName}`;
        downloadLink.hidden = false;

        messages.push("");
        messages.push("SUCCESS: DOS MZ executable generated.");
        messages.push(`Output: ${outputName}`);
        messages.push(`Size: ${executable.length} bytes`);

        showMessages(messages);
        statusText.textContent = "Compilation successful";
    } catch (error) {
        messages.push("");
        messages.push(`COMPILATION FAILED: ${error.message}`);
        showMessages(messages);
        statusText.textContent = "Compilation failed";
    } finally {
        setBusy(false);
    }
});

window.addEventListener("beforeunload", releaseDownload);
