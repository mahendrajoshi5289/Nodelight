const readline = require("readline");

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
