# backend pipelines
# esp32c3 (mqtt) <- (mqtt) backend (rest) <- (rest) frontned [config]
# esp32c3 (mqtt) -> (mqtt) backend (rest) -> (rest) frotend [status]

# backend REST routes
# /device/config/{uuid} (config)
# /device/status/{uuid} (status)
# /default_config 
# /uuid_list

# backend MQTT routes
# esp32c3/device/config/{uuid} (config)
# backend/device/status/{uuid} (status)

# backend functions
# convert_rest_to_mqtt(enum config/rest)
# convert_mqtt_to_rest(emum config/rest)
# load_configuration(file)

# backend enum
# protocol_message_types { config, status }
# modes { still, flash, alternate }

# backend variables 
# STRING_HOLE_FILLER NULL
# INTEGER_HOLE_FILLER -1
# MESSAGE_DELIMITER \t
# TYPE_INDEX 0
# LABEL_INDEX 1
# BRIGHTNESS_INDEX 2
# ACTIVE_INDEX 3
# MODE_INDEX 4
# R1_INDEX 5
# G1_INDEX 6
# B1_INDEX 7
# R2_INDEX 8
# G2_INDEX 9
# B2_INDEX 10
# GROUP_ENABLE_INDEX 11
# GROUP_TARGET_INDEX 12
# CAR_CLEAR_INDEX 13
# CAR_COUNT_INDEX 14
# MESSAGE_FLAGS_COUNT 15

from flask import Flask, jsonify, request
from flask_mqtt import Mqtt
from flask_socketio import SocketIO
import eventlet

import json
from enum import Enum

STRING_HOLE_FILLER = "NULL"
INTEGER_HOLE_FILLER = -1
MESSAGE_DELIMITER = "\t"
TYPE_INDEX = 0
LABEL_INDEX = 1
BRIGHTNESS_INDEX = 2
ACTIVE_INDEX = 3
MODE_INDEX = 4
R1_INDEX = 5
G1_INDEX = 6
B1_INDEX = 7
R2_INDEX = 8
G2_INDEX = 9
B2_INDEX = 10
GROUP_ENABLE_INDEX = 11
GROUP_TARGET_INDEX = 12
CAR_CLEAR_INDEX = 13
CAR_COUNT_INDEX = 14
MESSAGE_FLAGS_COUNT = 15

ESP32C3_DEVICE_CONFIG_TOPIC = "esp32c3/device/config"
ESP32C3_DEVICE_STATUS_TOPIC = "esp32c3/device/status"
BACKEND_DEVICE_STATUS_TOPIC = "backend/device/status"

MQTT_BROKER_HOST = "10.42.0.1"
MQTT_BROKER_PORT = 25565

CONFIG_FILE = "config.json"

class protocol_message_type:
    config = 0
    status = 1

class modes:
    still = 0
    flash = 1 
    alternate = 1

glowpuck_uuids = []

eventlet.monkey_patch()
api = Flask(__name__)
api.config['MQTT_BROKER_URL'] =  MQTT_BROKER_HOST
api.config['MQTT_BROKER_PORT'] = MQTT_BROKER_PORT
api.config['MQTT_TLS_ENABLED'] = False
api.config['SECRET_KEY'] = 'secret!'
socketio = SocketIO(api)
mqtt = Mqtt(app = api)

