import Widget from "../widget/widget";
import battery_widget_styles from "./battery_widget.module.css"; 
 
type battery_widget_props = {
    level: Number
}

export default function BatteryWidget(props: battery_widget_props) {

    const show_battery_level_text = () => {
        return `${props.level}%`
    }

    const show_dynamic_battery_icon = () => {

        let very_low_battery_threshold: Number = 10;
        let low_battery_threshold: Number = 25;
        let medium_battery_threshold: Number = 50;
        let high_battery_threshold: Number = 75;
        let full_battery_threshold: Number = 100;

        if ( (full_battery_threshold >= props.level) && (high_battery_threshold < props.level) ) {
            return "battery_full_alt"
        } else if ( (high_battery_threshold >= props.level) && (medium_battery_threshold < props.level) ) {
            return "battery_horiz_075"
        } else if ( (medium_battery_threshold >= props.level) && (low_battery_threshold < props.level) ) {
            return "battery_horiz_050"
        } else if ( (low_battery_threshold >= props.level) && (very_low_battery_threshold < props.level) ) {
            return "battery_low"
        } else {
            return "battery_very_low"
        }

    }

    return (
        <Widget>
            <p className={battery_widget_styles.battery_text}>
                { show_battery_level_text() }
            </p>
            <div className={battery_widget_styles.battery_icon}>
                <span className="material-symbols-outlined">
                    { show_dynamic_battery_icon() }
                </span>
            </div>
        </Widget>
    )

}