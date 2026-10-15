const editor = document.getElementById("script-editor");
const output = document.getElementById("compiler-output");
const status = document.getElementById("runtime-status");
const fileInput = document.getElementById("file-input");
const fileName = document.getElementById("file-name");
const dosScreen = document.getElementById("dos-screen");

console.log("js-dos diagnostic:", {
    Dos: typeof window.Dos,
    emulators: typeof window.emulators,
    page: window.location.href
});

pathPrefix: "./js-dos/emulators/",

window.addEventListener("load", () => {
    if (typeof window.Dos !== "function") {
        console.error(
            "js-dos failed to expose window.Dos. Check the Network tab for js-dos.js."
        );
    } else {
        console.log("js-dos player loaded successfully.");
    }
});

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

async function cleanDosScreen() {
    if (emulator) {
        const previousEmulator = emulator;
        emulator = null;
        await previousEmulator.stop();
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
            print: (message) => {
                log("[NTC] " + message);
            },
            printErr: (message) => {
                log("[NTC ERROR] " + message);
            }
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
            log("NTC compiler returned exit code: " + exitCode);
            log("Check the compiler messages above for the actual error.");

            setStatus("Compilation failed");
            return;
        }

        const executable = compiler.FS.readFile("/NPTGAME.EXE");

        log("Compilation completed.");
        log("Executable size: " + executable.length + " bytes.");
        log("Starting DOS emulator...");

        
        await cleanDosScreen();

        if (typeof Dos !== "function") {
            throw new Error(
                "js-dos did not load. Check the js-dos script URL and browser console."
            );
        }

        setStatus("Starting DOS emulator");

        emulator = Dos(dosScreen, {
            dosboxConf: `
        [sdl]
        autolock=false

        [dos]
        xms=true
        ems=true

        [autoexec]
        @echo off
        mount c .
        c:
        NPTGAME.EXE
        `,
            initFs: [
                {
                    path: "NPTGAME.EXE",
                    contents: new Uint8Array(executable)
                }
            ],
            autoStart: true,
            kiosk: true,
            theme: "retro",
            renderAspect: "4/3",
            imageRendering: "pixelated",
            onEvent: (event) => {
                if (event === "emu-ready") {
                    setStatus("Emulator ready");
                    log("DOS emulator initialized.");
                }

                if (event === "ci-ready") {
                    setStatus("DOS session active");
                    log("DOS command interface initialized.");
                }
            }
        });

        log("DOS emulator launched.");


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
