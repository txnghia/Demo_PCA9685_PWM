from arduino.app_utils import *
from arduino.app_bricks.web_ui import WebUI

ui = WebUI()

state = {
    "servo": [90, 90, 90, 90],
    "motor_l": 0,
    "motor_r": 0,
    "led1": 0,
    "led2": 0,
    "analog1": 0.0,
    "analog2": 0.0,
    "sine1": 0,
    "sine2": 0,
    "sine1_freq": 0.5,
    "sine2_freq": 1.0,
    "triac1": 0,
    "triac2": 0,
    "relay1": 0,
    "relay2": 0,
    "last_demo": "none"
}

def send_status(client=None):
    if client:
        ui.send_message("status_update", state, client)
    else:
        ui.send_message("status_update", state)

def servo_set(client, data):
    servo = int(data.get("servo", 0))
    angle = int(data.get("angle", 90))
    angle = max(0, min(180, angle))

    Bridge.call("servo", f"{servo},{angle}")
    state["servo"][servo] = angle
    send_status()

def motor_set(client, data):
    side = data.get("side", "L")
    percent = int(data.get("percent", 0))
    percent = max(0, min(100, percent))

    Bridge.call("motor", f"{side},{percent}")

    if side == "L":
        state["motor_l"] = percent
    else:
        state["motor_r"] = percent

    send_status()

def led_set(client, data):
    led = int(data.get("led", 1))
    percent = int(data.get("percent", 0))
    percent = max(0, min(100, percent))

    Bridge.call("led", f"{led},{percent}")

    if led == 1:
        state["led1"] = percent
    else:
        state["led2"] = percent

    send_status()

def analog_set(client, data):
    out = int(data.get("out", 1))
    volts = float(data.get("volts", 0))
    volts = max(0.0, min(5.0, volts))

    Bridge.call("analog", f"{out},{volts}")

    if out == 1:
        state["analog1"] = volts
    else:
        state["analog2"] = volts

    send_status()

def sine_set(client, data):
    out = int(data.get("out", 1))
    enabled = int(data.get("enabled", 0))
    freq = float(data.get("freq", 0.5))

    cmd = "on" if enabled else "off"
    Bridge.call("sine", f"{out},{cmd},{freq}")

    if out == 1:
        state["sine1"] = enabled
        state["sine1_freq"] = freq
    else:
        state["sine2"] = enabled
        state["sine2_freq"] = freq

    send_status()

def triac_set(client, data):
    triac = int(data.get("triac", 1))
    enabled = int(data.get("enabled", 0))

    cmd = "on" if enabled else "off"
    Bridge.call("triac", f"{triac},{cmd}")

    if triac == 1:
        state["triac1"] = enabled
    else:
        state["triac2"] = enabled

    send_status()

def relay_set(client, data):
    relay = int(data.get("relay", 1))
    enabled = int(data.get("enabled", 0))

    cmd = "on" if enabled else "off"
    Bridge.call("relay", f"{relay},{cmd}")

    if relay == 1:
        state["relay1"] = enabled
    else:
        state["relay2"] = enabled

    send_status()

def demo_run(client, data):
    mode = data.get("mode", "alloff")
    Bridge.call("demo", mode)
    state["last_demo"] = mode

    if mode == "alloff":
        state["servo"] = [90, 90, 90, 90]
        state["motor_l"] = 0
        state["motor_r"] = 0
        state["led1"] = 0
        state["led2"] = 0
        state["analog1"] = 0.0
        state["analog2"] = 0.0
        state["sine1"] = 0
        state["sine2"] = 0
        state["triac1"] = 0
        state["triac2"] = 0
        state["relay1"] = 0
        state["relay2"] = 0

    send_status()

def get_initial_state(client, data):
    send_status(client)

def get_live_status(client, data):
    send_status(client)

ui.on_message("servo_set", servo_set)
ui.on_message("motor_set", motor_set)
ui.on_message("led_set", led_set)
ui.on_message("analog_set", analog_set)
ui.on_message("sine_set", sine_set)
ui.on_message("triac_set", triac_set)
ui.on_message("relay_set", relay_set)
ui.on_message("demo_run", demo_run)

ui.on_message("get_initial_state", get_initial_state)
ui.on_message("get_live_status", get_live_status)

App.run()