import json

def construct_device_configuration(label, brightness, active, color, mode, group_enable, group_target):

    mqtt_configuration = dict()

    mqtt_configuration['label'] = label
    mqtt_configuration['brightness'] = brightness
    mqtt_configuration['active'] = active
    mqtt_configuration['color'] = construct_color(mode, color)
    mqtt_configuration['group_enable'] = group_enable
    mqtt_configuration['group_target'] = group_target

    return json.dumps(mqtt_configuration)

def construct_color(mode, color):

    if (mode == "alternate"):
        alternate_colors = dict()
        alternate_colors['primary_color'] = color['primary_color']
        alternate_colors['secondary_color'] = color['secondary_color']

        return alternate_colors

    return color

message_test = {
    "label":"GP 1",
    "brightness": 100,
    "active": True,
    "mode": "alternate",
    "color": {
        "primary_color": "red",
        "secondary_color": "blue"
    },
    "group_enable": True,
    "group_target": "4bef278d-eefb-44c5-be0d-99a6eae2b5ba"
}

def construct_device_status(mqtt_status):

    label = mqtt_status['label']
    brightness = mqtt_status['brightness']
    active = mqtt_status['active']
    mode = mqtt_status['mode']
    color = construct_color(mode, mqtt_status['color'])
    group_enable = mqtt_status['group_enable']
    group_target = mqtt_status['group_target']
    car_count = mqtt_status['car_count']

    device_status_response = None

    if (group_enable):

        device_status_response = {
            "label": label,
            "brightness": brightness,
            "active": active, 
            "mode": mode,
            "color": color,
            "group_enable": group_enable,
            "group_target": group_target,
            "car_count": car_count
        }

    else:

        device_status_response = {
            "label": label,
            "brightness": brightness,
            "active": active, 
            "mode": mode,
            "color": color,
            "group_enable": group_enable,
            "car_count": car_count
        }

    return device_status_response