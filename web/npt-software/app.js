"use strict";

const editor = document.getElementById("script-editor");
const output = document.getElementById("compiler-output");
const status = document.getElementById("runtime-status");
const fileInput = document.getElementById("file-input");
const fileName = document.getElementById("file-name");
const compileButton = document.getElementById("run-script");
const downloadButton = document.getElementById("download-exe");

const DEFAULT_SOURCE =
    'PROGRAM\nPRINT "HELLO FROM NPT SOFTWARE!"\nEXIT 0\nEND';

let currentFileName = "UNTITLED.NTC";
let compilerModulePromise = null;
let lastExecutable = null;

function log(message) {
    output.textContent += "\n" + message;
    output.scrollTop = output.scrollHeight;
}

function setStatus(message) {
    status.textContent = "Build: " + message;
}

function invalidateExecutable() {
    lastExecutable = null;
    downloadButton.disabled = true;
}

function downloadFile(name, data, type) {
    const blob = new Blob([data], { type });
    const url = URL.createObjectURL(blob);
    const link = document.createElement("a");

    link.href = url;
    link.download = name;
    document.body.appendChild(link);
    link.click();
    link.remove();

    window.setTimeout(() => URL.revokeObjectURL(url), 1000);
}

async function getCompiler() {
    if (typeof createNTCModule !== "function") {
        throw new Error(
            "The NTC compiler module was not found. " +
            "Check web/compiler/ntc.js and make sure it loads successfully."
        );
    }

    if (!compilerModulePromise) {
        compilerModulePromise = createNTCModule({
            print: (message) => {
                log("[NTC] " + message);
            },
            printErr: (message) => {
                log("[NTC ERROR] " + message);
            }
        }).catch((error) => {
            // Allow a later attempt if module initialization fails.
            compilerModulePromise = null;
            throw error;
        });
    }

    return compilerModulePromise;
}

async function compileScript() {
    if (compileButton.disabled) {
        return;
    }

    const source = editor.value;

    invalidateExecutable();
    output.textContent = "NPT Software 0.1\nInitializing NTC compiler...";
    setStatus("Compiling");
    compileButton.disabled = true;

    try {
        const compiler = await getCompiler();

        // Clear files left over from a previous compilation.
        for (const path of ["/NPTGAME.NTC", "/NPTGAME.EXE"]) {
            try {
                compiler.FS.unlink(path);
            } catch (_) {
                // The file may not exist.
            }
        }

        compiler.FS.writeFile(
            "/NPTGAME.NTC",
            new TextEncoder().encode(source)
        );

        log("Source: " + currentFileName);
        log("Target: DOS MZ executable");
        log("Compiling NPTGAME.NTC...");

        let exitCode = 0;

        try {
            const result = compiler.callMain([
                "/NPTGAME.NTC",
                "/NPTGAME.EXE"
            ]);

            if (typeof result === "number") {
                exitCode = result;
            }
        } catch (error) {
            if (error && error.name === "ExitStatus") {
                exitCode = error.status;
            } else {
                throw error;
            }
        }

        if (exitCode !== 0) {
            log("Compilation failed with exit code " + exitCode + ".");
            log("Review the compiler messages above and correct the source.");
            setStatus("Failed");
            return;
        }

        let executable;

        try {
            executable = compiler.FS.readFile("/NPTGAME.EXE");
        } catch (_) {
            throw new Error(
                "The compiler reported success but did not create " +
                "NPTGAME.EXE. Check the compiler's output and arguments."
            );
        }

        if (!executable || executable.length === 0) {
            throw new Error("The compiler created an empty executable.");
        }

        // Keep a separate copy for exporting.
        lastExecutable = new Uint8Array(executable);
        downloadButton.disabled = false;

        log("Compilation successful.");
        log("Executable: NPTGAME.EXE");
        log("Size: " + lastExecutable.length + " bytes.");
        log("Select Export EXE to download the compiled program.");

        setStatus("Successful");
    } catch (error) {
        setStatus("Error");
        log("ERROR: " + (error.message || String(error)));
        console.error("NPT Software compilation error:", error);
    } finally {
        compileButton.disabled = false;
    }
}

compileButton.addEventListener("click", compileScript);

downloadButton.addEventListener("click", () => {
    if (!lastExecutable) {
        log("No compiled executable is available to export.");
        return;
    }

    downloadFile(
        "NPTGAME.EXE",
        lastExecutable,
        "application/vnd.microsoft.portable-executable"
    );

    log("Exported NPTGAME.EXE.");
});

document.getElementById("new-script").addEventListener("click", () => {
    if (!confirm(
        "Discard the current editor contents? Save your script first if needed."
    )) {
        return;
    }

    editor.value = DEFAULT_SOURCE;
    currentFileName = "UNTITLED.NTC";
    fileName.textContent = currentFileName;
    output.textContent = "New NTC script created.";

    invalidateExecutable();
    setStatus("Ready");
});

document.getElementById("load-script").addEventListener("click", () => {
    fileInput.click();
});

fileInput.addEventListener("change", async () => {
    const file = fileInput.files[0];

    if (!file) {
        return;
    }

    try {
        editor.value = await file.text();
        currentFileName = file.name;
        fileName.textContent = currentFileName;
        output.textContent = "Loaded " + file.name + ".";

        invalidateExecutable();
        setStatus("Ready");
    } catch (error) {
        output.textContent =
            "ERROR: Could not read the selected file.\n" +
            (error.message || String(error));

        setStatus("Error");
    } finally {
        fileInput.value = "";
    }
});

document.getElementById("save-script").addEventListener("click", () => {
    const name = currentFileName.toLowerCase().endsWith(".ntc")
        ? currentFileName
        : currentFileName + ".ntc";

    downloadFile(
        name,
        editor.value,
        "text/plain;charset=utf-8"
    );

    log("Saved source file: " + name);
});

// Any source edit makes the previous build potentially outdated.
editor.addEventListener("input", () => {
    invalidateExecutable();
    setStatus("Source changed");
});

setStatus("Ready");
