import { useEffect, useState } from "react";
import { Navigate, useNavigate } from "react-router-dom";
import BatteryWidget from "../battery_widget/battery_widget";
import Graphic from "../graphic/graphic";
import LightActiveWidget from "../light_active_widget/light_active_widget";
import SettingsWidget from "../settings_widget/settings_widget";
import device_preview_styles from "./device_preview.module.css"; 

type device_preview_props = {
    uuid: any;
    light_active_widget_disable?: boolean;
    settings_widget_disable?: boolean;
}

// Will grab from API (socketio) for light_active, and label - Only UUID will be required
// Will grab from API (socketio) for r1, g1, b1 

export default function DevicePreview(props: device_preview_props) {

    const navigate = useNavigate()

    const [r1, set_r1] = useState<Number>(0)
    const [g1, set_g1] = useState<Number>(0)
    const [b1, set_b1] = useState<Number>(0)

    const click_handler = () => {
  
        navigate(`/devices/${props.uuid}`)
        
    }


    const test_object = {
        "label": "GP1",
        "brightness": 100,
        "active": 0,
        "mode": 0,
        "r1": 255,
        "g1": 0,
        "b1": 0,
        "r2": -1,
        "g2": -1,
        "b2": -1,
        "group_enable": 0,
        "group_target": "NULL",
        "car_clear": 0,
        "car_count": -1
    }

    useEffect(() => {

    }, [])

    

    return (
        <div className={device_preview_styles.device_preview}>
            <div className={device_preview_styles.device_preview_container}>
                <div className={device_preview_styles.label_container}>
                    <p className={device_preview_styles.label}>
                        { test_object.label }
                    </p>
                </div>
                <div className={device_preview_styles.graphic_container}>
                    <Graphic color="rgba(0,255,0,100)" active={false} />
                </div>
                <div className={device_preview_styles.widgets_container}>
                    { !props.light_active_widget_disable ? <LightActiveWidget active={test_object.light_active}/> : null }
                    { !props.settings_widget_disable ? <SettingsWidget onClick={click_handler} className={device_preview_styles.settings_widget} /> : null }
                </div>
            </div>
        </div>
    )

}