def construct_protocol_message(
    message_type: protocol_message_type,
    message_label: str,
    message_brightness: int,
    message_active: int,
    message_mode: int,
    message_r1: int,
    message_g1: int,
    message_b1: int,
    message_group_enable: int,
    message_car_clear: int,
    message_r2: int = INTEGER_HOLE_FILLER,
    message_g2: int = INTEGER_HOLE_FILLER,
    message_b2: int = INTEGER_HOLE_FILLER,
    message_group_target: str = STRING_HOLE_FILLER,
):
    protocol_message = ""

    for message_index in range(0, MESSAGE_FLAGS_COUNT):

        if (message_index == TYPE_INDEX):

            segment = f'{message_type}{MESSAGE_DELIMITER}'
            protocol_message = protocol_message + segment

        if (message_index == LABEL_INDEX):

            segment = f'{message_label}{MESSAGE_DELIMITER}'
            protocol_message = protocol_message + segment

        if (message_index == BRIGHTNESS_INDEX):

            segment = f'{message_brightness}{MESSAGE_DELIMITER}'
            protocol_message = protocol_message + segment

        if (message_index == ACTIVE_INDEX):

            segment = f'{message_active}{MESSAGE_DELIMITER}'
            protocol_message = protocol_message + segment

        if (message_index == MODE_INDEX):

            segment = f'{message_mode}{MESSAGE_DELIMITER}'
            protocol_message = protocol_message + segment

        if (message_index == R1_INDEX):

            segment = f'{message_r1}{MESSAGE_DELIMITER}'
            protocol_message = protocol_message + segment

        if (message_index == G1_INDEX):

            segment = f'{message_g1}{MESSAGE_DELIMITER}'
            protocol_message = protocol_message + segment

        if (message_index == B1_INDEX):

            segment = f'{message_b1}{MESSAGE_DELIMITER}'
            protocol_message = protocol_message + segment

        if (message_index == R2_INDEX):

            segment = f'{message_r2}{MESSAGE_DELIMITER}'
            protocol_message = protocol_message + segment

        if (message_index == G2_INDEX):

            segment = f'{message_g2}{MESSAGE_DELIMITER}'
            protocol_message = protocol_message + segment

        if (message_index == B2_INDEX):

            segment = f'{message_b2}{MESSAGE_DELIMITER}'
            protocol_message = protocol_message + segment

        if (message_index == GROUP_ENABLE_INDEX):

            segment = f'{message_group_enable}{MESSAGE_DELIMITER}'
            protocol_message = protocol_message + segment

        if (message_index == GROUP_TARGET_INDEX):

            segment = f'{message_group_target}{MESSAGE_DELIMITER}'
            protocol_message = protocol_message + segment

        if (message_index == CAR_CLEAR_INDEX):

            segment = f'{message_car_clear}{MESSAGE_DELIMITER}'
            protocol_message = protocol_message + segment

        if (message_index == CAR_COUNT_INDEX):

            segment = f'{INTEGER_HOLE_FILLER}{MESSAGE_DELIMITER}'
            protocol_message = protocol_message + segment

    return protocol_message
def parse_status_message(message: str):

    message_values = message.split(MESSAGE_DELIMITER)

    parsed_type = None
    parsed_label = None
    parsed_brightness = None
    parsed_active = None
    parsed_mode = None
    parsed_r1 = None
    parsed_g1 = None
    parsed_b1 = None
    parsed_r2 = None
    parsed_g2 = None
    parsed_b2 = None
    parsed_group_enable = None
    parsed_group_target = None
    parsed_car_clear = None
    parsed_car_count = None

    for message_index in range(0, MESSAGE_FLAGS_COUNT):

        if (message_index == TYPE_INDEX):

            parsed_type = message_values[message_index]

        if (message_index == LABEL_INDEX):

            parsed_label = message_values[message_index]

        if (message_index == BRIGHTNESS_INDEX):

            parsed_brightness = message_values[message_index]

        if (message_index == ACTIVE_INDEX):

            parsed_active = message_values[message_index]

        if (message_index == MODE_INDEX):

            parsed_mode = message_values[message_index]

        if (message_index == R1_INDEX):

            parsed_r1 = message_values[message_index]

        if (message_index == G1_INDEX):

            parsed_g1 = message_values[message_index]

        if (message_index == B1_INDEX):

            parsed_b1 = message_values[message_index]

        if (message_index == R2_INDEX):

            parsed_r2 = message_values[message_index]

        if (message_index == G2_INDEX):

            parsed_g2 = message_values[message_index]

        if (message_index == B2_INDEX):

            parsed_b2 = message_values[message_index]

        if (message_index == GROUP_ENABLE_INDEX):

            parsed_group_enable = message_values[message_index]

        if (message_index == GROUP_TARGET_INDEX):

            parsed_group_target = message_values[message_index]

        if (message_index == CAR_CLEAR_INDEX):

            parsed_car_clear = message_values[message_index]

        if (message_index == CAR_COUNT_INDEX):

            parsed_car_count = message_values[message_index]

    parsed_status_message = {
        "type": parsed_type,
        "label": parsed_label,
        "brightness": parsed_brightness,
        "active": parsed_active,
        "mode": parsed_mode,
        "r1": parsed_r1,
        "g1": parsed_g1,
        "b1": parsed_b1,
        "r2": parsed_r2,
        "g2": parsed_g2,
        "b2": parsed_b2,
        "group_enable": parsed_group_enable,
        "group_target": parsed_group_target,
        "car_clear": parsed_car_clear,
        "car_count": parsed_car_count
    }

    return parsed_status_message
def publish_configuration_update(
    uuid: str,
    label: str,
    brightness: int,
    active: int,
    mode: int,
    r1: int,
    g1: int,
    b1: int,
    group_enable: int,
    car_clear: int,
    r2: int = INTEGER_HOLE_FILLER,
    g2: int = INTEGER_HOLE_FILLER,
    b2: int = INTEGER_HOLE_FILLER,
    group_target: str = STRING_HOLE_FILLER
):

    if (group_enable == 1):
        group_target = group_target

    config_protocol_message = construct_protocol_message(
        message_type = protocol_message_type.config,
        message_label = label,
        message_brightness = brightness,
        message_active = active,
        message_mode = mode,
        message_r1 = r1,
        message_g1 = g1,
        message_b1 = b1,
        message_r2 = r2,
        message_g2 = g2,
        message_b2 = b2,
        message_group_enable = group_enable,
        message_group_target = group_target,
        message_car_clear = car_clear
    )

    topic = f'{ESP32C3_DEVICE_CONFIG_TOPIC}/{uuid}'
    mqtt.publish(topic, config_protocol_message)

    return config_protocol_message
