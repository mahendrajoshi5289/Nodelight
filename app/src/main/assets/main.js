process.env.NODE_PATH =
    "/data/data/com.example.androidnode/files/node_modules";

require("module").Module._initPaths();

console.log("CWD:", process.cwd());
console.log(
    "NODE_PATH:",
    process.env.NODE_PATH
);

try {

    const isNumber =
        require("is-number");

    console.log(
        "is-number loaded successfully"
    );

    console.log(
        isNumber(123)
    );

} catch (e) {

    console.error(
        "MODULE ERROR:"
    );

    console.error(
        e.stack || e.toString()
    );
}

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
