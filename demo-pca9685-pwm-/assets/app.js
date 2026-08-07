console.log("PCA9685 demo app.js loaded");

let socket;
let statusText;
let errorContainer;

document.addEventListener("DOMContentLoaded", () => {
    statusText = document.getElementById("status");
    errorContainer = document.getElementById("error-container");

    socket = io(`http://${window.location.host}`);

    socket.on("connect", () => {
        statusText.textContent = "Status: socket connected";
        socket.emit("get_initial_state", {});
    });

    socket.on("disconnect", () => {
        statusText.textContent = "Status: disconnected";
    });

    socket.on("connect_error", (err) => {
        if (errorContainer) {
            errorContainer.textContent = "Socket error: " + err.message;
        }
    });

    socket.on("status_update", (data) => {
        statusText.textContent = JSON.stringify(data, null, 2);
    });
});

function setServo(servo, angle) {
    socket.emit("servo_set", {
        servo: parseInt(servo),
        angle: parseInt(angle)
    });
}

function setMotor(side, percent) {
    socket.emit("motor_set", {
        side: side,
        percent: parseInt(percent)
    });
}

function setLed(led, percent) {
    socket.emit("led_set", {
        led: parseInt(led),
        percent: parseInt(percent)
    });
}

function setAnalog(out, volts) {
    document.getElementById(out === 1 ? "a1" : "a2").textContent = volts + "V";

    socket.emit("analog_set", {
        out: parseInt(out),
        volts: parseFloat(volts)
    });
}

function sine(out, enabled) {
    const freq = out === 1
        ? parseFloat(document.getElementById("sine1freq").value)
        : parseFloat(document.getElementById("sine2freq").value);

    socket.emit("sine_set", {
        out: parseInt(out),
        enabled: enabled ? 1 : 0,
        freq: freq
    });
}

function setRelay(relay, state) {
    socket.emit("relay_set", {
        relay: parseInt(relay),
        enabled: state === "on" ? 1 : 0
    });
}

function setTriac(triac, state) {
    socket.emit("triac_set", {
        triac: parseInt(triac),
        enabled: state === "on" ? 1 : 0
    });
}

function demo(mode) {
    socket.emit("demo_run", {
        mode: mode
    });
}