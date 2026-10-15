
"use strict";

const source = document.getElementById("source");
const fileInput = document.getElementById("fileInput");
const buildOutput = document.getElementById("buildOutput");
const dosOutput = document.getElementById("dosOutput");
const runtimeStatus = document.getElementById("runtimeStatus");
const downloadExe = document.getElementById("downloadExe");

let compiledExe = null;
let currentFileName = "game.ntc";

function log(message) {
  buildOutput.textContent = message;
}

function downloadBlob(blob, filename) {
  const url = URL.createObjectURL(blob);
  const link = document.createElement("a");

  link.href = url;
  link.download = filename;
  document.body.appendChild(link);
  link.click();
  link.remove();

  setTimeout(() => URL.revokeObjectURL(url), 1000);
}

document.getElementById("newProject").addEventListener("click", () => {
  source.value = [
    "PROGRAM",
    'PRINT "WELCOME TO NPT SOFTWARE"',
    "EXIT 0",
    "END"
  ].join("\n");

  currentFileName = "game.ntc";
  compiledExe = null;
  downloadExe.disabled = true;
  log("New NTC script created.");
});

document.getElementById("loadScript").addEventListener("click", () => {
  fileInput.click();
});

fileInput.addEventListener("change", async () => {
  const file = fileInput.files[0];
  if (!file) return;

  try {
    source.value = await file.text();
    currentFileName = file.name;
    compiledExe = null;
    downloadExe.disabled = true;
    log(`Loaded ${file.name}.`);
  } catch (error) {
    log(`Could not load script: ${error.message}`);
  }

  fileInput.value = "";
});

document.getElementById("saveScript").addEventListener("click", () => {
  const blob = new Blob([source.value], {
    type: "text/plain;charset=utf-8"
  });

  downloadBlob(blob, currentFileName);
  log(`Saved a copy of ${currentFileName}.`);
});

source.addEventListener("input", () => {
  compiledExe = null;
  downloadExe.disabled = true;
});

document.getElementById("compile").addEventListener("click", async () => {
  compiledExe = null;
  downloadExe.disabled = true;
  log("Compiling NTC script...");

  try {
    if (typeof createNTCModule !== "function") {
      throw new Error(
        "The NTC WebAssembly compiler was not loaded. " +
        "Check the script path ../compiler/ntc.js and the Pages build."
      );
    }

    const module = await createNTCModule({
      print: (text) => log(`Compiler: ${text}`),
      printErr: (text) => log(`Compiler error: ${text}`)
    });

    const inputPath = "/game.ntc";
    const outputPath = "/game.exe";

    try {
      module.FS.unlink(inputPath);
    } catch (_) {}

    try {
      module.FS.unlink(outputPath);
    } catch (_) {}

    module.FS.writeFile(inputPath, source.value);

    try {
      module.callMain([inputPath, outputPath]);
    } catch (error) {
      // Emscripten may throw an exit-status exception when the
      // command-line program finishes. Only accept normal exit.
      if (!(error && error.name === "ExitStatus" && error.status === 0)) {
        throw error;
      }
    }

    const bytes = module.FS.readFile(outputPath);

    compiledExe = new Blob([bytes], {
      type: "application/vnd.microsoft.portable-executable"
    });

    downloadExe.disabled = false;
    log("Compilation succeeded. The DOS executable is ready to download.");
  } catch (error) {
    log(`Compilation failed: ${error.message}`);
  }
});

downloadExe.addEventListener("click", () => {
  if (!compiledExe) return;

  const filename = currentFileName.replace(/\.ntc$/i, "") + ".exe";
  downloadBlob(compiledExe, filename);
});

document.getElementById("runGame").addEventListener("click", () => {
  log("DOS emulator integration has not been connected yet.");
});

document.getElementById("stopGame").addEventListener("click", () => {
  log("No DOS program is currently running.");
});

runtimeStatus.textContent = "DOS emulator: pending";
dosOutput.textContent =
  "NPT-DOS GAME SCENE\n" +
  "Copyright (C) Neptune Corporation\n\n" +
  "NTC editor initialized.\n" +
  "Compile your script to create a DOS executable.\n" +
  "DOS execution will be enabled in the next milestone.\n\n" +
  "C:\\GAME> _";
