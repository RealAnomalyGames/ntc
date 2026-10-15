const editor = document.getElementById("script-editor");
const output = document.getElementById("compiler-output");
const status = document.getElementById("runtime-status");
const fileInput = document.getElementById("file-input");
const fileName = document.getElementById("file-name");
const dosScreen = document.getElementById("dos-screen");

let currentFileName = "UNTITLED.NTC";
let emulator = null;
let compilerModulePromise = null;

function log(message) {
    output.textContent += "\n" + message;
    output.scrollTop = output.scrollHeight;
}

function setStatus(message) {
    status.textContent = "Runtime: " + message;
}

function downloadFile(name, data, type) {
    const blob = new Blob([data], { type });
    const url = URL.createObjectURL(blob);
    const link = document.createElement("a");

    link.href = url;
    link.download = name;
    link.click();

    setTimeout(() => URL.revokeObjectURL(url), 1000);
}

function cleanDosScreen() {
    if (emulator) {
        emulator.stop();
        emulator = null;
    }

    dosScreen.replaceChildren();
}

async function getCompiler() {
    if (typeof createNTCModule !== "function") {
        throw new Error(
            "The NTC compiler module was not found. Check web/compiler/ntc.js."
        );
    }

    if (!compilerModulePromise) {
        compilerModulePromise = createNTCModule({
            print: (text) => log(text),
            printErr: (text) => log("COMPILER: " + text)
        });
    }

    return compilerModulePromise;
}

async function compileAndRun() {
    const source = editor.value;

    output.textContent = "NPT Software 0.1\nPreparing NTC compiler...";
    setStatus("Compiling");

    document.getElementById("run-script").disabled = true;

    try {
        const compiler = await getCompiler();

        // Remove files from an earlier compile, if present.
        for (const path of ["/NPTGAME.NTC", "/NPTGAME.EXE"]) {
            try {
                compiler.FS.unlink(path);
            } catch (_) {
                // The file may not exist yet.
            }
        }

        compiler.FS.writeFile(
            "/NPTGAME.NTC",
            new TextEncoder().encode(source)
        );

        log("\nCompiling NPTGAME.NTC...");

        let exitCode = 0;

        try {
            exitCode = compiler.callMain([
                "/NPTGAME.NTC",
                "/NPTGAME.EXE"
            ]);
        } catch (error) {
            if (error && error.name === "ExitStatus") {
                exitCode = error.status;
            } else {
                throw error;
            }
        }

        if (exitCode !== 0) {
            throw new Error("NTC compilation failed with exit code " + exitCode);
        }

        const executable = compiler.FS.readFile("/NPTGAME.EXE");

        log("Compilation completed.");
        log("Executable size: " + executable.length + " bytes.");
        log("Starting DOS emulator...");

        await cleanDosScreen();

        emulator = Dos(dosScreen, {
            dosboxConf: [
                "[autoexec]",
                "mount c .",
                "c:",
                "NPTGAME.EXE"
            ].join("\n"),
            initFs: [{
                path: "NPTGAME.EXE",
                contents: executable
            }],
            autoStart: true,
            kiosk: true,
            theme: "retro",
            renderAspect: "4/3",
            imageRendering: "pixelated",
            onEvent: (event) => {
                if (event === "ci-ready") {
                    setStatus("Program running");
                }
            }
        });

        setStatus("Starting DOS");
        log("If the program does not start, check the emulator and compiler logs.");
    } catch (error) {
        setStatus("Error");
        log("\nERROR: " + (error.message || String(error)));
        console.error(error);
    } finally {
        document.getElementById("run-script").disabled = false;
    }
}

document.getElementById("run-script").addEventListener("click", compileAndRun);

document.getElementById("new-script").addEventListener("click", () => {
    if (!confirm("Discard the current editor contents? Save your script first if needed.")) {
        return;
    }

    editor.value = 'PROGRAM\nPRINT "HELLO FROM NPT SOFTWARE!"\nEXIT 0\nEND';
    currentFileName = "UNTITLED.NTC";
    fileName.textContent = currentFileName;
    output.textContent = "New NTC script created.";
    setStatus("Ready");
});

document.getElementById("load-script").addEventListener("click", () => {
    fileInput.click();
});

fileInput.addEventListener("change", async () => {
    const file = fileInput.files[0];
    if (!file) return;

    editor.value = await file.text();
    currentFileName = file.name;
    fileName.textContent = currentFileName;
    output.textContent = "Loaded " + file.name;
    setStatus("Ready");

    fileInput.value = "";
});

document.getElementById("save-script").addEventListener("click", () => {
    const name = currentFileName.toLowerCase().endsWith(".ntc")
        ? currentFileName
        : currentFileName + ".ntc";

    downloadFile(name, editor.value, "text/plain;charset=utf-8");
});

setStatus("Ready");
