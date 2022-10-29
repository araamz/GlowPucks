import BatteryWidget from "../battery_widget/battery_widget";
import LightActiveWidget from "../light_active_widget/light_active_widget";
import device_preview_styles from "./device_preview.module.css"; 

type device_preview_props = {
    id: any;
    label: String;
    battery_level: Number;
    light_active: boolean;
}

export default function DevicePreview(props: device_preview_props) {

    return (
        <div className={device_preview_styles.device_preview}>
            <div className={device_preview_styles.device_preview_container}>
                <div className={device_preview_styles.label_container}>
                    <p className={device_preview_styles.label}>
                        { props.label }
                    </p>
                </div>
                <div className={device_preview_styles.graphic_container}>
                    <div className={device_preview_styles.graphic} />
                </div>
                <div className={device_preview_styles.widgets_container}>
                    <BatteryWidget level={props.battery_level} />
                    <LightActiveWidget active={props.light_active}/>
                </div>
            </div>
        </div>
    )

}