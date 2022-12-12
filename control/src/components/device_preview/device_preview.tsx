import { Navigate, useNavigate } from "react-router-dom";
import BatteryWidget from "../battery_widget/battery_widget";
import Graphic from "../graphic/graphic";
import LightActiveWidget from "../light_active_widget/light_active_widget";
import SettingsWidget from "../settings_widget/settings_widget";
import device_preview_styles from "./device_preview.module.css"; 

type device_preview_props = {
    uuid: any;
    // label
    // brightness
    // active
    // color
    battery_widget_disable?: boolean;
    light_active_widget_disable?: boolean;
    settings_widget_disable?: boolean;
}

// Will grab from API for battery_level, light_active, and label - Only ID will be required

export default function DevicePreview(props: device_preview_props) {

    const navigate = useNavigate()

    const click_handler = () => {

        navigate(`/devices/${props.uuid}`)
        
    }

    const test_object = {
        label: "Puck 1",
        battery_level: 23,
        light_active: true
    }

    return (
        <div className={device_preview_styles.device_preview}>
            <div className={device_preview_styles.device_preview_container}>
                <div className={device_preview_styles.label_container}>
                    <p className={device_preview_styles.label}>
                        { test_object.label }
                    </p>
                </div>
                <div className={device_preview_styles.graphic_container}>
                    <Graphic color="#9CCC65" active={false} />
                </div>
                <div className={device_preview_styles.widgets_container}>
                    { !props.battery_widget_disable ? <BatteryWidget level={test_object.battery_level} /> : null }
                    { !props.light_active_widget_disable ? <LightActiveWidget active={test_object.light_active}/> : null }
                    { !props.settings_widget_disable ? <SettingsWidget onClick={click_handler} className={device_preview_styles.settings_widget} /> : null }
                </div>
            </div>
        </div>
    )

}