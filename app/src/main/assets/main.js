const axios = require("axios");

console.log("Android Node.js");
console.log("Node:", process.version);
console.log("Arch:", process.arch);
console.log("Axios:", axios.VERSION);
console.log("");

const readline = require("readline");

const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout,
    terminal: false
});

function execute(command) {
    command = command.trim();

    if (!command) {
        process.stdout.write("> ");
        return;
    }

    try {
        const result = eval(command);

        if (result !== undefined) {
            console.log(result);
        }
    } catch (error) {
        console.error(error.stack || error.toString());
    }

    process.stdout.write("> ");
}

rl.on("line", execute);

process.stdout.write("> ");