def publish_status_request(uuid: str):
    
    status_protocol_message = construct_protocol_message(
        message_type = protocol_message_type.status,
        message_label = STRING_HOLE_FILLER,
        message_brightness = INTEGER_HOLE_FILLER,
        message_active = INTEGER_HOLE_FILLER,
        message_mode = INTEGER_HOLE_FILLER,
        message_r1 = INTEGER_HOLE_FILLER,
        message_g1 = INTEGER_HOLE_FILLER,
        message_b1 = INTEGER_HOLE_FILLER,
        message_r2 = INTEGER_HOLE_FILLER,
        message_g2 = INTEGER_HOLE_FILLER,
        message_b2 = INTEGER_HOLE_FILLER,
        message_group_enable = INTEGER_HOLE_FILLER,
        message_group_target = STRING_HOLE_FILLER,
        message_car_clear = INTEGER_HOLE_FILLER
    )

    topic = f'{ESP32C3_DEVICE_STATUS_TOPIC}/{uuid}'
    mqtt.publish(topic, status_protocol_message)

    return status_protocol_message

def load_config(file: str):

    glowpucks_config_file = open(file)
    glowpucks_config = json.load(glowpucks_config_file)

    devices = glowpucks_config['devices']
    device_count = 0

    ## go through each uuid and send to esp32c3 configuration
    for device in devices:

        device_count = device_count + 1

        uuid = device['uuid']
        label = device['configuration']['label']
        brightness = device['configuration']['brightness']
        active = device['configuration']['active']
        mode = device['configuration']['mode']
        r1 = device['configuration']['r1']
        g1 = device['configuration']['g1']
        b1 = device['configuration']['b1']
        r2 = device['configuration']['r2']
        g2 = device['configuration']['g2']
        b2 = device['configuration']['b2']
        group_enable = device['configuration']['group_enable']
        group_target = device['configuration']['group_target']
        car_clear = device['configuration']['car_clear']

        glowpuck_uuids.append(uuid)
        publish_configuration_update(
            uuid = uuid,
            label = label,
            brightness = brightness,
            active = active,
            mode = mode,
            r1 = r1,
            g1 = g1,
            b1 = b1,
            r2 = r2,
            g2 = g2,
            b2 = b2,
            group_enable = group_enable,
            group_target = group_target,
            car_clear = car_clear
        )

    return glowpucks_config 

@api.route('/default_config', methods=['GET'])
def deliver_default_config():

    glowpucks_config_file = open(CONFIG_FILE)
    glowpucks_config = jsonify(glowpucks_config_file)

    return glowpucks_config

@socketio.on('config_update')
def update_esp32c3_device_config(uuid, config):

    device_config = json.loads(config)
    label = device_config['label']
    brightness = device_config['brightness']
    active = device_config['active']
    mode = device_config['mode']
    r1 = device_config['r1']
    g1 = device_config['g1']
    b1 = device_config['b1']
    r2 = device_config['r2']
    g2 = device_config['g2']
    b2 = device_config['b2']
    group_enable = device_config['group_enable']
    group_target = device_config['group_target']
    car_clear = device_config['car_clear']

    publish_configuration_update(
        uuid = uuid,
        label = label,
        brightness = brightness,
        active = active,
        mode = mode,
        r1 = r1,
        g1 = g1,
        b1 = b1,
        r2 = r2,
        g2 = g2,
        b2 = b2,
        group_enable = group_enable,
        group_target = group_target,
        car_clear = car_clear,
    )

    return jsonify(201)

@socketio.on('status_request')
def request_esp32c3_device_status(uuid):
    
    publish_status_request(uuid)

    return 

@mqtt.on_connect()
def initialize_mqtt(self, client, userdata, message):

    for uuid in glowpuck_uuids:

        topic = f'{BACKEND_DEVICE_STATUS_TOPIC}/{uuid}'
        mqtt.subscribe(topic)
        
@mqtt.on_message()
def handle_messages(client, userdata, message):

    topic = message.topic
    payload = message.payload.decode()

    uuid = str(topic).split(BACKEND_DEVICE_STATUS_TOPIC)[1].replace("/","")

    status_message = parse_status_message(
        message = str(payload)
    )

    socketio.emit("status_request", uuid, status_message)


if __name__ == '__main__':
    socketio.run(api)