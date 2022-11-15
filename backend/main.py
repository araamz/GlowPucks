from flask import Flask
from flask_mqtt import Mqtt

from blueprints.device_blueprint import device_blueprint
from blueprints.group_blueprint import group_blueprint

api = Flask(__name__)
mqtt = Mqtt(app=api)
api.config['MQTT_BROKER_URL'] = None
api.config['MQTT_BROKER_PORT'] = 0
api.config['MQTT_USERNAME'] = ''
api.config['MQTT_PASSWORD'] = ''
api.config['MQTT_KEEPALIVE'] = ''
api.config['MQTT_TLS_ENABLED'] = False

api.register_blueprint(device_blueprint, url_prefix="/device")
api.register_blueprint(group_blueprint, url_prefix="/group")

if __name__ == "__main__":
    
    api.run(host="127.0.0.1", port=3000, debug=True, use_reloader=False)