var exec = require('child_process').exec;

function dirtest() {

    exec('find ../kernel', function (error, stdout, stderr) {
        console.log('kernel stdout: ' + stdout);
        console.log('kernel stderr: ' + stderr);
        if (error !== null) {
            console.log('error: ' + error);
        }
    });

    exec('find ../userland/libraries', function (error, stdout, stderr) {
        console.log('libs stdout: ' + stdout);
        console.log('libs stderr: ' + stderr);
        if (error !== null) {
            console.log('error: ' + error);
        }
    });

    exec('find ../ak', function (error, stdout, stderr) {
        console.log('ak stdout: ' + stdout);
        console.log('ak stderr: ' + stderr);
        if (error !== null) {
            console.log('error: ' + error);
        }
    });

}

function main() {
    console.log('Running dirtest');

    dirtest();
}

main();