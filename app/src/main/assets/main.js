
const readline = require("readline");


try {
    const isNumber = require("is-number");

    console.log("is-number loaded successfully");
    console.log(isNumber(123));
} catch (e) {
    console.error("MODULE ERROR:");
    console.error(e.stack || e.toString());
}


console.log("Node.js terminal started");
console.log("Version:", process.version);
console.log("Architecture:", process.arch);
console.log("");

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
        console.error(error.toString());
    }

    process.stdout.write("> ");
}

rl.on("line", execute);

process.stdout.write("> ");
