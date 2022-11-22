from flask import Flask, jsonify, request
from flask_mqtt import Mqtt

from .device import construct_configuration

api = Flask(__name__)
api.config['MQTT_BROKER_URL'] =  '10.42.0.1'
api.config['MQTT_BROKER_PORT'] = 25565
api.config['MQTT_TLS_ENABLED'] = False
mqtt = Mqtt(app = api)

@api.route('/device/config/<uuid>', methods=['POST'])
def process_device_configuration(uuid):

    configuration_request = request.get_json()
    label = configuration_request['label']
    color = configuration_request['color']
    brightness = configuration_request['brightness']
    active = configuration_request['active']
    mode = configuration_request['mode']
    group_enable = configuration_request['group_enable']
    group_target = configuration_request['group_target']

    configuration = construct_configuration(
        label = label,
        color = color,
        brightness = brightness,
        active = active,
        mode = mode,
        group_enable = group_enable,
        group_target = group_target
    )

    topic = f'/device/config/{uuid}'

    successful = mqtt.publish(
        topic = topic,
        payload = configuration
    )[0]

    return jsonify(successful)

@api.route('/device/status/<uuid>', methods=['GET'])
def process_device_status(uuid):

    topic = f'/device/status/{uuid}'
    status = None

    mqtt.subscribe(topic)
    
    @mqtt.on_topic(topic)
    def view_status(client, userdata, message):
        status = message.payload.decode()

    mqtt.unsubscribe(topic)

    print(status)

'''
mqtt.subscribe('/device/config/4e51c2ac-9d96-4f3c-932c-1069741285fd')
@mqtt.on_topic('/device/config/4e51c2ac-9d96-4f3c-932c-1069741285fd')
def view_msg(client, userdata, message):
    print(message.payload.decode())
'''