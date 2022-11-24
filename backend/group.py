import json

def construct_group_configuration(brightness, active, color, mode, group_enable, group_target):
    
    mqtt_configuration = dict()

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
