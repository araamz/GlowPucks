from flask import Blueprint, jsonify

device_blueprint = Blueprint(name="device_blueprint", import_name=__name__)

@device_blueprint.route("/config/<uuid>")
def update_config(uuid):
    
    return jsonify({"config":"device " + uuid})

@device_blueprint.route("/status/<uuid>")
def request_status(uuid):
    
    return jsonify({"status":"device " + uuid})
