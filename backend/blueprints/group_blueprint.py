from flask import Blueprint, jsonify, request

group_blueprint = Blueprint(name="group_blueprint", import_name=__name__)

@group_blueprint.route("/config/<uuid>")
def update_config(uuid):
    
    configuration = request.get_json()
    


    return jsonify({"config":"group " + uuid})

@group_blueprint.route("/status/<uuid>")
def request_status(uuid):
    
    return jsonify({"status":"group " + uuid})
