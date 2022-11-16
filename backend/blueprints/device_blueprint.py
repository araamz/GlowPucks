from flask import Blueprint, jsonify, request
from enum import Enum
import json
#from ..main import mqtt

device_blueprint = Blueprint(name="device_blueprint", import_name=__name__)

@device_blueprint.route("/config/<uuid>")
def update_config(uuid):

    mqtt_payload = {}

    configuration = request.get_json()
    label = configuration.get('label')
    brightness = configuration.get('brightness')
    active = configuration.get('active')
    color = configuration.get('color')
    mode = configuration.get('mode')
    group = configuration.get('group')

    if (mode == "alternate"):
        mqtt_payload['color'] = {
            "primary_color": color['primary_color'],
            "secondary_color": color['secondary_color']
        }
    else:
        mqtt_payload['color'] = color
    
    mqtt_payload['label'] = label
    mqtt_payload['brightness'] = brightness
    mqtt_payload['active'] = active
    mqtt_payload['mode'] = mode

    #transmission_successful = mqtt.publish(f'/device/config/{uuid}', mqtt_payload)

    return jsonify(True)

@device_blueprint.route("/status/<uuid>")
def request_status(uuid):
    

    return jsonify({"status":"device " + uuid})
