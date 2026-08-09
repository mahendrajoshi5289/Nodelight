console.log("================================");
console.log("Node.js Mobile started!");
console.log("================================");

console.log("Node version:", process.version);
console.log("Platform:", process.platform);
console.log("Architecture:", process.arch);

setInterval(() => {
    console.log("Node is alive:", new Date().toISOString());
}, 5000);
